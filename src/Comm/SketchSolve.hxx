
#ifndef _Sketchsolve_Header_File_
#define _Sketchsolve_Header_File_

#include <memory>
#include <vector>

#include "sketchSolve/solve.h"

namespace Sketchsolve
{

enum class ConstraintType
{
    PointOnPoint = constraintType::pointOnPoint,
    PointToLine = constraintType::pointToLine,
    PointOnLine = constraintType::pointOnLine,
    Horizontal = constraintType::horizontal,
    Vertical = constraintType::vertical,
    InternalAngle = constraintType::internalAngle,
    RadiusValue = constraintType::radiusValue,
    TangentToArc = constraintType::tangentToArc,
    TangentToArcStart = constraintType::tangentToArcStart,
    TangentToArcEnd = constraintType::tangentToArcEnd,
    TangentToCircle = constraintType::tangentToCircle,
    ArcRules = constraintType::arcRules,
    PointToPointDistance = constraintType::P2PDistance,
    PointToPointDistanceVert = constraintType::P2PDistanceVert,
    PointToPointDistanceHorz = constraintType::P2PDistanceHorz,
    PointToLineDistance = constraintType::P2LDistance,
    PointToLineDistanceVert = constraintType::P2LDistanceVert,
    PointToLineDistanceHorz = constraintType::P2LDistanceHorz,
    LineLength = constraintType::lineLength,
    EqualLength = constraintType::equalLength,
    ArcRadius = constraintType::arcRadius,
    EqualRadiusArcs = constraintType::equalRadiusArcs,
    EqualRadiusCircles = constraintType::equalRadiusCircles,
    EqualRadiusCircArc = constraintType::equalRadiusCircArc,
    ConcentricArcs = constraintType::concentricArcs,
    ConcentricCircles = constraintType::concentricCircles,
    ConcentricCircArc = constraintType::concentricCircArc,
    CircleRadius = constraintType::circleRadius,
    ExternalAngle = constraintType::externalAngle,
    Parallel = constraintType::parallel,
    Perpendicular = constraintType::perpendicular,
    Colinear = constraintType::colinear,
    PointOnCircle = constraintType::pointOnCircle,
    PointOnArc = constraintType::pointOnArc,
    PointOnLineMidpoint = constraintType::pointOnLineMidpoint,
    PointOnArcMidpoint = constraintType::pointOnArcMidpoint,
    PointOnCircleQuad = constraintType::pointOnCircleQuad,
    SymmetricPoints = constraintType::symmetricPoints,
    SymmetricLines = constraintType::symmetricLines,
    SymmetricCircles = constraintType::symmetricCircles,
    SymmetricArcs = constraintType::symmetricArcs,
    PointOnArcStart = constraintType::pointOnArcStart,
    PointOnArcEnd = constraintType::pointOnArcEnd,
    ArcStartToArcEnd = constraintType::arcStartToArcEnd,
    ArcStartToArcStart = constraintType::arcStartToArcStart,
    ArcEndToArcEnd = constraintType::arcEndToArcEnd,
    ArcTangentToArc = constraintType::arcTangentToArc,
    CircleTangentToCircle = constraintType::circleTangentToCircle,
    CircleTangentToArc = constraintType::circleTangentToArc,
    TangentToEllipse = constraintType::tangentToEllipse,
    PointHorizontalDistance = constraintType::pointHorizontalDistance,
    PointVerticalDistance = constraintType::pointVerticalDistance,
};

enum class Usage
{
    Variable,
    Constant,
    Immutable
};

struct Parameter
{
    double value = 0.0;
    Usage usage = Usage::Variable;
    int pointKey = -1;
    double* pointer = nullptr;

};

static void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, 
                     int i, 
                     double*& native)
{
    native = (i >= 0 && i < static_cast<int>(parameters.size()) && parameters[i]) 
           ? parameters[i]->pointer
           : nullptr;
}

struct Point
{
    int x = -1;
    int y = -1;

    void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, point& native) const
    {
        Sketchsolve::ToNative(parameters, x, native.x);
        Sketchsolve::ToNative(parameters, y, native.y);
    }
};

struct Line
{
    Point p1;
    Point p2;

    void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, line& native) const
    {
        p1.ToNative(parameters, native.p1);
        p2.ToNative(parameters, native.p2);
    }
};

struct Circle
{
    Point center;
    int radius = -1;

    void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, circle& native) const
    {
        center.ToNative(parameters, native.center);
        Sketchsolve::ToNative(parameters, radius, native.rad);
    }
};

struct Arc
{
    Point start;
    Point end;
    Point center;

    void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, arc& native) const
    {
        start.ToNative(parameters, native.start);
        end.ToNative(parameters, native.end);
        center.ToNative(parameters, native.center);
    }
};

struct Constraint
{
    ConstraintType type;
    Point point1, point2;
    Line line1, line2, symLine;
    Circle circle1, circle2;
    int parameter = -1;
    Arc arc1, arc2;

    void ToNative(const std::vector<std::shared_ptr<Parameter>>& parameters, constraint& native) const
    {
        native.type = static_cast<constraintType>(type);
        point1.ToNative(parameters, native.point1);
        point2.ToNative(parameters, native.point2);
        line1.ToNative(parameters, native.line1);
        line2.ToNative(parameters, native.line2);
        symLine.ToNative(parameters, native.SymLine);
        circle1.ToNative(parameters, native.circle1);
        circle2.ToNative(parameters, native.circle2);
        arc1.ToNative(parameters, native.arc1);
        arc2.ToNative(parameters, native.arc2);

        Sketchsolve::ToNative(parameters, parameter, native.parameter);
    }
};

enum class SolveResult
{
    Success = 0,
    NoSolution = 1
};

class Solver
{
public:

    static SolveResult Solve(std::vector<std::shared_ptr<Parameter>>& parameters,
                             const std::vector<Constraint>& constraints,
                             bool precise)
    {
        const int count = static_cast<int>(parameters.size());
        std::unique_ptr<double[]> values(new double[count]);
        int curVar = 0;
        int curConst = count - 1;

        for(auto& p : parameters)
        {
            int idx = (p->usage == Usage::Variable) ? curVar++ : curConst--;
            p->pointer = &values[idx];
            *p->pointer = p->value;
        }

        const int numVar = curVar;
        std::unique_ptr<double* []> varPtrs(new double* [numVar]);
        for(int i = 0; i < numVar; ++i)
            varPtrs[i] = &values[i];

        std::vector<constraint> nativeConstraints;
        nativeConstraints.reserve(constraints.size());
        for(const auto& c : constraints) {
            nativeConstraints.emplace_back(constraint());
			c.ToNative(parameters, nativeConstraints.back());
        }

        ::Solver nativeSolver;
        int result = nativeSolver.solve(varPtrs.get(), 
                                        numVar, 
                                        nativeConstraints.data(), 
                                        static_cast<int>(constraints.size()),
                                        precise ? fine : rough);

        if(result == succsess)
        {
            for(auto& p : parameters)
                p->value = *p->pointer;
            return SolveResult::Success;
        }

        return SolveResult::NoSolution;
    }
};

}

#endif  // _Sketchsolve_Header_File_
