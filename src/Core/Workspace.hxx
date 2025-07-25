// Copyright [2025] TaCAD

#ifndef CORE_WORKSPACE_H_
#define CORE_WORKSPACE_H_

#include <vector>

#include <AIS_DisplayMode.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_TypeOfPlane.hxx>
#include <gp_Pln.hxx>
#include <gp_Quaternion.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Standard_Transient.hxx>
#include <TDataStd_Integer.hxx>
#include <V3d_Viewer.hxx>

#include "Core/Document.hxx"
#include "Core/GridTypes.hxx"
#include "Core/Project/WorkingContext.hxx"

class Viewport;

class Workspace : public Standard_Transient, public EnablePropertyChanged
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Integer, GridEnabled)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Name, CurrentWorkingContext)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Name, GlobalsWorkingContext)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Name, Viewports)

public:
	Workspace(const Handle<Document>& document, const TDF_Label& theLabel = {});
	~Workspace() {};

	TDF_Label Label() const { return myLabel; }
	void SetLabel(const TDF_Label& label) { myLabel = label; }

	void InitViewports();
	void InitV3dViewer();
	void InitAisContext();

	Handle<V3d_Viewer> V3dViewer() const;

	Handle<AIS_InteractiveContext> AisContext() const;

	std::vector<Handle<Viewport>>& Viewports();
	
	void UpdateCurrentViewer();

	bool IsGridEnabled() const;

	void SetGridEnabled(bool enabled)
	{
		auto aAttr = GridEnabledAttribute();
		if(aAttr.IsNull())
		{
			InitGridEnabled();
		}
		GridEnabledAttribute()->Set(enabled);
	}

	void SetGridType(GridTypes type);
	GridTypes GetGridType() const;
	void SetGridStep(double step);
	double GetGridStep() const;
	void SetGridRotation(double rotation);
	double GetGridRotation() const;
	void SetGridDivisions(int divisions);
	int GetGridDivisionsAttribute() const;
	void SetWorkingPlane(const gp_Pln& plane);
	gp_Pln WorkingPlane() const;

	void SetDefaultWorkingPlane(AIS_TypeOfPlane type);
	bool ProjectToGrid(const Handle<Viewport>& viewport, int screenX, int screenY, gp_Pnt& pnt);

	gp_Pnt2d ComputeGridPoint(const gp_Pnt2d& uv);

	Handle<WorkingContext> GetWorkingContext() const;
	Handle<WorkingContext> GetGlobalWorkingContext() const
	{
		return myGlobalWorkingContext;
	}

	gp_Quaternion GetWorkingPlaneRotation() const;

private:
	void _ApplyWorkingContext();
	void _RaiseGridChanged() { GridChanged(this); }

public:
	Signal<void(Handle<Workspace>)> GridChanged;

private:
	TDF_Label myLabel;

	std::vector<Handle<Viewport>> myViewports;  // List of Viewports in the workspace

	Handle<V3d_Viewer> myV3dViewer;  // 3D viewer handle
	Handle<AIS_InteractiveContext> myAisContext;  // AIS context handle

	Handle<Document> myDocument;

	Handle<WorkingContext> myCurrentWorkingContext;
	Handle<WorkingContext> myGlobalWorkingContext;
};

DEFINE_STANDARD_HANDLE(Workspace, Standard_Transient)

#endif  // CORE_WORKSPACE_H_
