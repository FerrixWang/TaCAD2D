// Copyright [2025] TaCAD

#include "Core/Workspace.hxx"

// Occt includes
#include <AIS_InteractiveContext.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <ElSLib.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfVisualization.hxx>
#include <V3d_Viewer.hxx>
#include <TDF_ChildIterator.hxx>

#include "Core/Viewport.hxx"

Workspace::Workspace(const Handle(Document)& document, const TDF_Label& theLabel)
    : myDocument(document)
	, myLabel(theLabel)
{
    SetGridEnabled(true);

    myCurrentWorkingContext = new WorkingContext(CurrentWorkingContextLabel());
    myGlobalWorkingContext  = new WorkingContext(GlobalsWorkingContextLabel());
    myCurrentWorkingContext->CopyFrom(myGlobalWorkingContext); 

    InitViewports();
}

void Workspace::InitViewports()
{
    TDF_Label aLabel = ViewportsLabel();
    if(!aLabel.HasChild())
    {
        aLabel.NewChild();
    }

    for(TDF_ChildIterator it(aLabel); it.More(); it.Next())
    {
        auto aViewport = new Viewport(this, it.Value());
        myViewports.push_back(aViewport);
    }
}

void Workspace::InitV3dViewer()
{
    if(myV3dViewer.IsNull())
    {
        Handle(Aspect_DisplayConnection) aDisp = new Aspect_DisplayConnection();
        Handle(OpenGl_GraphicDriver) aDriver = new OpenGl_GraphicDriver(aDisp, false);
        //// lets QOpenGLWidget to manage buffer swap
        //aDriver->ChangeOptions().buffersNoSwap = true;
        //// don't write into alpha channel
        //aDriver->ChangeOptions().buffersOpaqueAlpha = true;
        //// offscreen FBOs should be always used
        //aDriver->ChangeOptions().useSystemBuffer = false;
        aDriver->ChangeOptions().ffpEnable = false;
        aDriver->ChangeOptions().contextDebug = false;

        // create viewer
        myV3dViewer = new V3d_Viewer(aDriver);
    }

    // Initialize 3D viewer with graphic driver

    myV3dViewer->SetDefaultViewSize(1000.0);
    myV3dViewer->SetDefaultViewProj(V3d_TypeOfOrientation::V3d_XposYposZpos);
    myV3dViewer->SetDefaultBackgroundColor(Quantity_Color(NCollection_Vec3{0.3f, 0.3f, 0.3f}));
    myV3dViewer->SetDefaultVisualization(V3d_TypeOfVisualization::V3d_ZBUFFER);
    myV3dViewer->SetLightOn(new V3d_DirectionalLight(V3d_TypeOfOrientation::V3d_Zneg, Quantity_Color(Quantity_NOC_WHITE), true));
    myV3dViewer->SetLightOn(new V3d_AmbientLight(Quantity_Color(Quantity_NOC_WHITE)));
    myV3dViewer->SetGridEcho(false);
}

void Workspace::InitAisContext()
{
    if(myV3dViewer.IsNull())
    {
        InitV3dViewer();
    }

    if(myAisContext.IsNull())
    {
        myAisContext = new AIS_InteractiveContext(myV3dViewer);
        myAisContext->UpdateCurrentViewer();

        TPrsStd_AISViewer::New(myDocument->Main(), myAisContext);
    }

    myAisContext->SetDisplayMode(AIS_Shaded, true);

    myAisContext->SetAutoActivateSelection(true);
    myAisContext->SetToHilightSelected(false);
    myAisContext->SetPickingStrategy(SelectMgr_PickingStrategy::SelectMgr_PickingStrategy_OnlyTopmost);
    myAisContext->SetDisplayMode(AIS_DisplayMode::AIS_Shaded, false);
    myV3dViewer->DisplayPrivilegedPlane(false, 500.0);
    myAisContext->EnableDrawHiddenLine();

    // Reinit ais parameters
    _ApplyWorkingContext();
    myAisContext->SetPixelTolerance(2);

    auto drawer = myAisContext->DefaultDrawer();
    drawer->SetTypeOfHLR(Prs3d_TypeOfHLR::Prs3d_TOH_PolyAlgo);

    auto style = new Prs3d_Drawer();
    style->SetFaceBoundaryDraw(true);
    style->SetArrowAspect(new Prs3d_ArrowAspect(1.0, 35.0));
    style->SetFaceBoundaryAspect(new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.0));
	style->SetColor(Quantity_NOC_YELLOW1);
    myAisContext->SetHighlightStyle(style);
}

Handle(V3d_Viewer) Workspace::V3dViewer() const
{
    return myV3dViewer;
}

Handle(AIS_InteractiveContext) Workspace::AisContext() const
{
    return myAisContext;
}

std::vector<Handle(Viewport)>& Workspace::Viewports()
{
    return myViewports;
}

void Workspace::UpdateCurrentViewer()
{
    myAisContext->UpdateCurrentViewer();
}

bool Workspace::IsGridEnabled() const
{
    return GridEnabledAttribute()->Get();
}

gp_Pnt2d Workspace::ComputeGridPoint(const gp_Pnt2d& coord)
{
    // 获取网格参数
    double rotation = GetWorkingContext()->GridRotation() * M_PI / 180.0; // 转换为弧度
    gp_Pnt2d gridPoint = coord;

    if(std::abs(rotation) > Precision::Confusion())
    {
        gridPoint.Rotate(gp_Pnt2d(0, 0), -rotation);
    }

    if(GetWorkingContext()->GridType() == GridTypes::Circular)
    {
        // 圆形网格
        double angle = gp_Dir2d(1, 0).Angle(gp_Dir2d(gridPoint.X(), gridPoint.Y()));
        double circStep = M_PI / GetWorkingContext()->GridDivisions();
        int iSeg = static_cast<int>(angle / circStep + 0.5); // 四舍五入
        double radius = gridPoint.Distance(gp_Pnt2d(0, 0));
        int iCirc = static_cast<int>(radius / GetWorkingContext()->GridStep() + 0.5);
        gridPoint = gp_Pnt2d(GetWorkingContext()->GridStep() * iCirc, 0);
        gridPoint.Rotate(gp_Pnt2d(0, 0), circStep * iSeg);
    }
    else // GridTypes::Rectangular
    {
        // 矩形网格
        int ix = static_cast<int>(gridPoint.X() / GetWorkingContext()->GridStep() + 0.5);
        int iy = static_cast<int>(gridPoint.Y() / GetWorkingContext()->GridStep() + 0.5);
        gridPoint = gp_Pnt2d(GetWorkingContext()->GridStep() * ix, GetWorkingContext()->GridStep() * iy);
    }

    // 恢复旋转
    if(rotation != 0.0)
    {
        gridPoint.Rotate(gp_Pnt2d(0, 0), rotation);
    }

    return gridPoint;
}

void Workspace::SetGridType(GridTypes type)
{
    if(GetWorkingContext()->GridType() != type)
    {
        GetWorkingContext()->SetGridType(type);
        _RaiseGridChanged();
    }
}

GridTypes Workspace::GetGridType() const
{
    return GetWorkingContext()->GridType();
}

void Workspace::SetGridStep(double step)
{
    if(GetWorkingContext()->GridStep() != step)
    {
        GetWorkingContext()->SetGridStep(step);
        _RaiseGridChanged();
    }
}

double Workspace::GetGridStep() const
{
    return GetWorkingContext()->GridStep();
}

void Workspace::SetGridRotation(double rotation)
{
    if(GetWorkingContext()->GridRotation() != rotation)
    {
        GetWorkingContext()->SetGridRotation(rotation);
        _RaiseGridChanged();
    }
}

double Workspace::GetGridRotation() const
{
    return GetWorkingContext()->GridRotation();
}

void Workspace::SetGridDivisions(int divisions)
{
    if(GetWorkingContext()->GridDivisions() != divisions)
    {
        GetWorkingContext()->SetGridDivisions(divisions);
        _RaiseGridChanged();
    }
}

int Workspace::GetGridDivisionsAttribute() const
{
    return GetWorkingContext()->GridDivisions();
}

void Workspace::SetWorkingPlane(const gp_Pln& plane)
{
    if(GetWorkingContext()->WorkingPlane().Distance(plane) > Precision::Confusion())
    {
        GetWorkingContext()->SetWorkingPlane(plane);
        _RaiseGridChanged();
    }
    _ApplyWorkingContext();
}

gp_Pln Workspace::WorkingPlane() const
{
    return GetWorkingContext()->WorkingPlane();
}

void Workspace::SetDefaultWorkingPlane(AIS_TypeOfPlane type)
{
    gp_Pln plane;
    switch(type)
    {
    case AIS_TypeOfPlane::AIS_TOPL_XYPlane:
        plane = gp_Pln(gp::XOY()); // XOY 平面
        break;
    case AIS_TypeOfPlane::AIS_TOPL_XZPlane:
        plane = gp_Pln(gp::ZOX()); // YOZ 平面
        break;
    case AIS_TypeOfPlane::AIS_TOPL_YZPlane:
        plane = gp_Pln(gp::YOZ()); // ZOX 平面
        break;
    default:
        return; // 无效类型，保持不变
    }
    SetWorkingPlane(plane);
}

Handle(WorkingContext) Workspace::GetWorkingContext() const
{
    return myCurrentWorkingContext;
}

gp_Quaternion Workspace::GetWorkingPlaneRotation() const
{
    auto wp = WorkingPlane();
    gp_Mat mat(
        wp.XAxis().Direction().XYZ(),
        wp.YAxis().Direction().XYZ(),
        wp.Position().Direction().XYZ()
    );
    return gp_Quaternion(mat);
}

void Workspace::_ApplyWorkingContext()
{
    if(!myAisContext.IsNull())
    {
        myV3dViewer->SetPrivilegedPlane(GetWorkingContext()->WorkingPlane().Position());
    }
    _RaiseGridChanged();
}

bool Workspace::ProjectToGrid(const Handle(Viewport)& viewport, int screenX, int screenY, gp_Pnt& pnt)
{
    gp_Pln plane = WorkingPlane();
    if(!viewport->ScreenToPoint(plane, screenX, screenY, pnt))
        return false;

    double u = 0, v = 0;
    ElSLib::Parameters(plane, pnt, u, v);

    gp_Pnt2d uv(u, v);

    gp_Pnt2d gridUv = ComputeGridPoint(uv);

    ElSLib::Value(uv.X(), uv.Y(), plane);
    return true;
}
