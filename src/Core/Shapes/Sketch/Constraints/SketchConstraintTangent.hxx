#ifndef SketchConstraintTangent_HXX
#define SketchConstraintTangent_HXX

#include <map>
#include <memory>
#include <vector>

#include <gp_Pnt2d.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintTangent : public SketchConstraint
{
public:
    // Constructor for SketchConstraintTangent
    SketchConstraintTangent(int segment, int segTangentTo,
                            const TDF_Label& label = DocumentTool::SketchConstraintTangentsLabel().NewChild())
        : SketchConstraint(label.FindChild(0)), _Label(label)
    {
        assert(!label.HasAttribute());
        TDataStd_Name::Set(label, "SketchConstraintTangent");
        SetSegments({segment, segTangentTo});
    }

    // Constructor to load from label
    SketchConstraintTangent(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0)), _Label(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return _Label; }

    // MakeConstraint method to set tangent constraints
    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        auto constraints = std::vector<Constraint>();

        auto it1 = segments.find(Segment(0));
        auto it2 = segments.find(Segment(1));
        if(it1 == segments.end() || it2 == segments.end())
        {
            return false;
        }

        auto lineSegment = Handle(SketchSegmentLine)::DownCast(it1->second);
        auto tangentToSeg = it2->second;
        if(lineSegment.IsNull() || (!Handle(SketchSegmentCircle)::DownCast(tangentToSeg) &&
           !Handle(SketchSegmentArc)::DownCast(tangentToSeg)))
        {
            return false;
        }

        Constraint con;
        con.type = ConstraintType::TangentToCircle;

        bool valid = true;
        valid &= solver->SetLine(con.line1, lineSegment->PointsAttribute()->Value(0),
                                 lineSegment->PointsAttribute()->Value(1), false, false);
        valid &= solver->SetCircle(con.circle1, tangentToSeg, constraints, points, false, false);

        if(valid)
        {
            solver->AddConstraint(con);
        }

        return valid;
    }

    // CanCreate method to check if tangent constraint can be created
    bool CanCreate(const Handle(Sketch)& sketch,
                   const std::vector<int>& points,
                   const std::vector<int>& segments)
    {
        return points.size() == 0
            && segments.size() == 2
            && SketchConstraintHelper::AnySegmentsOfType<SketchSegmentLine>(sketch, segments)
            && (SketchConstraintHelper::AnySegmentsOfType<SketchSegmentCircle>(sketch, segments) ||
                SketchConstraintHelper::AnySegmentsOfType<SketchSegmentArc>(sketch, segments))
            && !SketchConstraintHelper::AllSegmentsHaveCommonAngleConstraint(sketch, segments);
    }

    // Create method to create tangent constraints
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

            list.push_back(MakeHandle<SketchConstraintTangent>(segments[i], segments[i + 1]));
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintTangent, SketchConstraint)

#endif // SketchConstraintTangent_HXX
