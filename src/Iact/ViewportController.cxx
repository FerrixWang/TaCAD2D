// Copyright [2025] TaCAD

// Own include
#include "Iact/ViewportController.hxx"

// occt includes
#include <AIS_ViewController.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <V3d_TypeOfOrientation.hxx>

// Project includes
#include "Core/Viewport.hxx"
#include "Iact/Viewport/ViewportWidget.hxx"
#include "Iact/WorkspaceController.hxx"

ViewportController::ViewportController(const Handle(Viewport)& viewport, 
									   const Handle(WorkspaceController)& workspacecontroller)
	: m_viewport(viewport)
	, m_workspaceController(workspacecontroller)
	, m_viewportWidget(nullptr)
{
	m_viewport->Init(true);

	// setViewCube(true, 50, 0.3);

	auto& mouseGestureMap = this->ChangeMouseGestureMap();

    myMouseGestureMap.Bind((Standard_UInteger)Aspect_VKeyMouse_RightButton,
                           AIS_MouseGesture_Pan);
    myMouseGestureMap.Bind((Standard_UInteger)Aspect_VKeyMouse_LeftButton,
                           AIS_MouseGesture_SelectRectangle);
	myMouseGestureMap.Bind((Standard_UInteger)Aspect_VKeyMouse_MiddleButton,
						   AIS_MouseGesture_Pan);
	myMouseGestureMap.Bind((Standard_UInteger)Aspect_VKeyMouse_RightButton
						   | (Standard_UInteger)Aspect_VKeyFlags_CTRL,
						   AIS_MouseGesture_Zoom);
}

void ViewportController::SetPredefinedView(PredefinedViews predefinedView)
{
	if(predefinedView == PredefinedViews::WorkingPlane)
	{
		return;
	}

	if(myLockedToPlane || _AisViewCube.IsNull())
		return;

	V3d_TypeOfOrientation orientation;
	switch(predefinedView)
	{
	case PredefinedViews::Top:
		orientation = V3d_TypeOfOrientation_Zup_Top;
		break;
	case PredefinedViews::Bottom:
		orientation = V3d_TypeOfOrientation_Zup_Bottom;
		break;
	case PredefinedViews::Left:
		orientation = V3d_TypeOfOrientation_Zup_Left;
		break;
	case PredefinedViews::Right:
		orientation = V3d_TypeOfOrientation_Zup_Right;
		break;
	case PredefinedViews::Front:
		orientation = V3d_TypeOfOrientation_Zup_Front;
		break;
	case PredefinedViews::Back:
		orientation = V3d_TypeOfOrientation_Zup_Back;
		break;
	default:
		return;
	}
	Handle(AIS_ViewCubeOwner) viewCubeOwner = new AIS_ViewCubeOwner(_AisViewCube, orientation);

	_AisViewCube->HandleClick(viewCubeOwner);
	m_workspaceController->Invalidate();
	m_viewport->OnViewMoved();
}

void ViewportController::SetOpenglWidget(ViewportWidget* widget)
{
	m_viewportWidget = widget;

	m_viewport->viewportChanged.connect(
		[this](const Handle(Viewport)& viewport) {
		if(m_viewportWidget)
		{
			m_viewportWidget->update();
		}
	});
}

//! Handle view redraw.

void ViewportController::handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx, const Handle(V3d_View)& theView)
{
	AIS_ViewController::handleViewRedraw(theCtx, theView);
	if(AIS_ViewController::toAskNextFrame())
	{
		// ask more frames for animation
		m_viewportWidget->update();
	}
}

void ViewportController::UpdateView()
{
	if(m_viewportWidget)
	{
		m_viewportWidget->update();
	}
}
