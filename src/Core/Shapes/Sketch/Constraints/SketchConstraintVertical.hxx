//! class SketchConstraintVertical
//! 
//! 
#ifndef SketchConstraintVertical_HXX
#define SketchConstraintVertical_HXX

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

class SketchConstraintVertical : public SketchConstraint
{
public:
    SketchConstraintVertical(int lineSegment, 
                             const TDF_Label& label = DocumentTool::SketchConstraintVerticalsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , myLabel(label)
    {
        TDataStd_Name::Set(label, "SketchConstraintVertical");

        SetSegments({lineSegment});
    }

    //! Constructor
    SketchConstraintVertical(const TDF_Label& label)
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
        if(it1 == segments.end())
        {
            return false;
        }

        auto lineSegment1 = Handle(SketchSegmentLine)::DownCast(it1->second);
        if(lineSegment1.IsNull()) return false;

        Constraint con;
        con.type = ConstraintType::Vertical;

        bool valid = solver->SetLine(con.line1, lineSegment1->PointsAttribute()->Value(0),
                                lineSegment1->PointsAttribute()->Value(1), false, false);
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
            && segments.size() > 0
            && SketchConstraintHelper::AllSegmentsOfType<SketchSegmentLine>(sketch, segments);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch, 
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments) 
    {
        std::vector<Handle(SketchConstraint)> list;
        const auto& segMap = sketch->Segments();

        for(size_t i = 0; i < segments.size(); ++i)
        {
            auto it1 = segMap.find(segments[i]);
            assert(it1 != segMap.end());

            Handle(SketchSegmentLine) line1 = Handle(SketchSegmentLine)::DownCast(it1->second);
            assert(!line1.IsNull());

            list.push_back(MakeHandle<SketchConstraintVertical>(segments[i]));

        }
        return list;
    }
private:
    TDF_Label myLabel;

    DEFINE_STANDARD_RTTI_INLINE(SketchConstraintVertical, SketchConstraint);
};

DEFINE_STANDARD_HANDLE(SketchConstraintVertical, SketchConstraint)

#endif  // SketchConstraintVertical_HXX
