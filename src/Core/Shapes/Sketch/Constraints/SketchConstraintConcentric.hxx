#ifndef SKETCHCONSTRAINTCONCENTRIC_HXX
#define SKETCHCONSTRAINTCONCENTRIC_HXX

#include <map>
#include <memory>
#include <vector>

#include <gp_Pnt2d.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintConcentric : public SketchConstraint
{
public:
    SketchConstraintConcentric(int segment1, int segment2,
                               const TDF_Label& label = DocumentTool::SketchConstraintConcentricsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , myLabel(label)
    {
        assert(!label.HasAttribute());
        TDataStd_Name::Set(label, "SketchConstraintConcentric");
        SetSegments({segment1, segment2});
    }

    SketchConstraintConcentric(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , myLabel(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return myLabel; }

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

        std::vector<Constraint> constraints;
        Constraint con;
        con.type = ConstraintType::ConcentricCircles;

        bool valid = true;
        valid &= solver->SetCircle(con.circle1, it1->second, constraints, points, false, true);
        valid &= solver->SetCircle(con.circle2, it2->second, constraints, points, false, true);

        if(valid)
        {
            for(const auto& constraint : constraints)
            {
                solver->AddConstraint(constraint);
            }
            solver->AddConstraint(con);
        }
        return valid;
    }

    bool CanCreate(const Handle(Sketch)& sketch,
                   const std::vector<int>& points,
                   const std::vector<int>& segments)
    {
        return points.size() == 0
            && segments.size() >= 2
            && (SketchConstraintHelper::AllSegmentsOfType<SketchSegmentCircle>(sketch, segments) ||
                SketchConstraintHelper::AllSegmentsOfType<SketchSegmentArc>(sketch, segments))
            && !SketchConstraintHelper::AnyConstrainedSegment<SketchConstraintConcentric>(sketch, segments);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        if(segments.empty()) return {};
        std::vector<Handle(SketchConstraint)> list;
        const auto& segMap = sketch->Segments();

        for(size_t i = 0; i < segments.size() - 1; ++i)
        {
            auto it1 = segMap.find(segments[i]);
            auto it2 = segMap.find(segments[i + 1]);
            assert(it1 != segMap.end() && it2 != segMap.end());

            Handle(SketchSegment) seg1 = it1->second;
            Handle(SketchSegment) seg2 = it2->second;

            if((seg1->IsKind(STANDARD_TYPE(SketchSegmentCircle)) || seg1->IsKind(STANDARD_TYPE(SketchSegmentArc))) &&
               (seg2->IsKind(STANDARD_TYPE(SketchSegmentCircle)) || seg2->IsKind(STANDARD_TYPE(SketchSegmentArc))))
            {
                list.push_back(MakeHandle<SketchConstraintConcentric>(segments[i], segments[i + 1]));
            }
        }
        return list;
    }

private:
    TDF_Label myLabel;

    DEFINE_STANDARD_RTTI_INLINE(SketchConstraintConcentric, SketchConstraint)
};

DEFINE_STANDARD_HANDLE(SketchConstraintConcentric, SketchConstraint)

#endif // SKETCHCONSTRAINTCONCENTRIC_HXX
