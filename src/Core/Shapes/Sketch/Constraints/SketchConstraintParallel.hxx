//! class SketchConstraintParallel
//! 
//! 
#ifndef SKETCHCONSTRAINTPARALLEL_HXX
#define SKETCHCONSTRAINTPARALLEL_HXX

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

class SketchConstraintParallel : public SketchConstraint
{
public:
    SketchConstraintParallel(int lineSegment1, 
                             int lineSegment2, 
                             const TDF_Label& label = DocumentTool::SketchConstraintParallelsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , myLabel(label)
    {
        TDataStd_Name::Set(label, "SketchConstraintParallel");

        SetSegments({lineSegment1, lineSegment2});
    }

    //! Constructor
    SketchConstraintParallel(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , myLabel(label)
    {
    };

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

        auto lineSegment1 = Handle(SketchSegmentLine)::DownCast(it1->second);
        if(lineSegment1.IsNull()) return false;
        auto lineSegment2 = Handle(SketchSegmentLine)::DownCast(it2->second);
        if(lineSegment2.IsNull()) return false;

        Constraint con;
        con.type = ConstraintType::Parallel;

        bool valid = !lineSegment1->IsConnected(lineSegment2);
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
        if(points.size() != 0 || segments.size() != 2) return false;
        const auto& segMap = sketch->Segments();
        if(segMap.find(segments[0]) == segMap.end() || segMap.find(segments[1]) == segMap.end())
        {
            return false;
        }
        return SketchConstraintHelper::AllSegmentsOfType<SketchSegment>(sketch, segments) &&
            !SketchConstraintHelper::AllSegmentsHaveCommonAngleConstraint(sketch, segments) &&
            !segMap.at(segments[0])->IsConnected(segMap.at(segments[1]));
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

            auto v1 = it1->second;
            Handle(SketchSegmentLine) line1 = Handle(SketchSegmentLine)::DownCast(it1->second);
            assert(!line1.IsNull());
            Handle(SketchSegmentLine) line2 = Handle(SketchSegmentLine)::DownCast(it2->second);
            assert(!line2.IsNull());

            if(!line1->IsConnected(line2))
            {
                list.push_back(MakeHandle<SketchConstraintParallel>(segments[i], segments[i + 1]));
            }
        }
        return list;
    }
private:
    TDF_Label myLabel;

    DEFINE_STANDARD_RTTI_INLINE(SketchConstraintParallel, SketchConstraint);
};

DEFINE_STANDARD_HANDLE(SketchConstraintParallel, SketchConstraint)

#endif  // SKETCHCONSTRAINTPARALLEL_HXX
