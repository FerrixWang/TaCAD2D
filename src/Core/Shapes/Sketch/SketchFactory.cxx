
#include "Core/Shapes/Sketch/SketchFactory.hxx"

#include "Core/Shapes/Sketch/Constraints/SketchConstraintAngle.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintConcentric.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintEqual.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHorizontal.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHorizontalDistance.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintLength.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintParallel.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPerpendicular.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPointOnMidpoint.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPointOnSegment.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintRadius.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintSmoothCorner.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintTangent.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintVertical.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintVerticalDistance.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementPoint.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircleArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipse.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipticalArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

std::map<std::string, SketchFactory::Creator> SketchFactory::_Wrappers;

Handle(Standard_Transient) SketchFactory::Wrap(const TDF_Label& label)
{
	return Wrap(DocumentTool::GetName(label), label);
}

void SketchFactory::Init()
{
    // Register sketch segments
    Register<SketchSegmentArc>("SketchSegmentArc");
    Register<SketchSegmentBezier>("SketchSegmentBezier");
    Register<SketchSegmentCircle>("SketchSegmentCircle");
    Register<SketchSegmentCircleArc>("SketchSegmentCircleArc");
    Register<SketchSegmentEllipse>("SketchSegmentEllipse");
    Register<SketchSegmentEllipticalArc>("SketchSegmentEllipticalArc");
    Register<SketchSegmentLine>("SketchSegmentLine");

    // Register sketch constraints
    Register<SketchConstraintAngle>("SketchConstraintAngle");
    Register<SketchConstraintConcentric>("SketchConstraintConcentric");
    Register<SketchConstraintEqual>("SketchConstraintEqual");
    Register<SketchConstraintFixed>("SketchConstraintFixed");
    Register<SketchConstraintHorizontal>("SketchConstraintHorizontal");
    Register<SketchConstraintHorizontalDistance>("SketchConstraintHorizontalDistance");
    Register<SketchConstraintLength>("SketchConstraintLineLength");
    Register<SketchConstraintParallel>("SketchConstraintParallel");
    Register<SketchConstraintPerpendicular>("SketchConstraintPerpendicular");
    Register<SketchConstraintPointOnMidpoint>("SketchConstraintPointOnMidpoint");
    Register<SketchConstraintPointOnSegment>("SketchConstraintPointOnSegment");
    Register<SketchConstraintRadius>("SketchConstraintRadius");
    Register<SketchConstraintSmoothCorner>("SketchConstraintSmoothCorner");
    Register<SketchConstraintTangent>("SketchConstraintTangent");
    Register<SketchConstraintVertical>("SketchConstraintVertical");
    Register<SketchConstraintVerticalDistance>("SketchConstraintVerticalDistance");

    // Register sketch elements
    Register<SketchElementPoint>("SketchElementPoint");
    Register<SketchElementSegment>("SketchElementSegment");
}

// 创建对象

Handle(Standard_Transient) SketchFactory::Wrap(const std::string& name, const TDF_Label& label)
{
    auto it = _Wrappers.find(name);
    if(it == _Wrappers.end())
    {
        throw std::runtime_error("UnRegisterred type: " + name);
    }
    return it->second(label);
}
