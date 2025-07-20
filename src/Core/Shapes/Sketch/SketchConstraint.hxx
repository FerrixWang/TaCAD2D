//! class SketchConstraint
//! 
//! 

#ifndef SketchConstraint_HXX_
#define SketchConstraint_HXX_

#include <ElSLib.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipse.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipticalArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchConstraintSolver;

// Abstract base class for sketch constraints
class SketchConstraint : public Standard_Transient
{
    DEFINE_ATTRIBUTE_ON_CHILD(0, TDataXtd_Shape, Shape)
    DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_IntegerArray, Points)
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_IntegerArray, Segments)

public:
    // Constructor and destructor
    SketchConstraint(const TDF_Label& label)
        : myLabel(label)
    {
        Handle(TDataXtd_Shape) shapeAttr;
        if(!ShapeLabel().FindAttribute(TDataXtd_Shape::GetID(), shapeAttr))
        {
            TDataXtd_Shape::Set(ShapeLabel(), {});
        }
        Handle(TDataStd_IntegerArray) arrayAttr;
        if(!PointsLabel().FindAttribute(TDataStd_IntegerArray::GetID(), arrayAttr))
        {
            PointsAttribute() = TDataStd_IntegerArray::Set(PointsLabel(), 0, 0);
        }
        Handle(TDataStd_IntegerArray) auxAttr;
        if(!SegmentsLabel().FindAttribute(TDataStd_IntegerArray::GetID(), auxAttr))
        {
            SegmentsAttribute() = TDataStd_IntegerArray::Set(SegmentsLabel(), 0, 0);
        }
    };

    virtual ~SketchConstraint() = default;

    virtual TDF_Label InstanceLabel() const { return myLabel.Father(); }

    virtual Handle(SketchConstraint) Clone() const
    {
        return nullptr;
    }

    TDF_Label Label() const { return myLabel; }

    int Point(int index) const
    {
        return PointsAttribute()->Value(index);
    }

    int Segment(int index) const
    {
        return SegmentsAttribute()->Value(index);
    }

    const std::vector<int>& Points() const
    {
        std::vector<int> points;
        for(int a : PointsAttribute()->Array()->Array1())
        {
            points.push_back(a);
        }
        return points;
    }

    const std::vector<int>& Segments() const
    {
        std::vector<int> segments;
        for(int a : SegmentsAttribute()->Array()->Array1())
        {
            segments.push_back(a);
        }
        return segments;
    }

    int PointsCount() const
    {
		return PointsAttribute()->Array()->Size();
    }

    int SegmentsCount() const
    {
        return SegmentsAttribute()->Array()->Size();
    }

    void SetPoints(const std::vector<int>& points)
    {
        PointsAttribute()->Init(0, points.size() - 1);
        for(int i = 0; i < points.size(); ++i)
        {
            PointsAttribute()->SetValue(i, points[i]);
        }
    }

    void SetSegments(const std::vector<int>& points)
    {
        SegmentsAttribute()->Init(0, points.size() - 1);
        for(int i = 0; i < points.size(); ++i)
        {
            SegmentsAttribute()->SetValue(i, points[i]);
        }
    }

    void SetPoint(int index, int value)
    {
        PointsAttribute()->SetValue(index, value);
    }

    void SetSegment(int index, int value)
    {
         SegmentsAttribute()->SetValue(index, value);
    }

    // Parameter property (virtual get/set)
    virtual double GetParameter() const { return 0.0; }
    virtual void SetParameter(double /*value*/) {}

    // Pure virtual methods
    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) = 0;

protected:
    bool ComputeCirclePoints(const std::map<int, gp_Pnt2d>& points,
                             const Handle(SketchSegment)& segment,
                             const gp_Pln& plane,
                             gp_Pnt& center,
                             gp_Pnt& rim,
                             double& radius)
    {
        center = gp_Pnt(0, 0, 0);
        rim = gp_Pnt(0, 0, 0);
        radius = 0.0;

        gp_Pnt2d centerPoint2d;
        gp_Pnt2d rimPoint2d;
        bool valid = false;

        if(segment->IsKind(STANDARD_TYPE(SketchSegmentCircle)))
        {
            auto circleSegment = Handle(SketchSegmentCircle)::DownCast(segment);
            auto it = points.find(circleSegment->CenterPoint());
            if(it != points.end())
            {
                centerPoint2d = it->second;
                it = points.find(circleSegment->RimPoint());
                if(it != points.end())
                {
                    rimPoint2d = it->second;
                    radius = circleSegment->Radius(points);
                    valid = true;
                }
            }
        }
        else if(segment->IsKind(STANDARD_TYPE(SketchSegmentArc)))
        {
            auto arcSegment = Handle(SketchSegmentArc)::DownCast(segment);
            centerPoint2d = arcSegment->Center(points);
            auto it = points.find(arcSegment->RimPoint());
            if(it != points.end())
            {
                rimPoint2d = it->second;
                radius = arcSegment->Radius(points);
                valid = true;
            }
        }

        if(!valid)
            return false;

        center = ElSLib::Value(centerPoint2d.X(), centerPoint2d.Y(), plane);
        rim = ElSLib::Value(rimPoint2d.X(), rimPoint2d.Y(), plane);
        return true;
    }
private:
    TDF_Label myLabel;
};

DEFINE_STANDARD_HANDLE(SketchConstraint, Standard_Transient)
#endif  // SketchConstraint_HXX_
