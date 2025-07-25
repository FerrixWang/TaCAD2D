// Copyright [2025] TaCAD

#ifndef IACT_WORKSPACE_INTERACTIVECONTEXT_H_
#define IACT_WORKSPACE_INTERACTIVECONTEXT_H_

#include "Core/CoreContext.hxx"
#include "Core/Application.hxx"
#include "Iact/ViewportController.hxx"
#include "Iact/WorkspaceController.hxx"
#include "Iact/Workspace/EditorState.hxx"

class InteractiveContext : public CoreContext
{
public:
	static InteractiveContext* Current()
	{
		return static_cast<InteractiveContext*>(CoreContext::Current());
	}

	Handle<Application> GetApplication() const
	{
		return m_application;
	}

	Handle<WorkspaceController> GetWorkspaceController() const
	{
		return m_workspaceController;
	}

	Handle<ViewportController> GetViewportController() const
	{
		return m_viewportController;
	}

	Handle<EditorState> GetEditorState() const
	{
		return m_editorState;
	}

protected:
	virtual void SetWorkspace(const Handle<Workspace>& workspace) override
	{
		if(!workspace.IsNull() && m_workspace != workspace)
		{
			SetWorkspaceController(new WorkspaceController(workspace));
			CoreContext::SetWorkspace(workspace);
			workspaceControllerChanged(m_workspaceController);
		}
	}

	virtual void SetViewport(const Handle<Viewport>& viewport) override
	{
		if(!viewport.IsNull() && m_viewport != viewport)
		{
			CoreContext::SetViewport(viewport);
			m_workspaceController->SetActiveViewport(viewport);
			SetViewportController(m_workspaceController->GetViewportController(viewport));
		}
	}

private:
	void SetWorkspaceController(const Handle<WorkspaceController>& controller)
	{
		if(!controller.IsNull() && m_workspaceController != controller)
		{
			m_workspaceController.Nullify();
			m_workspaceController = controller;
			workspaceControllerChanged(controller);
		}
	}

	void SetViewportController(const Handle<ViewportController>& controller)
	{
		if(!controller.IsNull() && m_viewportController != controller)
		{
			m_viewportController = controller;
			viewportControllerChanged(controller);
		}
	}

protected:
	InteractiveContext();

private:
	void LoadEditorState();
	void SaveEditorState();

public:
	Signal<void(const Handle<WorkspaceController>&)> workspaceControllerChanged;
	Signal<void(const Handle<ViewportController>&)> viewportControllerChanged;

private:
	Handle<Application>         m_application;
	Handle<WorkspaceController> m_workspaceController;
	Handle<ViewportController>  m_viewportController;

	Handle<EditorState> m_editorState;
};

#endif  // IACT_WORKSPACE_INTERACTIVECONTEXT_H_
