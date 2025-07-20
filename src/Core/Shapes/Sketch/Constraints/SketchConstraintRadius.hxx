// class SketchConstraintRadius
//
// Implements a constraint that fixes the radius of a circle or arc.

#ifndef SketchConstraintRadius_HXX
#define SketchConstraintRadius_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchConstraintRadius : public SketchConstraint
{
public:
    SketchConstraintRadius(int segment, double radius,
                           const TDF_Label& label = DocumentTool::SketchConstraintRadiusLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintRadius");
        TDataStd_Real::Set(label, radius);

        SetSegments({segment});
    }

    SketchConstraintRadius(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return _Label; }

    double Radius() const
    {
        Handle(TDataStd_Real) attr;
        if(_Label.FindAttribute(TDataStd_Real::GetID(), attr))
        {
            return attr->Get();
        }
        return 0.0;
    }

    void SetRadius(double radius)
    {
        TDataStd_Real::Set(_Label, radius);
    }

    double GetParameter() const override
    {
        return Radius();
    }

    void SetParameter(double radius) override
    {
        SetRadius(radius);
    }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        double radius = Radius();
        if(radius <= 0.0)
            return false;

        auto it = segments.find(Segment(0));
        if(it == segments.end())
            return false;

        Handle(SketchSegment) segment = it->second;
        std::vector<Constraint> constraints;
        bool valid = true;

        Constraint con;
        con.type = ConstraintType::CircleRadius;

        if(Handle(SketchSegmentCircle) circle = Handle(SketchSegmentCircle)::DownCast(segment))
        {
            valid &= solver->SetCircle(con.circle1, circle, constraints, points, false, false);
            valid &= solver->SetParameter(con.parameter, radius, true);
            constraints.push_back(con);
        }
        else if(Handle(SketchSegmentArc) arc = Handle(SketchSegmentArc)::DownCast(segment))
        {
            valid &= solver->SetCircle(con.circle1, arc, constraints, points, false, false);
            valid &= solver->SetParameter(con.parameter, radius, true);
            constraints.push_back(con);
        }
        else
        {
            return false;
        }

        if(valid)
        {
            for(const auto& c : constraints)
            {
                solver->AddConstraint(c);
            }
        }

        return true;
    }

    virtual Handle(SketchConstraint) Clone() const override
    {
        return new SketchConstraintRadius(Segment(0), Radius());
    }

    static bool CanCreate(const Handle(Sketch)& sketch,
                          const std::vector<int>& points,
                          const std::vector<int>& segments)
    {
        return points.empty()
            && !segments.empty()
            && (SketchConstraintHelper::AllSegmentsOfType<SketchSegmentCircle>(sketch, segments)
                || SketchConstraintHelper::AllSegmentsOfType<SketchSegmentArc>(sketch, segments))
            && !SketchConstraintHelper::AnyConstrainedSegment<SketchConstraintFixed>(sketch, segments)
            && !SketchConstraintHelper::AnyConstrainedSegment<SketchConstraintRadius>(sketch, segments);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;

        for(int segmentIndex : segments)
        {
            auto it = sketch->Segments().find(segmentIndex);
            if(it == sketch->Segments().end())
                continue;

            Handle(SketchSegment) segment = it->second;
            double radius = 0.0;

            if(Handle(SketchSegmentCircle) circle = Handle(SketchSegmentCircle)::DownCast(segment))
            {
                radius = circle->Radius(sketch->Points());
            }
            else if(Handle(SketchSegmentArc) arc = Handle(SketchSegmentArc)::DownCast(segment))
            {
                radius = arc->Radius(sketch->Points());
            }
            else
            {
                continue;
            }

            if(radius > 0.0)
            {
                list.push_back(new SketchConstraintRadius(segmentIndex, radius));
            }
        }

        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintRadius, SketchConstraint)

#endif // SketchConstraintRadius_HXX
