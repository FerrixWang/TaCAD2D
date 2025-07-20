//! class SketchSolver
//! 

#ifndef SKETCH_SOLVER_HXX
#define SKETCH_SOLVER_HXX

#include <memory>
#include <vector>

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

// sketchSolver include
#include "Comm/SketchSolve.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"

using namespace sketchsolve;

class SketchConstraintSolver : public Standard_Transient
{
public:
    SketchConstraintSolver()
        : _MaxPointKey(0)
    {}

    static bool Solve(const Handle(Sketch)& sketch, 
                      const std::vector<int>& fixedPoints = {})
    {
        auto solver = MakeHandle<SketchConstraintSolver>();

        std::map<int, gp_Pnt2d> points = sketch->Points();
        bool success = solver->_Solve(points, sketch->Segments(), sketch->Constraints(), fixedPoints, true) == SolveResult::Success;
        sketch->AddPoints(points);
        return success;
    }

    static bool Solve(const Handle(Sketch)& sketch, 
                      std::map<int, gp_Pnt2d>& tempPoints, 
                      const std::vector<int>& fixedPoints = {})
    {
        auto solver = std::make_unique<SketchConstraintSolver>();
        return solver->_Solve(tempPoints, sketch->Segments(), {}, fixedPoints, false) == SolveResult::Success;
    }

private:
    std::map<int, int> _PointMap;
    int _MaxPointKey;
    std::vector<Constraint> _Constraints;
    std::map<int, gp_Pnt2d> _Points;
    std::vector<std::shared_ptr<Parameter>> _Parameters;

    SolveResult _Solve(std::map<int, gp_Pnt2d>& points, 
                       const std::map<int, Handle(SketchSegment)>& segments,
                       const std::vector<Handle(SketchConstraint)>& constraints,
                       const std::vector<int>& fixedPoints, 
                       bool precise)
    {
        if(points.empty() || constraints.empty())
            return SolveResult::Success;

        _Parameters.clear();
        _PointMap.clear();
        _Constraints.clear();
        _Points = points;
        _MaxPointKey = _Points.rbegin()->first;

        for(int i = 0; i < constraints.size(); i++)
        {
            if(!constraints[i]->MakeConstraint(points, segments, this))
            {
				Message::SendWarning() 
                    << "The constraint " 
                    << i 
                    << " of type " 
                    << constraints[i]->get_type_name()
					<< " has invalid parameters. It will be ignored.";
            }
        }

        // Fix points
        for(const auto& fixedPoint : fixedPoints)
        {
            FixPoint(fixedPoint);
        }

        // Solve
        auto result = sketchsolve::Solver::Solve(_Parameters, _Constraints, precise);

        // Update points on success
        if(result == SolveResult::Success)
        {
            for(size_t i = 0; i < _Parameters.size(); i++)
            {
                auto& param = _Parameters[i];
                if(param->pointKey < 0)
                    continue;

                if(param->usage == Usage::Variable)
                {
                    points[param->pointKey] = gp_Pnt2d(param->value, _Parameters[i + 1]->value);
                }
                i++;
            }
        }

        return result;
    }

    int _GetPointIndex(int pointKey, bool isConstant)
    {
        if(_PointMap.find(pointKey) != _PointMap.end())
        {
            auto index = _PointMap[pointKey];
            if(!isConstant)
            {
                if(_Parameters[index]->usage == Usage::Constant)
                    _Parameters[index]->usage = Usage::Variable;
                if(_Parameters[index + 1]->usage == Usage::Constant)
                    _Parameters[index + 1]->usage = Usage::Variable;
            }
            return index;
        }

        auto it = _Points.find(pointKey);
        if(it == _Points.end())
            return -1;

        auto index = static_cast<int>(_Parameters.size());
        _PointMap[pointKey] = index;
        _Parameters.push_back(std::make_shared<Parameter>(Parameter{
            it->second.X(), isConstant ? Usage::Constant : Usage::Variable, pointKey}));
        _Parameters.push_back(std::make_shared<Parameter>(Parameter{
            it->second.Y(), isConstant ? Usage::Constant : Usage::Variable, pointKey}));
        return index;
    }

public:
    bool SetPoint(Point& point, int pointKey, bool isConstant)
    {
        if(pointKey < 0)
            return false;

        auto index = _GetPointIndex(pointKey, isConstant);
        if(index < 0)
            return false;

        point.x = index;
        point.y = index + 1;
        return true;
    }

    bool SetLine(Line& line, int startPointKey, int endPointKey, bool isStartConstant, bool isEndConstant)
    {
        if(startPointKey < 0 || endPointKey < 0)
            return false;

        auto startIndex = _GetPointIndex(startPointKey, isStartConstant);
        auto endIndex = _GetPointIndex(endPointKey, isEndConstant);
        if(startIndex < 0 || endIndex < 0)
            return false;

        line.p1.x = startIndex;
        line.p1.y = startIndex + 1;
        line.p2.x = endIndex;
        line.p2.y = endIndex + 1;
        return true;
    }

    bool SetCircle(Circle& circle, int centerPointKey, double radius, bool isCenterConstant, bool isRadiusConstant)
    {
        if(centerPointKey < 0 || radius <= 0)
            return false;

        auto centerIndex = _GetPointIndex(centerPointKey, isCenterConstant);
        if(centerIndex < 0)
            return false;

        circle.center.x = centerIndex;
        circle.center.y = centerIndex + 1;
        SetParameter(circle.radius, radius, isRadiusConstant);
        return true;
    }

    bool SetCircle(Circle& circle, 
                   const Handle(SketchSegment)& segment, 
                   std::vector<Constraint>& constraints,
                   const std::map<int, gp_Pnt2d>& points, 
                   bool isCenterConstant, 
                   bool isRadiusConstant)
    {
        bool valid = true;
        if(Handle(SketchSegmentCircle) circSeg = Handle(SketchSegmentCircle)::DownCast(segment))
        {
            valid &= SetCircle(circle, circSeg->CenterPoint(), circSeg->Radius(points), isCenterConstant, isRadiusConstant);

            Constraint rimCon;
            rimCon.type = ConstraintType::PointOnCircle;
            rimCon.circle1 = circle;
            valid &= SetPoint(rimCon.point1, circSeg->RimPoint(), false);
            constraints.push_back(rimCon);
        }
        else if(Handle(SketchSegmentArc) arcSeg = Handle(SketchSegmentArc)::DownCast(segment))
        {
            int centerKey = 0;
            valid &= AddAuxiliaryPoint(centerKey, arcSeg->Center(points), false);
            valid &= SetCircle(circle, centerKey, arcSeg->Radius(points), isCenterConstant, isRadiusConstant);

            for(int pointIndex = 0; pointIndex < 3; pointIndex++)
            {
                Constraint conPoint;
                conPoint.type = ConstraintType::PointOnCircle;
                conPoint.circle1 = circle;
                valid &= SetPoint(conPoint.point1, arcSeg->PointsAttribute()->Value(pointIndex), false);
                constraints.push_back(conPoint);
            }
        }
        else
        {
            valid = false;
        }
        return valid;
    }

    bool SetParameter(int& parameter, double value, bool isConstant)
    {
        parameter = static_cast<int>(_Parameters.size());
        _Parameters.push_back(std::make_shared<Parameter>(Parameter{
            value, isConstant ? Usage::Constant : Usage::Variable, -1}));
        return true;
    }

    bool AddAuxiliaryPoint(int& pointKey, const gp_Pnt2d& point, bool isConstant)
    {
        _MaxPointKey++;
        pointKey = _MaxPointKey;
        _PointMap[pointKey] = _Parameters.size();
        _Parameters.push_back(std::make_shared<Parameter>(Parameter{
            point.X(), isConstant ? Usage::Constant : Usage::Variable, pointKey}));
        _Parameters.push_back(std::make_shared<Parameter>(Parameter{
            point.Y(), isConstant ? Usage::Constant : Usage::Variable, pointKey}));
        return true;
    }

    void AddConstraint(const Constraint& constraint)
    {
        _Constraints.push_back(constraint);
    }

    void FixPoint(int pointKey)
    {
        auto index = _GetPointIndex(pointKey, true);
        if(index >= 0)
        {
            _Parameters[index]->usage = Usage::Immutable;
            _Parameters[index + 1]->usage = Usage::Immutable;
        }
    }
};

DEFINE_STANDARD_HANDLE(SketchConstraintSolver, Standard_Transient)

#endif  // SKETCH_SOLVER_HXX
