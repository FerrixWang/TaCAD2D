
#include "Core/Shapes/Sketch/SketchUtils.hxx"

#include <algorithm>

#include <ElCLib.hxx>
#include <gp_Circ2d.hxx>
#include <Geom2d_Circle.hxx>

#include "Core/Geom/Geom2dUtils.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipse.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipticalArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"

// Static member initialization
const SketchUtils::SplitSegmentResult SketchUtils::SplitSegmentResult::Failed = {-1, {}};
const std::vector<int> SketchUtils::SplitPointFailed = {};
const int SketchUtils::ConvertSegmentFailed = -1;

// Connectivity utilities
bool SketchUtils::IsUnconnectedEndpoint(const Handle(Sketch)& sketch, int point)
{
    bool foundOnce = false;
    for(const auto& segmentKvp : sketch->Segments())
    {
        if(segmentKvp.second->StartPoint() != point && segmentKvp.second->EndPoint() != point)
            continue;

        if(foundOnce)
            return false;

        foundOnce = true;
    }
    return foundOnce;
}

std::vector<int> SketchUtils::GetSegmentsUsingPoint(const Handle(Sketch)& sketch, int point)
{
    return GetSegmentsUsingPoint(sketch->Segments(), point);
}

std::vector<int> SketchUtils::GetSegmentsUsingPoint(const std::map<int, Handle(SketchSegment)>& segments, int point)
{
    std::vector<int> result;
    for(const auto& segmentKvp : segments)
    {
        const auto& points = segmentKvp.second->PointsAttribute();
        for(int i = points->Lower(); i <= points->Upper(); i++)
        {
            if(points->Value(i) == point)
            {
                result.push_back(segmentKvp.first);
                break;
            }
        }
    }
    return result;
}

bool SketchUtils::ReplaceSegmentPoint(const Handle(SketchSegment)& segment, int oldPointIndex, int newPointIndex)
{
	auto pointAttr = segment->PointsAttribute();
    for(int i = pointAttr->Lower(); i <= pointAttr->Upper(); i++)
    {
        if(pointAttr->Value(i) == oldPointIndex)
        {
            pointAttr->SetValue(i, newPointIndex);
            return true;
        }
    }

    return false;
}

// Split segment
bool SketchUtils::CanSplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment)
{
    if(std::find_if(sketch->Segments().begin(), sketch->Segments().end(),
       [segment](const auto& kvp) { return kvp.second == segment; }) == sketch->Segments().end())
        return false;

    if(!Handle(SketchSegmentLine)::DownCast(segment).IsNull() ||
       !Handle(SketchSegmentCircle)::DownCast(segment).IsNull() ||
       !Handle(SketchSegmentArc)::DownCast(segment).IsNull() ||
       !Handle(SketchSegmentEllipse)::DownCast(segment).IsNull() ||
       !Handle(SketchSegmentEllipticalArc)::DownCast(segment).IsNull() ||
       !Handle(SketchSegmentBezier)::DownCast(segment).IsNull())
    {

        return true;
    }
    return false;
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, double u)
{
    if(std::find_if(sketch->Segments().begin(), sketch->Segments().end(),
       [segment](const auto& kvp) { return kvp.second == segment; }) == sketch->Segments().end())
        return SplitSegmentResult::Failed;

    if(Handle(SketchSegmentLine) line = Handle(SketchSegmentLine)::DownCast(segment))
        return SplitSegment(sketch, line, u);
    if(Handle(SketchSegmentCircle) circle = Handle(SketchSegmentCircle)::DownCast(segment))
        return SplitSegment(sketch, circle, u);
    if(Handle(SketchSegmentArc) arc = Handle(SketchSegmentArc)::DownCast(segment))
        return SplitSegment(sketch, arc, u);
    if(Handle(SketchSegmentEllipse) ellipse = Handle(SketchSegmentEllipse)::DownCast(segment))
        return SplitSegment(sketch, ellipse, u);
    if(Handle(SketchSegmentEllipticalArc) ellipticalArc = Handle(SketchSegmentEllipticalArc)::DownCast(segment))
        return SplitSegment(sketch, ellipticalArc, u);
    if(Handle(SketchSegmentBezier) bezier = Handle(SketchSegmentBezier)::DownCast(segment))
        return SplitSegment(sketch, bezier, u);

    return SplitSegmentResult::Failed;
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentLine)& line, double u)
{
    Handle(Geom2d_Curve) curve = line->MakeCurve(sketch->Points());
    if(curve.IsNull() || u <= curve->FirstParameter() || u >= curve->LastParameter())
        return SplitSegmentResult::Failed;

    int splitPointIndex = sketch->AddPoint(curve->Value(u));

    Handle(SketchSegmentLine) newLine1 = new SketchSegmentLine(line->StartPoint(), splitPointIndex);
    int seg1Index = sketch->AddSegment(newLine1);

    Handle(SketchSegmentLine) newLine2 = new SketchSegmentLine(splitPointIndex, line->EndPoint());
    int seg2Index = sketch->AddSegment(newLine2);

    sketch->DeleteSegment(line);

    return {splitPointIndex, {seg1Index, seg2Index}};
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentCircle)& circle, double u)
{
    u = Math::NormalizeAngleRad(u);
    if(u == 0.0)
        return SplitSegmentResult::Failed;

    Handle(Geom2d_Curve) curve = circle->MakeCurve(sketch->Points());
    if(curve.IsNull())
        return SplitSegmentResult::Failed;

    int splitPointIndex = sketch->AddPoint(curve->Value(u));

    Geom2dAPI_ProjectPointOnCurve pointOnCurve(sketch->Points().at(circle->RimPoint()), curve);
    if(pointOnCurve.NbPoints() < 1)
        return SplitSegmentResult::Failed;
    double oldRimParameter = pointOnCurve.LowerDistanceParameter();
    double rimOffset = (u - oldRimParameter) * 0.5;

    int rim1Index = sketch->AddPoint(curve->Value(oldRimParameter + rimOffset));
    Handle(SketchSegmentArc) newArc1 = new SketchSegmentArc(circle->RimPoint(), splitPointIndex, rim1Index);
    int seg1Index = sketch->AddSegment(newArc1);

    int rim2Index = sketch->AddPoint(curve->Value(oldRimParameter + Math::PI + rimOffset));
    Handle(SketchSegmentArc) newArc2 = new SketchSegmentArc(splitPointIndex, circle->RimPoint(), rim2Index);
    int seg2Index = sketch->AddSegment(newArc2);

    sketch->DeleteSegment(circle);

    return {splitPointIndex, {seg1Index, seg2Index}};
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentArc)& arc, double u)
{
    std::vector<double> parameters(2);
    Handle(Geom2d_Circle) circ = new Geom2d_Circle(arc->GetCircle(sketch->Points(), parameters));
    if(circ.IsNull())
        return SplitSegmentResult::Failed;

    u = Math::NormalizeAngleRad(u);
    if(u <= parameters[0] || u >= parameters[1])
        return SplitSegmentResult::Failed;

    int splitPointIndex = sketch->AddPoint(ElCLib::Value(u, circ->Circ2d()));

    int rim1Index = sketch->AddPoint(ElCLib::Value((parameters[0] + u) * 0.5, circ->Circ2d()));
    Handle(SketchSegmentArc) newArc1 = new SketchSegmentArc(arc->StartPoint(), splitPointIndex, rim1Index);
    int seg1Index = sketch->AddSegment(newArc1);

    int rim2Index = sketch->AddPoint(ElCLib::Value((parameters[1] + u) * 0.5, circ->Circ2d()));
    Handle(SketchSegmentArc) newArc2 = new SketchSegmentArc(splitPointIndex, arc->EndPoint(), rim2Index);
    int seg2Index = sketch->AddSegment(newArc2);

    sketch->DeleteSegment(arc);

    return {splitPointIndex, {seg1Index, seg2Index}};
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentEllipse)& ellipse, double u)
{
    u = Math::NormalizeAngleRad(u);
    if(u == 0.0)
        return SplitSegmentResult::Failed;

    Handle(Geom2d_Curve) curve = ellipse->MakeCurve(sketch->Points());
    if(curve.IsNull())
        return SplitSegmentResult::Failed;

    int splitPointIndex = sketch->AddPoint(curve->Value(u));
    int majorAxisPointIndex = ellipse->GetMajorAxisPoint(sketch->Points());

    Handle(SketchSegmentEllipticalArc) newArc1 = new SketchSegmentEllipticalArc(majorAxisPointIndex, splitPointIndex, ellipse->CenterPoint());
    int seg1Index = sketch->AddSegment(newArc1);

    Handle(SketchSegmentEllipticalArc) newArc2 = new SketchSegmentEllipticalArc(splitPointIndex, majorAxisPointIndex, ellipse->CenterPoint());
    int seg2Index = sketch->AddSegment(newArc2);

    sketch->DeleteSegment(ellipse);

    return {splitPointIndex, {seg1Index, seg2Index}};
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentEllipticalArc)& ellipticalArc, double u)
{
    std::vector<double> parameters(2);
    Handle(Geom2d_Ellipse) ellipse = new Geom2d_Ellipse(ellipticalArc->GetEllipse(sketch->Points(), parameters));
    if(ellipse.IsNull())
        return SplitSegmentResult::Failed;

    if(parameters[1] < parameters[0])
        parameters[1] += Math::DoublePI;

    u = Math::NormalizeAngleRad(u);
    if(u <= parameters[0] || u >= parameters[1])
        return SplitSegmentResult::Failed;

    bool startIsMajor = ellipticalArc->StartPoint() == ellipticalArc->GetMajorAxisPoint(sketch->Points());
    int splitPointIndex = sketch->AddPoint(ElCLib::Value(u, ellipse->Elips2d()));

    std::vector<int> newSegments;
    Handle(SketchSegmentEllipticalArc) newArc = startIsMajor
        ? new SketchSegmentEllipticalArc(ellipticalArc->StartPoint(), splitPointIndex, ellipticalArc->CenterPoint())
        : new SketchSegmentEllipticalArc(splitPointIndex, ellipticalArc->EndPoint(), ellipticalArc->CenterPoint());
    newSegments.push_back(sketch->AddSegment(newArc));

    sketch->DeleteSegment(ellipticalArc);

    return {splitPointIndex, newSegments};
}

SketchUtils::SplitSegmentResult SketchUtils::SplitSegment(const Handle(Sketch)& sketch, const Handle(SketchSegmentBezier)& bezier, double u)
{
    if(u <= 0.0 || u >= 1.0)
        return SplitSegmentResult::Failed;

    Handle(Geom2d_Curve) curve = bezier->MakeCurve(sketch->Points());
    if(curve.IsNull())
        return SplitSegmentResult::Failed;

    gp_Pnt2d splitPoint = curve->Value(u);
    int splitPointIndex = sketch->AddPoint(splitPoint);

    const auto& points = sketch->Points();
    gp_Pnt2d p1 = points.at(bezier->StartPoint());
    gp_Pnt2d p2 = points.at(bezier->EndPoint());

    Handle(SketchSegmentBezier) newBezier1, newBezier2;

    if(bezier->Degree() == 2)
    {
        gp_Pnt2d cp1 = points.at(bezier->PointsAttribute()->Value(1));
        gp_Pnt2d cp11 = p1.Translated(gp_Vec2d(p1, cp1) * u);
        newBezier1 = new SketchSegmentBezier(bezier->StartPoint(), sketch->AddPoint(cp11), splitPointIndex);

        gp_Pnt2d cp21 = p2.Translated(gp_Vec2d(p2, cp1) * (1.0 - u));
        newBezier2 = new SketchSegmentBezier(splitPointIndex, sketch->AddPoint(cp21), bezier->EndPoint());
    }
    else if(bezier->Degree() == 3)
    {
        gp_Pnt2d cp1 = points.at(bezier->PointsAttribute()->Value(1));
        gp_Pnt2d cp2 = points.at(bezier->PointsAttribute()->Value(2));
        gp_Pnt2d splitCp = cp1.Translated(gp_Vec2d(cp1, cp2) * u);

        gp_Pnt2d cp11 = p1.Translated(gp_Vec2d(p1, cp1) * u);
        gp_Pnt2d cp12 = cp11.Translated(gp_Vec2d(cp11, splitCp) * u);
        newBezier1 = new SketchSegmentBezier(bezier->StartPoint(), sketch->AddPoint(cp11), sketch->AddPoint(cp12), splitPointIndex);

        gp_Pnt2d cp22 = p2.Translated(gp_Vec2d(p2, cp2) * (1.0 - u));
        gp_Pnt2d cp21 = cp22.Translated(gp_Vec2d(cp22, splitCp) * (1.0 - u));
        newBezier2 = new SketchSegmentBezier(splitPointIndex, sketch->AddPoint(cp21), sketch->AddPoint(cp22), bezier->EndPoint());
    }
    else
    {
        return SplitSegmentResult::Failed;
    }

    int seg1Index = sketch->AddSegment(newBezier1);
    int seg2Index = sketch->AddSegment(newBezier2);

    sketch->DeleteSegment(bezier);

    return {splitPointIndex, {seg1Index, seg2Index}};
}

// Split point
std::vector<int> SketchUtils::SplitPoint(const Handle(Sketch)& sketch, int point)
{
    std::vector<int> segmentIds = GetSegmentsUsingPoint(sketch, point);
    if(segmentIds.size() <= 1)
        return SplitPointFailed;

    std::vector<int> newPoints(segmentIds.size() - 1);

    return newPoints;
}

// Delete point
Handle(SketchSegment) SketchUtils::DeletePointTrySubstituteSegments(const Handle(Sketch)& sketch, int point)
{
    return nullptr;
}

// Convert segment
bool SketchUtils::CanConvertSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, const opencascade::handle<Standard_Type>& targetType)
{
    return false;
}

int SketchUtils::ConvertSegment(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment, const opencascade::handle<Standard_Type>& targetType)
{

    return -1;
}

int SketchUtils::ConvertSegmentToLine(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment)
{
    return -1;
}

int SketchUtils::ConvertSegmentToBezier(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment)
{

    return -1;
}

int SketchUtils::ConvertSegmentToArc(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment)
{
    return -1;
}

int SketchUtils::ConvertSegmentToEllipticalArc(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment)
{
    Handle(SketchSegmentArc) arcSegment = Handle(SketchSegmentArc)::DownCast(segment);
    if(arcSegment.IsNull())
        return ConvertSegmentFailed;

    int startPoint = segment->StartPoint();
    int endPoint = segment->EndPoint();
    int centerPoint = sketch->AddPoint(arcSegment->Center(sketch->Points()));

    Handle(SketchSegmentEllipticalArc) newSegment = new SketchSegmentEllipticalArc(startPoint, endPoint, centerPoint);
    int newSegIndex = sketch->AddSegment(newSegment);
    sketch->DeleteSegment(segment);
    return newSegIndex;
}

// Welding
bool SketchUtils::WeldPoints(const Handle(Sketch)& sketch, const std::vector<int>& pointIndices)
{

    return true;
}

bool SketchUtils::WeldPointsWithSegments(const Handle(Sketch)& sketch, const std::vector<Handle(SketchSegment)>& segments, const std::vector<int>& pointIndices)
{
    bool result = false;

    return result;
}

bool SketchUtils::WeldSegments(const Handle(Sketch)& sketch, const std::vector<Handle(SketchSegment)>& segments)
{

    return -1;
}

bool SketchUtils::TryWeldTangentSegments(const Handle(Sketch)& sketch, const Handle(SketchSegment)& segment,
                                         const Handle(Geom2d_Curve)& curve, const Handle(SketchSegment)& toolSeg)
{
    if(toolSeg->IsPeriodic())
        return false;

    double us, ue;
    bool foundUs = FindParameter(curve, sketch->Points().at(toolSeg->StartPoint()), us);
    bool foundUe = FindParameter(curve, sketch->Points().at(toolSeg->EndPoint()), ue);
    if(foundUs && foundUe)
    {
        if(WeldPointsWithSegments(sketch, {segment}, {toolSeg->StartPoint(), toolSeg->EndPoint()}))
        {
            sketch->DeleteSegment(toolSeg);
            return true;
        }
    }
    else if(foundUs)
        return WeldPointsWithSegments(sketch, {segment}, {toolSeg->StartPoint()});
    else if(foundUe)
        return WeldPointsWithSegments(sketch, {segment}, {toolSeg->EndPoint()});

    return false;
}

int SketchUtils::SplitSegmentAtPoint(const Handle(Sketch)& sketch, std::map<Handle(SketchSegment), Handle(Geom2d_Curve)>& segInfos, const gp_Pnt2d& point)
{
    for(auto it = segInfos.begin(); it != segInfos.end(); ++it)
    {
        Handle(SketchSegment) segment = it->first;
        Handle(Geom2d_Curve) curve = it->second;

        double u;
        if(!FindParameter(curve, point, u))
            continue;

        SplitSegmentResult splitResult = SplitSegment(sketch, segment, u);
        //if(splitResult == SplitSegmentResult::Failed)
        //    continue;

        segInfos.erase(it);
        for(int newSegIndex : splitResult.NewSegments)
        {
            Handle(SketchSegment) newSegment = sketch->Segments().at(newSegIndex);
            segInfos[newSegment] = newSegment->MakeCurve(sketch->Points());
        }

        return splitResult.SplitPoint;
    }

    return -1;
}

bool SketchUtils::FindParameter(const Handle(Geom2d_Curve)& curve, const gp_Pnt2d& point, double& u)
{
    u = 0.0;
    Geom2dAPI_ProjectPointOnCurve projector(point, curve);
    if(projector.NbPoints() == 0 || projector.LowerDistance() > 0.01)
        return false;

    u = projector.LowerDistanceParameter();
    if(u <= curve->FirstParameter() || u >= curve->LastParameter())
        return false;

    return true;
}
