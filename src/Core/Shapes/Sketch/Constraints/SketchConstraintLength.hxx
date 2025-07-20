//! class SketchConstraintLineLength
//! 
//! 
#ifndef SketchConstraintLineLength_HXX
#define SketchConstraintLineLength_HXX

#include <map>
#include <memory>
#include <vector>

#include <TDataStd_Real.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintLength : public SketchConstraint
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Real, Length)

public:
    SketchConstraintLength(int lineSegment, double length,
                             const TDF_Label& label = DocumentTool::SketchConstraintLengthsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintLineLength");

        SetSegments({lineSegment});
		SetParameter(length);
    }

    //! Constructor
    SketchConstraintLength(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    };

    // Parameter property (virtual get/set)
    virtual double GetParameter() const { return LengthAttribute()->Get(); }
    virtual void SetParameter(double length) { LengthAttribute()->Set(_Label, length); }

    TDF_Label Label() const { return _Label; }

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
        con.type = ConstraintType::LineLength;

        bool valid = LengthAttribute()->Get() > 0.0;
        valid &= solver->SetParameter(con.parameter, LengthAttribute()->Get(), true);
		valid &= solver->SetLine(con.line1, lineSegment1->PointsAttribute()->Value(0),
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

			auto length = line1->Length(sketch->Points());
            if(length > 0.0)
            {
                list.push_back(new SketchConstraintLength(segments[i], length));
            }
        }
        return list;
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintLength, SketchConstraint)

#endif  // SketchConstraintLineLength_HXX
