//! class SketchPointAction

#ifndef IACT_TOOLACTIONS_SketchPointAction_HXX
#define IACT_TOOLACTIONS_SketchPointAction_HXX

#include <AIS_Point.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include "Iact/Framework/ToolAction.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"
#include "Iact/Visual/Marker.hxx"
#include "Iact/Workspace/Snap3D.hxx"

class SketchPointAction : public ToolAction
{
public:
	// Constructor
	SketchPointAction(const Handle(SketchEditorTool)& sketchEditorTool);

	// Destructor
	~SketchPointAction() override = default;

	// Point
	gp_Pnt2d Point() const
	{
		return _Point;
	}

protected:
	virtual void cleanup() override
	{
		Preview.disconnect_all_slots();
		Finished.disconnect_all_slots();
		ToolAction::cleanup();
	}

public:
	virtual bool onMouseMove(const Handle(MouseEventData)& theWorkspaceController) override;

	virtual bool onMousePress(const Handle(MouseEventData)& theWorkspaceController) override;

	virtual bool onMouseRelease(const Handle(MouseEventData)& theWorkspaceController)override;

	//Get key 
	Aspect_VKeyFlags GetKeyFlags() const
	{
		return myKeyFlags;
	}

	int MergeCandidateIndex() const
	{
		return _MergeCandidateIndex;
	}

private:
	virtual bool onStart() override
	{

		return true;
	}

	void ProcessMouseInput(const Handle(MouseEventData)& theWorkspaceController);

	void _EnsureMarker();

public:
	Signal<void(Handle(SketchPointAction))> Preview;
	Signal<void(Handle(SketchPointAction))> Finished;

private:
	gp_Pnt myPoint;
	Handle(AIS_Point) myMarker;
	Handle(Marker) _Marker;
	Handle(Snap3D) _SnapHandler;
	Aspect_VKeyFlags myKeyFlags;

	bool _EnablePointMerge;
	Handle(SketchEditorTool) _SketchEditorTool;
	gp_Pnt2d _Point;
	gp_Pnt2d _PointOnWorkingPlane;
	int _MergeCandidateIndex = -1;
};

DEFINE_STANDARD_HANDLE(SketchPointAction, ToolAction)

#endif  // IACT_TOOLACTIONS_SketchPointAction_HXX

