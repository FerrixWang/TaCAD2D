// Copyright [2025] TaCAD

// Own include
#include "Core/Viewport.hxx"

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
#include <V3d_View.hxx>
#include <IntAna_IntConicQuad.hxx>

#include "Core/Workspace.hxx"

// Constructor
Viewport::Viewport(const Handle(Workspace)& workspace, const TDF_Label& theLabel)
	: m_workspace(workspace)
    , myLabel(theLabel)
{
}

void Viewport::Init(bool useMsaa)
{
    if(m_v3dView || !m_workspace)
    {
        return;
    }

	auto aViewer = m_workspace->V3dViewer();
    m_v3dView = aViewer->CreateView();
    aViewer->HideGridEcho(m_v3dView);

    m_v3dView->SetBgGradientColors(Quantity_Color(0.624, 0.714, 0.804, Quantity_TOC_sRGB),
                                   Quantity_Color(0.424, 0.482, 0.545, Quantity_TOC_sRGB),
                                   Aspect_GFM_VER, false);

    m_v3dView->SetImmediateUpdate(false);
    m_v3dView->SetProj(V3d_Zpos);  // 设置为正投影俯视图

    Graphic3d_RenderingParams& renderParams = m_v3dView->ChangeRenderingParams();
    renderParams.NbMsaaSamples = useMsaa ? 4 : 0;
    renderParams.IsAntialiasingEnabled = useMsaa;
    renderParams.TransparencyMethod = Graphic3d_RTM_BLEND_OIT;
    renderParams.Method = Graphic3d_RM_RASTERIZATION;
    renderParams.RaytracingDepth = 3;
    renderParams.IsShadowEnabled = true;
    renderParams.IsReflectionEnabled = true;
    renderParams.IsTransparentShadowEnabled = true;
}

gp_Ax1 Viewport::GetViewAxis(int screenX, int screenY)
{
    if(m_v3dView.IsNull())
        return gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));

    double px = 0, py = 0, pz = 0;
    m_v3dView->Convert(screenX, screenY, px, py, pz);
    return  gp_Ax1(gp_Pnt(px, py, pz), getViewDirection());
}

Viewport::RenderModes Viewport::GetRenderMode() const
{
    auto aAttr = RenderModeAttribute();
    if(aAttr.IsNull())
    {
        TDataStd_Integer::Set(RenderModeLabel(), 0);
    }
    return static_cast<RenderModes>(RenderModeAttribute()->Get());
}

void Viewport::SetRenderMode(RenderModes value)
{
    RenderModeAttribute()->Set(RenderModeLabel(), value);
    viewportChanged(this);
    updateRenderMode();
}
// Function to update render mode

void Viewport::updateRenderMode()
{
    if(!m_v3dView) return;

    auto m_renderMode = GetRenderMode();
    m_v3dView->SetComputedMode(m_renderMode == HLR);

    auto& renderParams = m_v3dView->ChangeRenderingParams();
    if(m_renderMode == Raytraced)
    {
        renderParams.Method = Graphic3d_RM_RAYTRACING;
    }
    else
    {
        renderParams.Method = Graphic3d_RM_RASTERIZATION;
    }
}

gp_Dir Viewport::getRightDirection()
{
    auto upDir = getUpDirection();
    auto eyeDir = getViewDirection();
    return upDir.Crossed(eyeDir);
}

gp_Dir Viewport::getUpDirection()
{
    if(V3dView().IsNull())
    {
        return gp_Dir(0, 0, 1);
    }

    double xUp = 0, yUp = 0, zUp = 0;
    V3dView()->Up(xUp, yUp, zUp);
    return gp_Dir(xUp, yUp, zUp);
}

gp_Dir Viewport::getViewDirection()
{
    validateViewGeometry();
	auto m_targetPoint = GetTargetPoint();
	auto m_eyePoint = GetEyePoint();
    auto eyeVector = gp_Vec(m_targetPoint, m_eyePoint);
    return gp_Dir(eyeVector);
}


bool Viewport::ScreenToPoint(gp_Pln plane, int screenX, int screenY, gp_Pnt& resultPnt)
{
    if(!V3dView().IsNull())
    {
        try
        {
            if(V3dView()->IfWindow())
            {
                double xv = 0, yv = 0, zv = 0;
                double vx = 0, vy = 0, vz = 0;

                V3dView()->Convert(screenX, screenY, xv, yv, zv);
                V3dView()->Proj(vx, vy, vz);

                gp_Lin line(gp_Pnt(xv, yv, zv), gp_Dir(vx, vy, vz));
                IntAna_IntConicQuad intersection(line, plane, Precision::Angular(), 0, 0);

                if(intersection.IsDone()
                   && !intersection.IsParallel()
                   && intersection.NbPoints() > 0)
                {
                    resultPnt = intersection.Point(1);
                    return true;
                }
            }
        }
        catch(std::exception e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
            assert(false);
        }
    }

    resultPnt = gp_Pnt();
    return false;
}

gp_Pnt Viewport::GetEyePoint() const
{
    auto aAttr = EyePointAttribute();
    if(aAttr.IsNull())
    {
        TDataXtd_Point::Set(EyePointLabel(), gp_Pnt());
    }

    gp_Pnt eyePoint;
    TDataXtd_Geometry::Point(EyePointLabel(), eyePoint);
    return eyePoint;
}

gp_Pnt Viewport::GetTargetPoint() const
{
    auto aAttr = TargetPointAttribute();
    if(aAttr.IsNull())
    {
        TDataXtd_Point::Set(TargetPointLabel(), gp_Pnt());
    }

    gp_Pnt targetPoint;
    TDataXtd_Geometry::Point(TargetPointLabel(), targetPoint);
    return targetPoint;
}

double Viewport::GetTwist() const
{
    auto aAttr = TwistAttribute();
    if(aAttr.IsNull())
    {
        TDataStd_Real::Set(TwistLabel(), 0.0);
    }
    return TwistAttribute()->Get();
}

double Viewport::GetScale() const
{
    auto aAttr = ScaleAttribute();
    if(aAttr.IsNull())
    {
        TDataStd_Real::Set(ScaleLabel(), 1.0);
    }
    return ScaleAttribute()->Get();
}

Signal<void(const Handle(Viewport)&)> Viewport::viewportChanged;
