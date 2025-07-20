//! class PointAction

#ifndef IACT_TOOLACTIONS_POINTACTION_HXX
#define IACT_TOOLACTIONS_POINTACTION_HXX

#include <AIS_Point.hxx>
#include <gp_Pnt.hxx>

#include "Iact/Framework/ToolAction.hxx"
#include "Iact/Visual/Marker.hxx"
#include "Iact/Workspace/Snap3D.hxx"

class PointAction : public ToolAction
{
public:
	// Constructor
	PointAction();

	// Destructor
	~PointAction() override = default;

	gp_Pnt PointOnPlane() const
	{
		return myPoint;
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

private:
	virtual bool onStart() override { 

		return true; 
	}

	void ProcessMouseInput(const Handle(MouseEventData)& theWorkspaceController);

	void _EnsureMarker();

public:
	Signal<void(Handle(PointAction))> Preview;
	Signal<void(Handle(PointAction))> Finished;

private:
	gp_Pnt myPoint;
	Handle(AIS_Point) myMarker;
	Handle(Marker) _Marker;
	Handle(Snap3D) _SnapHandler;
	Aspect_VKeyFlags myKeyFlags;
};

DEFINE_STANDARD_HANDLE(PointAction, ToolAction)

#endif  // IACT_TOOLACTIONS_POINTACTION_HXX

