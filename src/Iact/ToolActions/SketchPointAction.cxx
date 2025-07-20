#include "Iact/ToolActions/SketchPointAction.hxx"

#include <Geom_CartesianPoint.hxx>
#include <Message.hxx>
#include <ElSLib.hxx>

#include "Iact/Visual/Marker.hxx"
#include "Iact/WorkspaceController.hxx"

// Constructor

SketchPointAction::SketchPointAction(const Handle(SketchEditorTool)& sketchEditorTool)
	: ToolAction(),
	myPoint(),
	_Marker(nullptr),
	_SnapHandler(nullptr),
	_SketchEditorTool(sketchEditorTool),
	_EnablePointMerge(true)
{}

bool SketchPointAction::onMouseMove(const Handle(MouseEventData)& data)
{
	if(!m_isFinished)
	{
		_EnsureMarker();
		ProcessMouseInput(data);

		_Marker->Set(ElSLib::Value(_Point.X(), _Point.Y(), _SketchEditorTool->GetSketch()->Plane()));

		// Emit the signal to preview the point
		Preview(this);

		return ToolAction::onMouseMove(data);
	}
	return false;
}

bool SketchPointAction::onMouseRelease(const Handle(MouseEventData)& data)
{
	if(!m_isFinished)
	{
		ProcessMouseInput(data);
		m_isFinished = true;
		Finished(this);
	}
	return true;
}

bool SketchPointAction::onMousePress(const Handle(MouseEventData)& data)
{
	return true;
}

void SketchPointAction::ProcessMouseInput(const Handle(MouseEventData)& data)
{
	myPoint = data->PointOnPlane();

	double u = 0, v = 0;
	ElSLib::Parameters(_SketchEditorTool->GetSketch()->Plane(), myPoint, u, v);

	gp_Pnt2d sketchPoint(u, v);

	_Point = sketchPoint;
}

void SketchPointAction::_EnsureMarker()
{
	if(_Marker.IsNull())
	{
		_Marker = new Marker(MarkerStyle::Bitmap, Marker::PlusImage());
		_Marker->SetWorkspaceController(m_workspaceController);

		Add(_Marker);
	}
}
