#ifndef Geom2dUtils_Header_File_
#define Geom2dUtils_Header_File_

#include <vector>

#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_XY.hxx>
#include <gp_Mat2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <ElCLib.hxx>
#include <gce_MakeElips2d.hxx>
#include <Math.hxx>

class Geom2dUtils
{
public:
    //! Compute the center of an ellipse given two endpoints, radii, rotation angle, and sense
    static gp_Pnt2d FindEllipseCenterFromEndpoints(const gp_Pnt2d& startPnt, const gp_Pnt2d& endPnt,
                                                   double rX, double rY, double angle, bool sense)
    {
        // https://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes
        // F.6.5.1
        gp_Mat2d matRotation;
        matRotation.SetRotation(-angle);
        gp_XY v1((startPnt.X() - endPnt.X()) * 0.5, (startPnt.Y() - endPnt.Y()) * 0.5);
        v1.Multiply(matRotation);

        // F.6.5.2
        double denom = rX * rX * v1.Y() * v1.Y() + rY * rY * v1.X() * v1.X();
        double numer = rX * rX * rY * rY - denom;
        double root = Sqrt(Abs(numer / denom)) * (sense ? -1.0 : 1.0);
        gp_XY c1(rX * v1.Y() / rY * root, -rY * v1.X() / rX * root);

        // F.6.5.3
        matRotation.SetRotation(angle);
        gp_XY c((startPnt.X() + endPnt.X()) * 0.5, (startPnt.Y() + endPnt.Y()) * 0.5);
        c.Add(c1.Multiplied(matRotation));

        return gp_Pnt2d(c.X(), c.Y());
    }

    //! Convert an elliptical arc to a series of Bezier curves
    static std::vector<std::tuple<gp_Pnt2d, gp_Pnt2d, gp_Pnt2d, gp_Pnt2d>>
        EllipticalArcToBezier(const gp_Elips2d& ellipse, double startParam, double endParam, double maxInterval = M_PI / 3.0)
    {
        if(endParam < startParam)
            endParam += 2.0 * M_PI;

        double paramInterval = endParam - startParam;
        int itCount = static_cast<int>(std::ceil(std::abs(paramInterval) / maxInterval));
        std::vector<std::tuple<gp_Pnt2d, gp_Pnt2d, gp_Pnt2d, gp_Pnt2d>> result(itCount);
        double itParamStep = paramInterval / itCount;
        double itStartParam = startParam;

        for(int step = 0; step < itCount; ++step)
        {
            double itEndParam = itStartParam + itParamStep;

            // http://www.spaceroots.org/documents/ellipse/node22.html
            // https://mortoray.com/2017/02/16/rendering-an-svg-elliptical-arc-as-bezier-curves/
            gp_Pnt2d p1;
            gp_Vec2d v1;
            ElCLib::D1(itStartParam, ellipse, p1, v1);

            gp_Pnt2d p2;
            gp_Vec2d v2;
            ElCLib::D1(itEndParam, ellipse, p2, v2);

            double a = 3.0 * std::pow(std::tan((itEndParam - itStartParam) / 2.0), 2);
            double b = (std::sqrt(4.0 + a) - 1.0) / 3.0;
            double alpha = std::sin(itEndParam - itStartParam) * b;
            gp_Pnt2d cp1 = p1.Translated(v1 * alpha);
            gp_Pnt2d cp2 = p2.Translated(v2 * -alpha);

            result[step] = std::make_tuple(p1, p2, cp1, cp2);
            itStartParam = itEndParam;
        }

        return result;
    }

    static bool isInvalidEllipse(const gp_Elips2d& ellipse)
    {
        Standard_Real major = ellipse.MajorRadius();
        Standard_Real minor = ellipse.MinorRadius();
        // 检查默认值、负值、无效关系或过大值
        return major == RealLast() || minor == RealSmall() ||
            major < minor || minor < 0.0 ||
            major > 1e6 || minor > 1e6;
    }
    //! Create an ellipse from center, major axis point, and minor axis point
    static gp_Elips2d MakeEllipse(const gp_Pnt2d& center, gp_Pnt2d p1, gp_Pnt2d p2, bool sense = true)
    {
        try
        {
            if(p1.Distance(p2) <= 0.0 || p2.Distance(center) <= 0.0 || p1.Distance(center) <= 0.0)
                return gp_Elips2d();

            // Make sure that p1 is the end of the major axis
            if(p1.Distance(center) < p2.Distance(center))
            {
                std::swap(p1, p2);
            }

            // The radius a is defined by distance to point p1
            double a = center.Distance(p1);

            // Normalize ellipse to x-axis running through p1, center is in (0,0)
            gp_Ax2d ax(center, gp_Vec2d(center, p1));
            double rotAngle = ax.Angle(gp_Ax2d(center, gp_Dir2d(1.0, 0.0)));
            gp_Pnt2d p2Norm = p2.Translated(gp_Vec2d(center, gp_Pnt2d(0.0, 0.0))).Rotated(gp_Pnt2d(0.0, 0.0), rotAngle);

            // The ellipse formula is: x²/a² + y²/b² = 1
            double x2 = p2Norm.X() * p2Norm.X();
            double y2 = p2Norm.Y() * p2Norm.Y();
            double a2 = a * a;
            double b = std::sqrt(y2 / (1.0 - x2 / a2));

            if(std::isnan(a) || std::isnan(b))
                return gp_Elips2d();

            return gce_MakeElips2d(gp_Ax2d(center, gp_Dir2d(gp_Vec2d(center, p1))), a, b, sense).Value();
        }
        catch(...)
        {
            return gp_Elips2d();
        }
    }

    //! Create a 2D BSpline curve from control points, knots, weights, and degree
    static Handle(Geom2d_BSplineCurve) MakeBSplineCurve(int degree,
                                                        const std::vector<double>& knots,
                                                        const std::vector<gp_Pnt2d>& controlPoints,
                                                        const std::vector<double>& weights,
                                                        bool isClosed)
    {
        // Copy control points
        int poleCount = static_cast<int>(controlPoints.size());
        TColgp_Array1OfPnt2d poles(1, poleCount);
        TColStd_Array1OfReal* weightsCol = weights.empty() ? nullptr : new TColStd_Array1OfReal(1, poleCount);

        for(int i = 0; i < poleCount; ++i)
        {
            poles.SetValue(i + 1, controlPoints[i]);
            if(weightsCol)
                weightsCol->SetValue(i + 1, weights[i]);
        }

        // Count multiplicities and compact knot list
        std::vector<double> knotList;
        std::vector<int> multList;
        double lastKnot = std::numeric_limits<double>::quiet_NaN();
        for(const auto& knot : knots)
        {
            if(!IsEqual(lastKnot, knot))
            {
                knotList.push_back(knot);
                lastKnot = knot;
                multList.push_back(1);
            }
            else
            {
                multList.back()++;
            }
        }

        // Copy knots and multiplicities
        int knotCount = static_cast<int>(knotList.size());
        TColStd_Array1OfReal knotsCol(1, knotCount);
        TColStd_Array1OfInteger multsCol(1, knotCount);
        for(int i = 0; i < knotCount; ++i)
        {
            knotsCol.SetValue(i + 1, knotList[i]);
            multsCol.SetValue(i + 1, multList[i]);
        }

        // Create spline
        Handle(Geom2d_BSplineCurve) spline;
        if(weightsCol)
        {
            spline = new Geom2d_BSplineCurve(poles, *weightsCol, knotsCol, multsCol, degree, isClosed);
            delete weightsCol;
        }
        else
        {
            spline = new Geom2d_BSplineCurve(poles, knotsCol, multsCol, degree, isClosed);
        }

        return spline;
    }
};

#endif // _Geom2dUtils_Header_File_
