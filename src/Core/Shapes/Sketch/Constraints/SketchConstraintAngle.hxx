#ifndef SketchConstraintAngle_HXX
#define SketchConstraintAngle_HXX

#include <map>
#include <memory>
#include <vector>

#include <TDataStd_Real.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>
#include <Geom2dAPI_InterCurveCurve.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintAngle : public SketchConstraint
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Real, Angle)

public:
    SketchConstraintAngle(int lineSegment1, int lineSegment2, double angle,
                          const TDF_Label& label = DocumentTool::SketchConstraintAnglesLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintAngle");

        SetSegments({lineSegment1, lineSegment2});
        SetParameter(angle);
    }

    SketchConstraintAngle(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    virtual double GetParameter() const { return AngleAttribute()->Get(); }
    virtual void SetParameter(double angle) { AngleAttribute()->Set(_Label, angle); }

    TDF_Label Label() const { return _Label; }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        auto it1 = segments.find(Segment(0));
        auto it2 = segments.find(Segment(1));
        if(it1 == segments.end() || it2 == segments.end())
        {
            return false;
        }

        auto lineSegment1 = Handle(SketchSegmentLine)::DownCast(it1->second);
        auto lineSegment2 = Handle(SketchSegmentLine)::DownCast(it2->second);
        if(lineSegment1.IsNull() || lineSegment2.IsNull())
        {
            return false;
        }

        auto lin2d1 = lineSegment1->MakeCurve(points);
        auto lin2d2 = lineSegment2->MakeCurve(points);

        if(lin2d1.IsNull() || lin2d2.IsNull())
        {
            return false;
        }

        Geom2dAPI_InterCurveCurve intersection(lin2d1, lin2d2);
        if(!intersection.NbPoints())
        {
            return false;
        }
        gp_Pnt2d intPnt = intersection.Point(1);

        auto lin1 = lineSegment1->GetLine(points);
        auto lin2 = lineSegment2->GetLine(points);

        bool reverse1 = intPnt.SquareDistance(lin1.Location()) >
            intPnt.SquareDistance(lin1.Location().Translated(lin1.Direction().XY()));
        bool reverse2 = intPnt.SquareDistance(lin2.Location()) >
            intPnt.SquareDistance(lin2.Location().Translated(lin2.Direction().XY()));

        Constraint con;
        con.type = ConstraintType::InternalAngle;

        bool valid = true;
        valid &= solver->SetLine(con.line1, lineSegment1->PointsAttribute()->Value(reverse1 ? 1 : 0),
                                 lineSegment1->PointsAttribute()->Value(reverse1 ? 0 : 1), false, false);
        valid &= solver->SetLine(con.line2, lineSegment2->PointsAttribute()->Value(reverse2 ? 1 : 0),
                                 lineSegment2->PointsAttribute()->Value(reverse2 ? 0 : 1), false, false);
        valid &= solver->SetParameter(con.parameter, AngleAttribute()->Get() * M_PI / 180.0, true);

        if(valid)
        {
            solver->AddConstraint(con);
        }
        return valid;
    }

    bool CanCreate(const Handle(Sketch)& sketch,
                   const std::vector<int>& points,
                   const std::vector<int>& segments)
    {
        return points.size() == 0
            && segments.size() == 2
            && SketchConstraintHelper::AllSegmentsOfType<SketchSegmentLine>(sketch, segments)
            && !SketchConstraintHelper::AllSegmentsHaveCommonAngleConstraint(sketch, segments);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;
        const auto& segMap = sketch->Segments();

        if(segments.size() < 2)
        {
            return list;
        }

        auto it1 = segMap.find(segments[0]);
        auto it2 = segMap.find(segments[1]);
        if(it1 == segMap.end() || it2 == segMap.end())
        {
            return list;
        }

        Handle(SketchSegmentLine) line1 = Handle(SketchSegmentLine)::DownCast(it1->second);
        Handle(SketchSegmentLine) line2 = Handle(SketchSegmentLine)::DownCast(it2->second);
        if(line1.IsNull() || line2.IsNull())
        {
            return list;
        }
		auto spoints = sketch->Points();
        auto lin2d1 = line1->MakeCurve(spoints);
        auto lin2d2 = line2->MakeCurve(spoints);

        if(lin2d1.IsNull() || lin2d2.IsNull())
        {
            return list;
        }

        Geom2dAPI_InterCurveCurve intersection(lin2d1, lin2d2);
        if(!intersection.NbPoints())
        {
            return list;
        }

        gp_Pnt2d intPnt = intersection.Point(1);
        auto lin1 = line1->GetLine(spoints);
        auto lin2 = line2->GetLine(spoints);

        if(intPnt.SquareDistance(lin1.Location()) >
           intPnt.SquareDistance(lin1.Location().Translated(lin1.Direction().XY())))
        {
            lin1 = lin1.Reversed();
        }
        if(intPnt.SquareDistance(lin2.Location()) >
           intPnt.SquareDistance(lin2.Location().Translated(lin2.Direction().XY())))
        {
            lin2 = lin2.Reversed();
        }

        double angle = lin1.Angle(lin2) * 180.0 / M_PI;
        if(angle > 0.0)
        {
            list.push_back(new SketchConstraintAngle(segments[0], segments[1], angle));
        }

        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintAngle, SketchConstraint)

#endif // SketchConstraintAngle_HXX
