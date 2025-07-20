#ifndef SketchSegmentCircleCreator_Header_File_
#define SketchSegmentCircleCreator_Header_File_

#include <map>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/Marker.hxx"

class SketchSegmentCircleCreator : public SketchSegmentCreator
{
public:
    //! Constructor
    SketchSegmentCircleCreator()
        : SketchSegmentCreator()
        , _Segment(nullptr)
        , _PointAction(nullptr)
        , _Coord2DHudElement(nullptr)
        , _ValueHudElement(nullptr)
        , _Marker(nullptr)
    {}

    //! Destructor
    virtual ~SketchSegmentCircleCreator() = default;

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
        SetHintMessage("Select center point of the circle.");

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
        if(_Marker != nullptr) _Marker->Remove();
        SketchSegmentCreator::cleanup();
    }

private:
    void _PointAction_Preview(const Handle(SketchPointAction)& sender)
    {
        auto point = sender->Point();

        if(!_Segment.IsNull())
        {
            _Points[1] = point;
            _Element->OnPointsChanged(_Points, {});

            if(_ValueHudElement == nullptr)
            {
                _ValueHudElement = new ValueHudElement();
                _ValueHudElement->SetLabel("Radius:");
                Add(_ValueHudElement);
            }
            _ValueHudElement->SetValue(_Segment->Radius(_Points));
        }

        _Coord2DHudElement->setValues(point.X(), point.Y());
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        if(_Segment.IsNull())
        {
            _Points.emplace(0, sender->Point());
            _MergePointIndices[0] = sender->MergeCandidateIndex();
            _Points.emplace(1, sender->Point());

            _Segment = new SketchSegmentCircle(0, 1);
            _Element = new SketchElementSegment(_Segment);
            _Element->OnPointsChanged(_Points, {});

            _Marker = new Marker(MarkerStyle::Bitmap, Marker::BallImage());
            _Marker->SetWorkspaceController(m_workspaceController);

            Add(_Marker);
            _Marker->Set(_Points[0], _SketchEditorTool->GetSketch()->Plane());

            SetHintMessage("Select any rim point of the circle.");
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
            _MergePointIndices[1] = sender->MergeCandidateIndex();

            StopAction(_PointAction);
            _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, _MergePointIndices[1] >= 0 ? -1 : 1);
        }
    }

    void _ValueHudElement_ValueEntered(double newValue)
    {
        if(newValue <= 0)
            return;

        gp_Vec2d vec(_Points[0], _Points[1]);
        if(vec.Magnitude() == 0)
            return;

        _Points[1] = _Points[0].Translated(vec.Normalized().Scaled(newValue));
        _MergePointIndices[1] = -1;

        StopAction(_PointAction);
        _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, 1);
    }

public:
    static Signal<void()> Stoped;

private:    
    std::vector<int> _MergePointIndices = {-1, -1};
    std::map<int, gp_Pnt2d> _Points;
    Handle(SketchSegmentCircle) _Segment;
    Handle(SketchElementSegment) _Element;
    Handle(SketchPointAction) _PointAction;
    Coord2DHudElement* _Coord2DHudElement;
    ValueHudElement* _ValueHudElement;
    Marker* _Marker;
};

#endif // _SketchSegmentCircleCreator_Header_File_