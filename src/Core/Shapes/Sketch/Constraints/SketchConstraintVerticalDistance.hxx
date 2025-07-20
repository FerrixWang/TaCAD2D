#ifndef SketchConstraintVerticalDistance_HXX
#define SketchConstraintVerticalDistance_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintVerticalDistance : public SketchConstraint
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Real, Distance)

public:
    SketchConstraintVerticalDistance(int point, double distance,
                                     const TDF_Label& label = DocumentTool::SketchConstraintVerticalDistancesLabel().NewChild())
        : SketchConstraint(label.FindChild(0)), _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintVerticalDistance");

        SetPoints({point});
        SetParameter(distance);
    }

    //! Constructor to load from label
    SketchConstraintVerticalDistance(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0)), _Label(label)
    {
        assert(label.HasAttribute());
    }

    // Parameter property (virtual get/set)
    virtual double GetParameter() const override { return DistanceAttribute()->Get(); }
    virtual void SetParameter(double distance) override { DistanceAttribute()->Set(_Label, distance); }

    TDF_Label Label() const { return _Label; }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        auto it1 = points.find(Points()[0]);
        if(it1 == points.end())
        {
            return false;
        }

        Constraint con;
        con.type = ConstraintType::PointVerticalDistance;

        bool valid = DistanceAttribute()->Get() > 0.0;
        valid &= solver->SetParameter(con.parameter, DistanceAttribute()->Get(), true);
        valid &= solver->SetPoint(con.point1, Point(0), false);
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
        return points.size() > 0
            && segments.size() == 0
            && !SketchConstraintHelper::AnyConstrainedPoint<SketchConstraintVerticalDistance>(sketch, points)
            && !SketchConstraintHelper::AnyConstrainedPoint<SketchConstraintFixed>(sketch, points);
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch, 
                                                        const std::vector<int>& points, 
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;
        for(size_t i = 0; i < points.size(); ++i)
        {
            list.push_back(new SketchConstraintVerticalDistance(points[i], sketch->Point(points[i]).Y()));
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintVerticalDistance, SketchConstraint)

#endif // SketchConstraintVerticalDistance_HXX
