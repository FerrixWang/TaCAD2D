#ifndef _Math_Header_File_
#define _Math_Header_File_

#include <cmath>
#include <cassert>

namespace Math
{
// Mathematical constants
constexpr double DoublePI = M_PI * 2.0;
constexpr double HalfPI = M_PI / 2.0;
constexpr double PI = M_PI;
constexpr double DegRadMultiplier = 180.0 / M_PI;
static int Precision = 3;

// Angle normalization
inline double NormalizeAngleDegree(double value)
{
    if(value >= 360.0)
        return std::fmod(value, 360.0);
    if(value < 0.0)
    {
        value = std::fmod(value, 360.0);
        if(value < 0.0)
            value += 360.0;
    }
    return value;
}
inline double NormalizeAngleDegree(double value, double min, double max)
{
    assert(max - min >= 360.0);
    while(value >= max)
        value -= 360.0;
    while(value < min)
        value += 360.0;
    return value;
}
inline double NormalizeAngleRad(double value)
{
    if(value >= DoublePI)
        return std::fmod(value, DoublePI);
    if(value < 0.0)
    {
        value = std::fmod(value, DoublePI);
        if(value < 0.0)
            value += DoublePI;
    }
    return value;
}
inline double NormalizeAngleRad(double value, double min, double max)
{
    assert(max - min >= DoublePI);
    while(value >= max)
        value -= DoublePI;
    while(value < min)
        value += DoublePI;
    return value;
}

// Rounding
inline double RoundToNearest(double value, double divider)
{
    return std::round(value / divider) * divider;
}

}

#endif // _Math_Header_File_
