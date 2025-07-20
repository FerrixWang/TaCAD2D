#ifndef SKETCHUTILS_HXX
#define SKETCHUTILS_HXX

#include <map>
#include <vector>

#include <Geom2d_BoundedCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom2dConvert_BSplineCurveToBezierCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <Standard.hxx>

#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipse.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipticalArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchUtils
{
public:
    // Connectivity utilities
    static bool IsUnconnectedEndpoint(const Handle(Sketch)& sketch, int point);
    static std::vector<int> GetSegmentsUsingPoint(const Handle(Sketch)& sketch, int point);
    static std::vector<int> GetSegmentsUsingPoint(const std::map<int, Handle(SketchSegment)>& segments, int point);
    static bool ReplaceSegmentPoint(const Handle(SketchSegment)& segment, int oldPointIndex, int newPointIndex);

    // Split segment
    struct SplitSegmentResult
    {
        int SplitPoint;
        std::vector<int> NewSegments;
        static const SplitSegmentResult Failed;
    };

    static bool CanSplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentLine)& line, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentCircle)& circle, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentArc)& arc, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentEllipse)& ellipse, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentEllipticalArc)& ellipticalArc, double u);
    static SplitSegmentResult SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentBezier)& bezier, double u);

    // Split point
    static std::vector<int> SplitPoint(const Handle(Sketch)& sketch, int point);
    static const std::vector<int> SplitPointFailed;

    // Delete point
    static Handle(SketchSegment) DeletePointTrySubstituteSegments(const Handle(Sketch)& sketch, int point);

    // Convert segment
    static const int ConvertSegmentFailed;

    static bool CanConvertSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, const opencascade::handle<Standard_Type>& targetType);
    static int ConvertSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, const opencascade::handle<Standard_Type>& targetType);
    static int ConvertSegmentToLine(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment);
    static int ConvertSegmentToBezier(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment);
    static int ConvertSegmentToArc(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment);
    static int ConvertSegmentToEllipticalArc(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment);

    // Welding
    static bool WeldPoints(const Handle(Sketch)& sketch, const std::vector<int>& pointIndices);
    static bool WeldPointsWithSegments(const Handle(Sketch)& sketch, const std::vector<Handle(SketchSegment)>& segments, const std::vector<int>& pointIndices);
    static bool WeldSegments(const Handle(Sketch)& sketch, const std::vector<Handle(SketchSegment)>& segments);

private:
    // Internal welding helpers
    static bool TryWeldTangentSegments(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment,
                                       const Handle(Geom2d_Curve)& curve, const Handle(SketchSegment)& toolSeg);
    static int SplitSegmentAtPoint(const Handle(Sketch)& sketch, std::map<Handle(SketchSegment), Handle(Geom2d_Curve)>& segInfos, const gp_Pnt2d& point);
    static bool FindParameter(const Handle(Geom2d_Curve)& curve, const gp_Pnt2d& point, double& u);
};

#endif // SKETCHUTILS_HXX
