// Copyright [2025] TaCAD

// Own include
#include "Iact/WorkspaceController.hxx"

#include <Message.hxx>
#include <TDataXtd_Geometry.hxx>

#include "Core/Viewport.hxx"
#include "Core/Workspace.hxx"
#include "Iact/Framework/Editor.hxx"
#include "Iact/Framework/Tool.hxx"
#include "Iact/HudElements/HudManager.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/MouseEventData.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"
#include "Iact/ViewportController.hxx"
#include "Occt/AisExtensions/AISX_Grid.hxx"
#include "Occt/OcctHelper/AisHelper.hxx"
#include "Xcaf/Attributes/WorkingContextAttribute.hxx"

WorkspaceController::WorkspaceController(const Handle(Workspace)& workspace) 
	: m_workspace(workspace)
	, m_activeViewport(nullptr)
	, m_currentTool(nullptr)
	, m_currentEditor(nullptr)
	, m_mouseEventData(new MouseEventData)
	, m_hudManager(new HudManager)
{
	// Initialize the workspace controller
	InitWorkspace();
}

void WorkspaceController::InitWorkspace()
{
	m_workspace->InitAisContext();

	for(const auto& viewport : m_workspace->Viewports())
	{
		// Create a new ViewportController for each viewport
		auto viewportController = new ViewportController(viewport, this);
		m_viewportControllers.push_back(viewportController);
	}

	m_workspace->GridChanged.connect([this](const auto&) {
		_GridNeedsUpdate = true;
		_UpdateGrid();
		InteractiveContext::Current()->GetViewportController()->GetViewport()->OnViewMoved();
	});

	myGrid = new AISX_Grid();
	AisHelper::DisableGlobalClipPlanes(myGrid);
	m_workspace->AisContext()->Display(myGrid, 0, -1, true);
	_UpdateGrid();
}                                                                                                                        

void WorkspaceController::_UpdateGrid()
{
	if(!_GridNeedsUpdate)
	{
		return;
	}

	if(myGrid.IsNull())
	{
		return;
	}

	auto wc = m_workspace->GetWorkingContext();
	if(m_workspace->IsGridEnabled()) 
	{
		gp_Pln aPlanePoint = wc->WorkingPlane();
		gp_Ax3 position = aPlanePoint.Position();

		if(wc->GridRotation() != 0.0)
		{
			position.Rotate(aPlanePoint.Axis(), wc->GridRotation());
		}

		myGrid->SetPosition(position);
		myGrid->SetExtents(_LastGridSize.X(), _LastGridSize.Y());
		myGrid->SetDivisions(wc->GridStep(), wc->GridDivisions());
		if(wc->GridType() == GridTypes::Rectangular)
		{
			m_workspace->AisContext()->SetDisplayMode(myGrid, 1, false);
		}
		else
		{
			m_workspace->AisContext()->SetDisplayMode(myGrid, 2, false);
		}
	}
	else
	{
		m_workspace->AisContext()->SetDisplayMode(myGrid, 0, false);
	}
	_GridNeedsUpdate = false;
}

void WorkspaceController::SetActiveViewport(const Handle(Viewport)& viewport)
{
	if(!viewport.IsNull() && m_activeViewport != viewport)
	{
		m_activeViewport = viewport;
	}
}

bool WorkspaceController::StartTool(const Handle(Tool)& tool)
{
	try
	{
		if(!m_currentTool.IsNull() && !CancelTool(m_currentTool, true))
		{
			return false;
		}

		if(!tool.IsNull())
		{
			tool->setWorkspaceController(this);
			m_currentTool = tool;
			if(!tool->start())
			{
				return false;
			}

			currentToolChanged(tool);
			return true;
		}
		return false;
	}
	catch(std::exception& e)
	{
		std::cerr << "Error starting tool: " << e.what() << std::endl;
	}

	return false;
}

bool WorkspaceController::CancelTool(const Handle(Tool)& tool, bool force)
{
	bool isCanceled = false;

	if(m_currentTool != tool)
	{
		return false;
	}

	if(!m_currentTool.IsNull())
	{
		isCanceled = m_currentTool->cancel(force);
	}

	if(isCanceled)
	{
		m_currentTool = nullptr;
		currentToolChanged(nullptr);
	}

	return isCanceled;
}

void WorkspaceController::RemoveTool(const Handle(Tool)& tool)
{
	if(!m_currentTool.IsNull())
		return;
	m_currentTool.Nullify();

	currentToolChanged(nullptr);
}

bool WorkspaceController::CancelCurrentTool() {
	
	auto sketchTool = Handle(SketchEditorTool)::DownCast(m_currentTool);
	if(sketchTool.IsNull())
	{
		return true;
	}
	sketchTool->StopTool();
	return true;
}

Handle(ViewportController) WorkspaceController::GetViewportController(const Handle(Viewport)& viewport)
{
	for(const auto& controller : m_viewportControllers)
	{
		if(controller->GetViewport() == viewport)
		{
			return controller;
		}
	}
	return nullptr;
}

bool WorkspaceController::onMouseMove(const Handle(ViewportController)& theViewportController)
{
	m_mouseEventData->Clear();

	auto aMousePos = theViewportController->LastMousePosition();
	auto viewport = theViewportController->GetViewport();

	if(!viewport->ScreenToPoint(m_workspace->WorkingPlane(), aMousePos.x(), aMousePos.y(), aPlanePoint));

	m_mouseEventData->Set(viewport, aMousePos, aPlanePoint, theViewportController->LastMouseFlags());

	if(!m_currentTool.IsNull())
	{
		if(m_currentTool->onMouseMove(m_mouseEventData) && !m_currentEditor.IsNull())
		{
			m_currentEditor->onMouseMove(m_mouseEventData);
		}
	}

	return false;
}

bool WorkspaceController::onMousePress(const Handle(ViewportController)& theViewportController)
{
	if(!m_currentTool.IsNull())
	{
		if(m_currentTool->onMousePress(m_mouseEventData) && !m_currentEditor.IsNull())
		{
			m_currentEditor->onMousePress(m_mouseEventData);
		}
	}
	return false;
}

bool WorkspaceController::onMouseRelease(const Handle(ViewportController)& theViewportController)
{
	if(!m_currentTool.IsNull())
	{
		if(m_currentTool->onMouseRelease(m_mouseEventData) && !m_currentEditor.IsNull())
		{
			m_currentEditor->onMouseRelease(m_mouseEventData);
		}
	}
	return false;
}
