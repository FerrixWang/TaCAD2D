//! class SnapBase
//! 

#ifndef IACT_WORKSPACE_SNAPBASE_HXX
#define IACT_WORKSPACE_SNAPBASE_HXX

#include <bitset>
#include <string>

#include <AIS_InteractiveObject.hxx>
#include <AIS_Point.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Point.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include "Iact/Workspace/SnapMode.hxx"

class WorkspaceController;


// 捕捉信息结构体
struct SnapInfo
{
    SnapModes mode;
    gp_Pnt point;
    std::string targetName;

    SnapInfo(SnapModes m = SnapMode::None, const gp_Pnt& p = gp_Pnt())
        : mode(m), point(p), targetName("")
    {}

    static const SnapInfo Empty;
};

// 捕捉处理器接口
class ISnapHandler
{
public:
    virtual ~ISnapHandler() = default;
    virtual SnapModes GetSupportedModes() const = 0;
    virtual void SetSupportedModes(SnapModes modes) = 0;
    virtual SnapInfo GetCurrentInfo() const = 0;
    virtual void SetCurrentInfo(const SnapInfo& info) = 0;
};

// 基础捕捉类
class SnapBase : public Standard_Transient, public ISnapHandler
{
public:
    SnapBase() : supportedModes_(SnapMode::None), currentInfo_(SnapInfo::Empty) {}
    virtual ~SnapBase() = default;

    DEFINE_STANDARD_RTTIEXT(SnapBase, Standard_Transient)

    SnapModes GetSupportedModes() const override { return supportedModes_; }
    void SetSupportedModes(SnapModes modes) override { supportedModes_ = modes; }
    SnapInfo GetCurrentInfo() const override { return currentInfo_; }
    void SetCurrentInfo(const SnapInfo& info) override { currentInfo_ = info; }
	void SetWorkspaceController(const Handle(WorkspaceController)& controller) { workspaceController_ = controller; }
	Handle(WorkspaceController) GetWorkspaceController() const { return workspaceController_; }
    virtual void Cleanup() {}

protected:
    std::pair<SnapModes, gp_Pnt> SnapToShape(const Graphic3d_Vec2i& screenPoint, const TopoDS_Shape& shape)
    {
        if(shape.IsNull())
        {
            return {SnapMode::None, gp_Pnt()};
        }

        if((supportedModes_ & SnapMode::Vertex) && shape.ShapeType() == TopAbs_VERTEX)
        {
            TopoDS_Vertex vertex = TopoDS::Vertex(shape);
            return {SnapMode::Vertex, BRep_Tool::Pnt(vertex)};
        }

        if((supportedModes_ & SnapMode::Edge) && shape.ShapeType() == TopAbs_EDGE)
        {
            TopoDS_Edge edge = TopoDS::Edge(shape);
            Standard_Real umin, umax;
            auto curve = BRep_Tool::Curve(edge, umin, umax);
			if(curve.IsNull() && BRepLib::BuildCurve3d(edge))
			{
                curve = BRep_Tool::Curve(edge, umin, umax);
            }

            if(!curve.IsNull())
            {
                Handle(Geom_Line) ray = new Geom_Line(gp_Pnt(screenPoint.x(), screenPoint.y(), 0), gp_Dir(0, 0, 1));
                GeomAPI_ExtremaCurveCurve extrema(curve, ray);

                if(extrema.NbExtrema() >= 1)
                {
                    gp_Pnt p1, p2;
                    extrema.NearestPoints(p1, p2);
                    return {SnapMode::Edge, p1};
                }
            }
        }

        return {SnapMode::None, gp_Pnt()};
    }

    std::pair<SnapModes, gp_Pnt> SnapToAisObject(const Graphic3d_Vec2i& Graphic3d_Vec2i, const Handle(AIS_InteractiveObject)& aisObject)
    {
        if(aisObject.IsNull())
        {
            return {SnapMode::None, gp_Pnt()};
        }

        if((supportedModes_ & SnapMode::Vertex))
        {
            Handle(AIS_Point) aisPoint = Handle(AIS_Point)::DownCast(aisObject);
            if(!aisPoint.IsNull())
            {
                return {SnapMode::Vertex, aisPoint->Component()->Pnt()};
            }
        }

        return {SnapMode::None, gp_Pnt()};
    }

protected:
    SnapModes supportedModes_;
    SnapInfo currentInfo_;
	Handle(WorkspaceController) workspaceController_;
 };

// 定义句柄
DEFINE_STANDARD_HANDLE(SnapBase, Standard_Transient)

#endif  // IACT_WORKSPACE_SNAPBASE_HXX
