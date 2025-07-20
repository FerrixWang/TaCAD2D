#ifndef SketchConstraintPerpendicular_HXX
#define SketchConstraintPerpendicular_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintPerpendicular : public SketchConstraint
{
public:
    SketchConstraintPerpendicular(int lineSegment1, int lineSegment2,
                                  const TDF_Label& label = DocumentTool::SketchConstraintPerpendicularsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintPerpendicular");

        SetSegments({lineSegment1, lineSegment2});
    }

    SketchConstraintPerpendicular(const TDF_Label& label)
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

        auto lineSegment1 = Handle(SketchSegmentLine)::DownCast(it1->second);
        auto lineSegment2 = Handle(SketchSegmentLine)::DownCast(it2->second);
        if(lineSegment1.IsNull() || lineSegment2.IsNull())
        {
            return false;
        }

        Constraint con;
        con.type = ConstraintType::Perpendicular;

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

        for(size_t i = 0; i < segments.size() - 1; ++i)
        {
            auto it1 = segMap.find(segments[i]);
            auto it2 = segMap.find(segments[i + 1]);
            if(it1 == segMap.end() || it2 == segMap.end())
            {
                continue;
            }

            Handle(SketchSegmentLine) line1 = Handle(SketchSegmentLine)::DownCast(it1->second);
            Handle(SketchSegmentLine) line2 = Handle(SketchSegmentLine)::DownCast(it2->second);
            if(line1.IsNull() || line2.IsNull())
            {
                continue;
            }

            list.push_back(new SketchConstraintPerpendicular(segments[i], segments[i + 1]));
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintPerpendicular, SketchConstraint)

#endif // SketchConstraintPerpendicular_HXX
