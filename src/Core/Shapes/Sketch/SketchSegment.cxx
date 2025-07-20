#include "Core/Shapes/Sketch/SketchSegment.hxx"

#include "Core/Shapes/Sketch/SketchFactory.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

Handle(SketchSegment) SketchSegment::Clone(const TDF_Label& label) const
{
	DocumentTool::CopyLabel(InstanceLabel(), label);
	return SketchFactory::Wrap<SketchSegment>(label);
}
