#ifndef SketchEditorTool_Header_File_
#define SketchEditorTool_Header_File_

#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Iact/Framework/Tool.hxx"

class SketchTool;

class SketchEditorTool : public Tool
{
public:
	SketchEditorTool(const Handle(Sketch)& sketch)
		: Tool()
		, mySketch(sketch)
	{
	}

	~SketchEditorTool() override = default;

	bool onStart() override
	{
		return true;
	}

	void onStop() override
	{
		// Cleanup the tool
	}

	bool onCancel() override
	{
		// Handle cancel action
		return true;
	}

	// GetSketch
	Handle(Sketch) GetSketch() const
	{
		return mySketch;
	}

	// CurrentTool
	Handle(SketchTool) CurrentTool() const
	{
		return _CurrentTool;
	}

	void StartTool(const Handle(SketchTool)& tool);

	void StopTool();

	bool StartToolAction(const Handle(ToolAction)& toolAction)
	{
		// Forward for sketch tool
		return startAction(toolAction, false);
	}

	void StopToolAction(const Handle(ToolAction)& toolAction)
	{
		stopAction(toolAction);
		_LastContinuesSegmentStartPoint = -1;
		_ContinuesSegmentCreation = false;
	}

	template<typename T>
	bool StartSegmentCreation(bool continuesCreation = false);

	void FinishSegmentCreation(const std::map<int, gp_Pnt2d>& points,
							   const std::vector<int>& mergePointIndices,
							   const std::vector<Handle(SketchSegment)>& segments,
							   int continueWithPoint = -1);

	template<typename T>
	bool CanCreateConstraint()
	{
		return true;
	}

	template<typename T>
	std::vector<Handle(SketchConstraint)> CreateConstraint()
	{
		if(!CanCreateConstraint<T>()) return {};

		OpenCommand();
		UpdateSelections();

		auto constraints = T::Create(mySketch, {}, _SelectedSegmentIndices);
		if(!constraints.empty())
		{
			mySketch->AddElements({}, {}, {}, constraints);
			mySketch->SolveConstraints(false);
			mySketch->SolveConstraints(true);
			CommitCommand();

			UpdateSelections();
		}

		ConstraintCreated();
		return constraints;
	}

	void UpdateSelections();

	std::vector<Handle(SketchSegment)>& SelectedSegments()
	{
		return _SelectedSegments;
	}

	void _UpdateStatusText() {}

public:
	static Signal<void()> ConstraintCreated;

private:
	std::vector<Handle(SketchSegment)> _SelectedSegments;
	std::vector<int> _SelectedSegmentIndices;
	std::vector<int> _SelectedSegmentPoints;
	Handle(Sketch) mySketch;
	Handle(SketchTool) _CurrentTool;
	int _LastContinuesSegmentStartPoint = -1;
	bool _ContinuesSegmentCreation = false;

	const std::string UnselectedStatusText = "";
	const std::string MixedSelectedStatusText = "Multiple items selected.";
	const std::string SegmentSelectedStatusText = "Segment {0} selected.";
	const std::string MultiSegmentSelectedStatusText = "Multiple Segments ({0}) selected.";
	const std::string PointSelectedStatusText = "Point {0} selected.";
	const std::string MultiPointSelectedStatusText = "Multiple Points ({0}) selected.";
	const std::string ConstraintSelectedStatusText = "Constraint {0} selected.";
	const std::string MultiConstraintSelectedStatusText = "Multiple Constraints ({0}) selected.";
};

template<typename T>
inline bool SketchEditorTool::StartSegmentCreation(bool continuesCreation)
{
	bool wasInContinuesSegmentCreation = _ContinuesSegmentCreation;
	StopTool();

	auto newCreator = new T();
	if(!newCreator->Start(this))
	{
		return false;
	}

	if(wasInContinuesSegmentCreation && continuesCreation && _LastContinuesSegmentStartPoint >= 0)
	{
		_ContinuesSegmentCreation = newCreator->Continue(_LastContinuesSegmentStartPoint);
	}
	else
	{
		_LastContinuesSegmentStartPoint = -1;
	}

	_ContinuesSegmentCreation = continuesCreation;

	_CurrentTool = newCreator;
	return false;
}

#endif  // _SketchEditorTool_Header_File_
