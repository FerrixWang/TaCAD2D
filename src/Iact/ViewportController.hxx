// Copyright [2025] TaCAD

#ifndef IACT_WORKSPACE_VIEWPORTCONTROLLER_H_
#define IACT_WORKSPACE_VIEWPORTCONTROLLER_H_

// Occt includes
#include <AIS_RubberBand.hxx>
#include <AIS_ViewController.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Graphic3d_TransformPers.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Standard_Integer.hxx>
#include <V3d_View.hxx>

// Project includes
#include "Core/Viewport.hxx"
#include "Iact/WorkspaceController.hxx"

class ViewportWidget;

class ViewportController : public AIS_ViewController, public Standard_Transient
{
public:
	enum PredefinedViews
	{
		Top,
		Bottom,
		Left,
		Right,
		Front,
		Back,
		WorkingPlane,
		
		Count
	};

public:
	explicit ViewportController(const Handle(Viewport)& viewport,
								const Handle(WorkspaceController)& workspacecontroller);

	void SetPredefinedView(PredefinedViews predefinedView);

	void ZoomFitAll() {
		FitAllAuto(m_workspaceController->workspace()->AisContext(), m_viewport->V3dView());
		m_viewport->OnViewMoved();
	}

	void Zoom(double value)
	{
		if(value > 0)
		{
			m_viewport->V3dView()->SetZoom(1.0 + value, true);
		}
		else if(value < 0)
		{
			m_viewport->V3dView()->SetZoom(1.0 / (1.0 - value), true);
		}
		m_workspaceController->Invalidate();
		m_viewport->OnViewMoved();
	}

	Handle(Viewport) GetViewport() const
	{
		return m_viewport;
	}

	Handle(WorkspaceController) GetWorkspaceController() const
	{
		return m_workspaceController;
	}

	void SetOpenglWidget(ViewportWidget* widget);

	Handle(AIS_ViewCube) ViewCube() const
	{
		return _AisViewCube;
	}

	Handle(V3d_View) FocusView() const
	{
		return myFocusView;
	}

	// ================================================================
	// Function : OnSubviewChanged
	// Purpose  :
	// ================================================================
	void OnSubviewChanged(const Handle(AIS_InteractiveContext)&,
										  const Handle(V3d_View)&,
										  const Handle(V3d_View)& theNewView) override
	{
		myFocusView = theNewView;
	}

	bool onMouseMove(const Graphic3d_Vec2i& thePoint,
					 Aspect_VKeyMouse       theButtons,
					 Aspect_VKeyFlags       theModifiers)
	{
		AIS_ViewController::UpdateMousePosition(thePoint, theButtons, theModifiers, false);
		m_workspaceController->onMouseMove(this);
		return true;
	}

	bool onMousePress(const Graphic3d_Vec2i& thePoint,
													 Aspect_VKeyMouse       theButtons,
													 Aspect_VKeyFlags       theModifiers)
	{
		AIS_ViewController::UpdateMouseButtons(thePoint, theButtons, theModifiers, false);
		m_workspaceController->onMousePress(this);
		return true;
	}

	bool onMouseRelease(const Graphic3d_Vec2i& thePoint,
					  Aspect_VKeyMouse       theButtons,
					  Aspect_VKeyFlags       theModifiers)
	{
		AIS_ViewController::UpdateMouseButtons(thePoint, theButtons, theModifiers, false);
		m_workspaceController->onMouseRelease(this);
		return true;
	}

	static std::string convertToString(PredefinedViews predefinedView)
	{
		switch(predefinedView)
		{
		case Top:
			return "Top";
		case Bottom:
			return "Bottom";
		case Left:
			return "Left";
		case Right:
			return "Right";
		case Front:
			return "Front";
		case Back:
			return "Back";
		default:
			return "Unknown";
		}
	}
	void UpdateView();

private:
	//! Handle view redraw.
	virtual void handleViewRedraw(const Handle(AIS_InteractiveContext)& theCtx,
								  const Handle(V3d_View)& theView) override;

	void setViewCube(bool isVisible, uint size, double duration)
	{
		_AisViewCube = new AIS_ViewCube();
		_AisViewCube->SetSize(size);
		_AisViewCube->SetBoxFacetExtension(size * 0.15);
		_AisViewCube->SetViewAnimation(myViewAnimation);
		_AisViewCube->SetFixedAnimationLoop(false);
		_AisViewCube->SetDrawAxes(false);
		_AisViewCube->SetDuration(duration);
		_AisViewCube->SetResetCamera(true);
		_AisViewCube->SetFitSelected(true);
		_AisViewCube->SetTransformPersistence(new Graphic3d_TransformPers(Graphic3d_TMF_TriedronPers,
											  Aspect_TOTP_RIGHT_UPPER,
											  Graphic3d_Vec2i(100, 100))
		);

		if(isVisible)
		{
			auto aisContext = m_workspaceController->workspace()->AisContext();
			aisContext->Display(_AisViewCube, false);
		}
	}

private:
	Handle(Viewport) m_viewport;
	Handle(WorkspaceController) m_workspaceController;
	Handle(AIS_ViewCube)           _AisViewCube;

	Handle(V3d_View) myFocusView;

	ViewportWidget* m_viewportWidget;

	bool myLockedToPlane = false;
};

DEFINE_STANDARD_HANDLE(ViewportController, Standard_Transient)

#endif  // IACT_WORKSPACE_VIEWPORTCONTROLLER_H_
