#include "Iact/Shapes/Sketch/SketchSegmentLineCreator.hxx"

#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"

Signal<void()> SketchSegmentLineCreator::Stoped;

bool SketchSegmentLineCreator::Continue(int continueWithPoint)
{
	SketchSegmentCreator::Continue(continueWithPoint);

	_Points[0] = _SketchEditorTool->GetSketch()->Point(continueWithPoint);
	_Points[1] = _Points[0];
	_MergePointIndices[0] = continueWithPoint;

	_Segment = new SketchSegmentLine(0, 1);
	_Element = new SketchElementSegment(_Segment);
	_Element->OnPointsChanged(_Points, {});

	_PointAction->Reset();
	return true;
}
