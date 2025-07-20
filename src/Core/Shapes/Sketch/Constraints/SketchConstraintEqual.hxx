#ifndef SketchConstraintEqual_HXX
#define SketchConstraintEqual_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintEqual : public SketchConstraint
{
public:
    SketchConstraintEqual(int segment1, int segment2,
                          const TDF_Label& label = DocumentTool::SketchConstraintEqualsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());
        TDataStd_Name::Set(label, "SketchConstraintEqual");
        SetSegments({segment1, segment2});
    }

    SketchConstraintEqual(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

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

        if(Handle(SketchSegmentLine) lineSegment1 = Handle(SketchSegmentLine)::DownCast(it1->second))
        {
            Handle(SketchSegmentLine) lineSegment2 = Handle(SketchSegmentLine)::DownCast(it2->second);
            if(lineSegment2.IsNull())
            {
                return false;
            }

            Constraint con;
            con.type = ConstraintType::EqualLength;

            bool valid = true;
            valid &= solver->SetLine(con.line1, lineSegment1->PointsAttribute()->Value(0),
                                     lineSegment1->PointsAttribute()->Value(1), false, false);
            valid &= solver->SetLine(con.line2, lineSegment2->PointsAttribute()->Value(0),
                                     lineSegment2->PointsAttribute()->Value(1), false, false);

            if(valid)
            {
                solver->AddConstraint(con);
            }
            return valid;
        }

        if(Handle(SketchSegmentCircle) circleSegment1 = Handle(SketchSegmentCircle)::DownCast(it1->second))
        {
            Handle(SketchSegmentCircle) circleSegment2 = Handle(SketchSegmentCircle)::DownCast(it2->second);
            if(circleSegment2.IsNull())
            {
                return false;
            }

            std::vector<Constraint> constraints;
            Constraint con;
            con.type = ConstraintType::EqualRadiusCircles;

            bool valid = true;
            valid &= solver->SetCircle(con.circle1, circleSegment1, constraints, points, true, false);
            valid &= solver->SetCircle(con.circle2, circleSegment2, constraints, points, true, false);
            constraints.push_back(con);

            if(valid)
            {
                for(const auto& constraint : constraints)
                {
                    solver->AddConstraint(constraint);
                }
            }
            return valid;
        }

        return false;
    }

    bool CanCreate(const Handle(Sketch)& sketch,
                   const std::vector<int>& points,
                   const std::vector<int>& segments)
    {
        return points.size() == 0
            && segments.size() >= 2
            && (SketchConstraintHelper::AllSegmentsOfType<SketchSegmentCircle>(sketch, segments) ||
                SketchConstraintHelper::AllSegmentsOfType<SketchSegmentLine>(sketch, segments))
            && !SketchConstraintHelper::AnyConstrainedSegment<SketchConstraintEqual>(sketch, segments);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;
        const auto& segMap = sketch->Segments();

        for(size_t i = 0; i < segments.size() - 1; ++i)
        {
            auto it1 = segMap.find(segments[i]);
            auto it2 = segMap.find(segments[i + 1]);
            if(it1 == segMap.end() || it2 == segMap.end())
            {
                continue;
            }

            Handle(SketchSegment) seg1 = it1->second;
            Handle(SketchSegment) seg2 = it2->second;
            if((seg1->IsKind(STANDARD_TYPE(SketchSegmentCircle)) || seg1->IsKind(STANDARD_TYPE(SketchSegmentLine))) &&
               (seg2->IsKind(STANDARD_TYPE(SketchSegmentCircle)) || seg2->IsKind(STANDARD_TYPE(SketchSegmentLine))))
            {
                list.push_back(new SketchConstraintEqual(segments[i], segments[i + 1]));
            }
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintEqual, SketchConstraint)

#endif // SketchConstraintEqual_HXX
