#ifndef SketchSegmentLineCreator_Header_File_
#define SketchSegmentLineCreator_Header_File_

#include <map>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"

class SketchSegmentLineCreator : public SketchSegmentCreator
{
public:
	//! Constructor
	SketchSegmentLineCreator() 
		: SketchSegmentCreator()
		, _Segment(nullptr)
		, _PointAction(nullptr)
		, _Coord2DHudElement(nullptr)
	{}
		
	//! Destructor
	virtual ~SketchSegmentLineCreator() = default;

	virtual bool OnStart() override
	{
		SketchSegmentCreator::OnStart();

		_PointAction = new SketchPointAction(_SketchEditorTool);
		if(!StartAction(_PointAction))
			return false;

		_PointAction->Preview.connect([this](const auto& thePointAction) { _PointAction_Preview(thePointAction); });
		_PointAction->Finished.connect([this](const auto& thePointAction) { _PointAction_Finished(thePointAction); });

		_Coord2DHudElement = new Coord2DHudElement();
		Add(_Coord2DHudElement);
		SetHintMessage("Select start point for line.");
		// Implementation of the start of the line segment creation
		return true;
	}

	virtual void OnStop() override
	{
		SketchSegmentCreator::OnStop();
		Stoped();
	}

	virtual void cleanup() override
	{
		if(!_Segment.IsNull()) _Segment->ForgetAllAttributes();
		if(!_Element.IsNull()) _Element->ForgetAllAttributes();
		SketchSegmentCreator::cleanup();
	}

	virtual bool Continue(int continueWithPoint) override;

private:
	void _PointAction_Preview(const Handle(SketchPointAction)& sender)
	{
		auto point = sender->Point();

		if(!_Element.IsNull())
		{
			_Points[1] = point;
			_Element->OnPointsChanged(_Points, {});
		}

		_Coord2DHudElement->setValues(point.X(), point.Y());
	}

	void _PointAction_Finished(const Handle(SketchPointAction)& sender)
	{
		if(_Segment.IsNull())
		{
			_Points.emplace(0, sender->Point());
			_Points.emplace(1, sender->Point());

			_Segment = new SketchSegmentLine(0, 1);
			_Element = new SketchElementSegment(_Segment);
			_Element->OnPointsChanged(_Points, {});

			_PointAction->Reset();
		}
		else
		{
			if(_Points[0].Distance(sender->Point()) < 0.001)
			{
				_PointAction->Reset();
				return;
			}

			_Points[1] = sender->Point();

			_SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, 1);
		}
	}
public:
	static Signal<void()> Stoped;

private:
	std::vector<int> _MergePointIndices = {-1, -1};
	std::map<int, gp_Pnt2d> _Points;
	Handle(SketchPointAction) _PointAction;
	Handle(SketchSegmentLine) _Segment;
	Handle(SketchElementSegment) _Element;
	Coord2DHudElement* _Coord2DHudElement;
	ValueHudElement* _ValueHudElement;
};

#endif  // _SketchSegmentLineCreator_Header_File_
