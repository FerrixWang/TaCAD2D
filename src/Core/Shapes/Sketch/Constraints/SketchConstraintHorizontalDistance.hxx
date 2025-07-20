#ifndef SketchConstraintHorizontalDistance_HXX
#define SketchConstraintHorizontalDistance_HXX

#include <map>
#include <memory>
#include <vector>

#include <TDataStd_Real.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintHorizontalDistance : public SketchConstraint
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Real, Distance)

public:
    SketchConstraintHorizontalDistance(int point, double distance,
                                       const TDF_Label& label = DocumentTool::SketchConstraintHorizontalDistancesLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        TDataStd_Name::Set(label, "SketchConstraintHorizontalDistance");

        SetPoints({point});
        SetParameter(distance);
    }

    SketchConstraintHorizontalDistance(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    virtual double GetParameter() const { return DistanceAttribute()->Get(); }
    virtual void SetParameter(double distance) { DistanceAttribute()->Set(_Label, distance); }

    TDF_Label Label() const { return _Label; }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        Constraint con;
        con.type = ConstraintType::PointHorizontalDistance;

        bool valid = true;
        valid &= solver->SetPoint(con.point1, Point(0), false);
        valid &= solver->SetParameter(con.parameter, DistanceAttribute()->Get(), true);

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
        return !points.empty()
            && segments.empty()
            && true;
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;

        for(const auto& pointIndex : points)
        {
            auto it = sketch->Points().find(pointIndex);
            if(it != sketch->Points().end())
            {
                list.push_back(new SketchConstraintHorizontalDistance(pointIndex, it->second.X()));
            }
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintHorizontalDistance, SketchConstraint)

#endif // SketchConstraintHorizontalDistance_HXX
