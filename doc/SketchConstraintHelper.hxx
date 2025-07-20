#ifndef _SketchConstraintHelper_Header_File_
#define _SketchConstraintHelper_Header_File_

#include <vector>
#include <algorithm>

#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPerpendicular.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintParallel.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintAngle.hxx"

class SketchConstraintHelper
{
public:
    template <typename T>
    static bool AnySegmentsOfType(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        for(const auto& segIndex : segments)
        {
            auto it = sketch->Segments().find(segIndex);
            if(it != sketch->Segments().end() && it->second->IsKind(STANDARD_TYPE(T)))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T>
    static bool AllSegmentsOfType(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        for(const auto& segIndex : segments)
        {
            auto it = sketch->Segments().find(segIndex);
            if(it == sketch->Segments().end() || !it->second->IsKind(STANDARD_TYPE(T)))
            {
                return false;
            }
        }
        return true;
    }

    template <typename T>
    static bool AnyConstrainedSegment(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        for(const auto& constraint : sketch->Constraints())
        {
            if(constraint->IsKind(STANDARD_TYPE(T)))
            {
                const auto& conSegments = constraint->Segments();
                for(const auto& segIndex : segments)
                {
                    if(std::find(conSegments.begin(), conSegments.end(), segIndex) != conSegments.end())
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    static bool AllSegmentsHaveCommonAngleConstraint(const Handle(Sketch)& sketch, const std::vector<int>& segments)
    {
        for(const auto& constraint : sketch->Constraints())
        {
            if(constraint->IsKind(STANDARD_TYPE(SketchConstraintPerpendicular)) ||
               constraint->IsKind(STANDARD_TYPE(SketchConstraintParallel)) ||
               constraint->IsKind(STANDARD_TYPE(SketchConstraintAngle)))
            {
                const auto& conSegments = constraint->Segments();
                if(std::includes(conSegments.begin(), conSegments.end(), segments.begin(), segments.end()))
                {
                    return true;
                }
            }
        }
        return false;
    }

    template <typename T>
    static bool AnyConstrainedPoint(const Handle(Sketch)& sketch, const std::vector<int>& points)
    {
        for(const auto& constraint : sketch->Constraints())
        {
            if(constraint->IsKind(STANDARD_TYPE(T)))
            {
                const auto& conPoints = constraint->Points();
                for(const auto& pointIndex : points)
                {
                    if(std::find(conPoints.begin(), conPoints.end(), pointIndex) != conPoints.end())
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    template <typename T>
    static bool AnySegmentWithConstrainedPoint(const Handle(Sketch)& sketch, const std::vector<int>& points)
    {
        for(const auto& constraint : sketch->Constraints())
        {
            if(constraint->IsKind(STANDARD_TYPE(T)))
            {
                const auto& conSegments = constraint->Segments();
                for(const auto& segIndex : conSegments)
                {
                    auto it = sketch->Segments().find(segIndex);
                    if(it != sketch->Segments().end())
                    {
                        const auto& segPoints = it->second->Points();
                        for(const auto& pointIndex : points)
                        {
                            if(std::find(segPoints.begin(), segPoints.end(), pointIndex) != segPoints.end())
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    template <typename T>
    static bool AnySegmentAndPointConstraint(const Handle(Sketch)& sketch, const std::vector<int>& points, const std::vector<int>& segments)
    {
        for(const auto& constraint : sketch->Constraints())
        {
            if(constraint->IsKind(STANDARD_TYPE(T)))
            {
                const auto& conSegments = constraint->Segments();
                const auto& conPoints = constraint->Points();
                bool hasSegment = false;
                bool hasPoint = false;

                for(const auto& segIndex : segments)
                {
                    if(std::find(conSegments.begin(), conSegments.end(), segIndex) != conSegments.end())
                    {
                        hasSegment = true;
                        break;
                    }
                }

                for(const auto& pointIndex : points)
                {
                    if(std::find(conPoints.begin(), conPoints.end(), pointIndex) != conPoints.end())
                    {
                        hasPoint = true;
                        break;
                    }
                }

                if(hasSegment && hasPoint)
                {
                    return true;
                }
            }
        }
        return false;
    }
};

#endif // _SketchConstraintHelper_Header_File_
