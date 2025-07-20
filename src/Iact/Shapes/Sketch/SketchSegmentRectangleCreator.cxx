#include "Iact/Shapes/Sketch/SketchSegmentRectangleCreator.hxx"

#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"

Signal<void()> SketchSegmentRectangleCreator::Stoped;
