#ifndef SketchConstraintFixed_HXX
#define SketchConstraintFixed_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintFixed : public SketchConstraint
{
public:
    enum class TargetType
    {
        Point,
        Segment
    };

    SketchConstraintFixed(TargetType targetType, int index,
                          const TDF_Label& label = DocumentTool::SketchConstraintFixedsLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());
        TDataStd_Name::Set(label, "SketchConstraintFixed");

        switch(targetType)
        {
        case TargetType::Point:
            SetPoints({index});
            break;
        case TargetType::Segment:
            SetSegments({index});
            break;
        default:
            throw Standard_ProgramError("Invalid target type");
        }
    }

    SketchConstraintFixed(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return _Label; }

    TargetType GetTarget() const
    {
        return Segments().empty() ? TargetType::Point : TargetType::Segment;
    }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        switch(GetTarget())
        {
        case TargetType::Point:
            solver->FixPoint(Point(0));
            return true;

        case TargetType::Segment:
        {
            auto it = segments.find(Segment(0));
            if(it == segments.end())
                return false;

			auto points = it->second->PointsAttribute()->Array();
            for(const auto& point : *points)
            {
                solver->FixPoint(point);
            }
            return true;
        }
        }
        return false;
    }

    bool CanCreate(const Handle(Sketch)& sketch,
                   const std::vector<int>& points,
                   const std::vector<int>& segments)
    {
        if(!segments.empty())
        {
            return !SketchConstraintHelper::AnyConstrainedSegment<SketchConstraintFixed>(sketch, segments);
        }
        else if(!points.empty())
        {
            return true;
        }
        return false;
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;

        for(const auto& pointIndex : points)
        {
            list.push_back(new SketchConstraintFixed(TargetType::Point, pointIndex));
        }
        for(const auto& segIndex : segments)
        {
            list.push_back(new SketchConstraintFixed(TargetType::Segment, segIndex));
        }
        return list;
    }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchConstraintFixed, SketchConstraint)

#endif // SketchConstraintFixed_HXX
