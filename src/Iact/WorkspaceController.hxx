// Copyright [2025] TaCAD

#ifndef IACT_WORKSPACE_WORKSPACECONTROLLER_H_
#define IACT_WORKSPACE_WORKSPACECONTROLLER_H_

// stl includes
#include <vector>

// Occt includes
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>

// Project includes
#include "Core/Viewport.hxx"
#include "Core/Workspace.hxx"
#include "Iact/Framework/Editor.hxx"
#include "Iact/Framework/Tool.hxx"
#include "Occt/AisExtensions/AISX_Grid.hxx"

class ViewportController;
class HudManager;

class WorkspaceController : public Standard_Transient, public EnablePropertyChanged
{
public:
    explicit WorkspaceController(const Handle(Workspace)& workspace);

	void InitWorkspace();

	void _UpdateGrid();

	void SetActiveViewport(const Handle(Viewport)& viewport);

	bool StartTool(const Handle(Tool)& tool);
	bool CancelTool(const Handle(Tool)& tool, bool force = false);
	void RemoveTool(const Handle(Tool)& tool);

	bool CancelCurrentTool();
	// current Tool
	Handle(Tool) CurrentTool() const { return m_currentTool; }

	Handle(ViewportController) GetViewportController(const Handle(Viewport)& viewport);
	Handle(Workspace) workspace() const { 
		return m_workspace; 
	}

	bool onMouseMove(const Handle(ViewportController)& theViewportController);

	bool onMousePress(const Handle(ViewportController)& theViewportController);

	bool onMouseRelease(const Handle(ViewportController)& theViewportController);

	void Invalidate() { m_workspace->UpdateCurrentViewer(); }

	const Handle(SelectMgr_SelectionManager)& Selection() const { return m_workspace->AisContext()->SelectionManager(); }
	// selector
	const Handle(SelectMgr_ViewerSelector)& Selector() const { return Selection()->Selector(); }

	gp_Pnt PlaneOnPoint() const
	{
		return aPlanePoint;
	}

	void InvalidateSelection(){}

	Handle(HudManager) GetHudManager() const
	{
		return m_hudManager;
	}

protected:

public:
	Signal<void(const Handle(Tool)&)> currentToolChanged;

private:

private:
	std::vector<Handle(ViewportController)> m_viewportControllers;

	Handle(Workspace) m_workspace;
	Handle(Viewport) m_activeViewport;

	Handle(Tool) m_currentTool;
	Handle(Editor) m_currentEditor; 

	Handle(MouseEventData) m_mouseEventData;
	gp_Pnt aPlanePoint;

	Handle(HudManager) m_hudManager;
	Handle(AISX_Grid) myGrid;

	bool _GridNeedsUpdate = true;
	gp_XY _LastGridSize = gp_XY(20000.0, 20000.0);
};

DEFINE_STANDARD_HANDLE(WorkspaceController, Standard_Transient)

#endif // IACT_WORKSPACE_WORKSPACECONTROLLER_H_
