#ifndef IACT_SNAPPING_SnapOnCurve2D_HXX
#define IACT_SNAPPING_SnapOnCurve2D_HXX

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <BRep_Tool.hxx>
#include <BRepLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <GeomAPI.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <ProjLib.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

#include "Iact/InteractiveContext.hxx"
#include "Iact/Workspace/SnapBase.hxx"
#include "Iact/WorkspaceController.hxx"

class SnapInfoCurve2D
{
public:
    SnapModes Mode;
    gp_Pnt2d Point;
    double Parameter;
    double TargetParameter;
    TopoDS_Shape TargetBrepShape;
    Handle(AIS_InteractiveObject) TargetAisObject;

    SnapInfoCurve2D(SnapModes mode, const gp_Pnt2d& point, double param)
        : Mode(mode), Point(point), Parameter(param), TargetParameter(0)
    {}

    SnapInfoCurve2D()
        : SnapInfoCurve2D(SnapMode::None, gp_Pnt2d(0, 0), 0)
    {}
};

class SnapOnCurve2D
{
public:
    // ¹¹Ôìº¯Êý
    SnapOnCurve2D()
        : SupportedModes(SnapMode::Edge)
    {}

    virtual ~SnapOnCurve2D() = default;

    SnapInfoCurve2D Snap(const Handle(Geom2d_Curve)& curve,
                         const Handle(MouseEventData)& mouseEvent)
    {
        SnapInfoCurve2D snapInfo;
        return snapInfo;
    }

private:
    SnapInfoCurve2D _Snap(const Handle(Geom2d_Curve)& curve,
                          const TopoDS_Shape& shapeToSnap,
                          const Handle(AIS_InteractiveObject)& aisObject)
    {
        SnapInfoCurve2D snapInfo;

        if(SupportedModes & SnapMode::Edge
           && shapeToSnap.ShapeType() == TopAbs_ShapeEnum::TopAbs_EDGE)
        {
            auto edge = TopoDS::Edge(shapeToSnap);
            double umin, umax;
            Handle(Geom_Curve) edgeCurve = BRep_Tool::Curve(edge, umin, umax);
            if(edgeCurve.IsNull() && BRepLib::BuildCurve3d(edge))
            {
                edgeCurve = BRep_Tool::Curve(edge, umin, umax);
            }

            if(!edgeCurve.IsNull())
            {
                Handle(Geom2d_Curve) projected2dCurve = GeomAPI::To2d(edgeCurve, _Plane);
                Geom2dAPI_InterCurveCurve intersect(curve, projected2dCurve);
                if(intersect.NbPoints() > 0)
                {
                    IntRes2d_IntersectionPoint Point1 = intersect.Intersector().Point(1);
                    SnapInfoCurve2D result(SnapMode::Edge, intersect.Point(1), Point1.ParamOnFirst());
                    result.TargetParameter = Point1.ParamOnSecond();
                    result.TargetBrepShape = shapeToSnap;
                    result.TargetAisObject = aisObject;
                    return result;
                }
            }
        }

        return snapInfo;
    }

    SnapInfoCurve2D _SnapGrid(const Handle(Geom2d_Curve)& curve,
                              int screenPointX,
                              int screenPointY)
    {
        SnapInfoCurve2D snapInfo;
        double currentDistance = 0.1;

        return snapInfo;
    }

private:
    gp_Pln _Plane;
    SnapModes SupportedModes;
    bool _IsPlaneSet = false;
};

DEFINE_STANDARD_HANDLE(SnapOnCurve2D, SnapBase)

#endif // IACT_SNAPPING_SnapOnCurve2D_HXX
