// Copyright [2025] TaCAD

#ifndef CORE_VIEWPORT_H_
#define CORE_VIEWPORT_H_

// stl include
#include <cmath>

// boost includes
#include <boost/signals2.hpp>

// Occt includes
#include <AIS_AnimationCamera.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_RenderingMode.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_RenderTransparentMethod.hxx>
#include <TDataXtd_Point.hxx>
#include <V3d_View.hxx>
#include <TDataXtd_Geometry.hxx>

// Project includes
#include "Core/Workspace.hxx"

class Viewport : public Standard_Transient
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataXtd_Point, EyePoint)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataXtd_Point, TargetPoint)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Real, Twist)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real, Scale)
	DEFINE_ATTRIBUTE_ON_CHILD(5, TDataStd_Integer, RenderMode)

public:
    // Enum for RenderModes
    enum RenderModes
    {
        SolidShaded,
        HLR,
        Raytraced
    };
    // Constructor
    explicit Viewport(const Handle(Workspace)& workspace, const TDF_Label& theLabel);

    void Init(bool useMsaa);

    TDF_Label Label() const { return myLabel; }
	void SetLabel(const TDF_Label& label) { myLabel = label; }

    gp_Ax1 GetViewAxis(int screenX, int screenY);

	RenderModes GetRenderMode() const;

	void SetRenderMode(RenderModes value);

	Handle(V3d_View) V3dView() const
	{
		return m_v3dView;
	}

	Handle(Workspace) GetWorkspace() const
	{
		return m_workspace;
	}

    void OnViewMoved()
    {
        viewportChanged(this);
	}

    void updateRenderMode();

    gp_Dir getRightDirection();

    gp_Dir getUpDirection();

    gp_Dir getViewDirection();

    bool ScreenToPoint(gp_Pln plane, int screenX, int screenY, gp_Pnt& resultPnt);

	void SetEyePoint(const gp_Pnt& eyePoint)
	{
		EyePointAttribute()->Set(EyePointLabel(), eyePoint);
		viewportChanged(this);
	}

	gp_Pnt GetEyePoint() const;

	void SetTargetPoint(const gp_Pnt& targetPoint)
	{
		TargetPointAttribute()->Set(TargetPointLabel(), targetPoint);
		viewportChanged(this);
	}

	gp_Pnt GetTargetPoint() const;

	void SetTwist(double twist)
	{
		TwistAttribute()->Set(TwistLabel(), twist);
		viewportChanged(this);
	}

	double GetTwist() const;

	void SetScale(double scale)
	{
		ScaleAttribute()->Set(ScaleLabel(), scale);
		viewportChanged(this);
	}

	double GetScale() const;

	void SetDpiScale(double dpiScale)
	{
		m_dpiScale = dpiScale;
	}

	double GetDpiScale() const
	{
		return m_dpiScale;
	}

	Handle(AIS_AnimationCamera) GetAisAnimationCamera() const
	{
		return m_aisAnimationCamera;
	}

public:
    static Signal<void(const Handle(Viewport)&)> viewportChanged;

private:
    void validateViewGeometry() {}

private:
	TDF_Label myLabel;
    Handle(Workspace) m_workspace;

	double m_dpiScale = 1.0;
    Handle(V3d_View) m_v3dView;
    Handle(AIS_AnimationCamera) m_aisAnimationCamera;
};

DEFINE_STANDARD_HANDLE(Viewport, Standard_Transient)

#endif  // CORE_VIEWPORT_H_
