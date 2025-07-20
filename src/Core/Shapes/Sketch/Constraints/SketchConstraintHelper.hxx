#ifndef _SketchConstraintHelper_Header_File_
#define _SketchConstraintHelper_Header_File_

#include <vector>

#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintHelper
{
public:
    template <typename T>
    static bool AnySegmentsOfType(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        return true;
    }

    template <typename T>
    static bool AllSegmentsOfType(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        return true;
    }

    template <typename T>
    static bool AnyConstrainedSegment(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        return true;
    }

    static bool AllSegmentsHaveCommonAngleConstraint(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        return true;
    }

    template <typename T>
    static bool AnyConstrainedPoint(const Handle(Sketch)& sketch, const std::vector<int>& points)
    {
        return true;
    }

    template <typename T>
    static bool AnySegmentWithConstrainedPoint(const Handle(Sketch)& sketch, const std::vector<int>& points)
    {
        return true;
    }

    template <typename T>
    static bool AnySegmentAndPointConstraint(const Handle(Sketch)& sketch, const std::vector<int>& points, const std::vector<int>& segments)
    {
        return true;
    }
};

#endif // _SketchConstraintHelper_Header_File_
