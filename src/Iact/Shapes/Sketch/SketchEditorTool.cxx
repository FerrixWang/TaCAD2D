#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"

#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementPoint.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchFactory.hxx"
#include "Iact/Framework/Tool.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/Shapes/Sketch/SketchTool.hxx"

namespace
{
const std::map<int, Handle(SketchSegment)> ToIndexedDictionary(const std::vector<Handle(SketchSegment)>& segments)
{
	std::map<int, Handle(SketchSegment)> result;
	int i = 0;
	for(const auto& segment : segments)
		if(segment)
			result[i++] = segment;
	return result;
}
} // namespace

Signal<void()> SketchEditorTool::ConstraintCreated;

void SketchEditorTool::StartTool(const Handle(SketchTool)& tool)
{
	StopTool();

	// Free own message, so tool can set it
	SetHintMessage("");

	tool->setWorkspaceController(m_workspaceController);
	if(!tool->Start(this))
	{
		_UpdateStatusText();
		return;
	}
	_CurrentTool = tool;
}

void SketchEditorTool::StopTool()
{
	if(_CurrentTool.IsNull())
	{
		return;
	}

	_CurrentTool->Stop();
	_CurrentTool.Nullify();
}

void SketchEditorTool::FinishSegmentCreation(const std::map<int, gp_Pnt2d>& points, 
											 const std::vector<int>& mergePointIndices, 
											 const std::vector<Handle(SketchSegment)>& segments, 
											 int continueWithPoint)
{
	auto [pointMap, segmentMap, _] = mySketch->AddElements(points, mergePointIndices, ToIndexedDictionary(segments));
	CommitCommand();

	if(_ContinuesSegmentCreation && continueWithPoint >= 0)
	{
		auto it = pointMap.find(continueWithPoint);
		if(it == pointMap.end()) return;

		Handle(SketchSegmentCreator) creator = Handle(SketchSegmentCreator)::DownCast(_CurrentTool);
		if(creator.IsNull()) return;

		if(creator->Continue(it->second))
		{
			// Segment creation will continue
			_LastContinuesSegmentStartPoint = it->second;
			return;
		}
	}

	StopTool();
}

void SketchEditorTool::UpdateSelections()
{
	auto workspaceConller = GetWorkspaceController();
	if(workspaceConller.IsNull()) return ;

	_SelectedSegments.clear();
	_SelectedSegmentIndices.clear();
	_SelectedSegmentPoints.clear();

	auto context = workspaceConller->workspace()->AisContext();
	context->InitSelected();

	while(context->MoreSelected())
	{
		auto owner = context->SelectedInteractive()->GetOwner();
		auto pres = Handle(TPrsStd_AISPresentation)::DownCast(owner);
		Handle(TDataStd_Name) name;
		pres->Label().FindAttribute(TDataStd_Name::GetID(), name);
		if(name->Get() == "SketchElementSegment")
		{
			auto element = SketchFactory::Wrap<SketchElementSegment>(pres->Label());
			auto segmentLabel = element->ReferenceAttribute()->Get();
			auto segment = SketchFactory::Wrap<SketchSegment>(segmentLabel);

			if(!segment.IsNull())
			{
				_SelectedSegments.push_back(segment);
				_SelectedSegmentIndices.push_back(segmentLabel.Tag());
			}
		}
		else if(name->Get() == "SketchElementPoint")
		{
			_SelectedSegmentPoints.push_back(pres->Label().Tag());		
		}

		context->NextSelected();
	}
}
