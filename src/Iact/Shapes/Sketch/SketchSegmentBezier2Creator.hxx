#ifndef SketchSegmentBezier2Creator_Header_File_
#define SketchSegmentBezier2Creator_Header_File_

#include <map>
#include <vector>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TopoDS_Wire.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintLine.hxx"

class SketchSegmentBezier2Creator : public SketchSegmentCreator
{
public:
    //! Constructor
    SketchSegmentBezier2Creator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Coord2DHudElement(nullptr)
        , _ValueHudElement(nullptr)
        , _PointsFinished(0)
		, _Segment(nullptr)
		, _HintLine(nullptr)
    {
        _MergePointIndices.assign(3, -1);
    }

    //! Destructor
    virtual ~SketchSegmentBezier2Creator() = default;

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

        SetHintMessage("Select start point for bezier curve.");

        return true;
    }

    virtual void cleanup() override
    {
        if(!_Segment.IsNull())
            _Segment->ForgetAllAttributes();
		if(!_Element.IsNull())
			_Element->ForgetAllAttributes();

        SketchSegmentCreator::cleanup();
    }

    virtual void OnStop() override
    {
        SketchSegmentCreator::OnStop();
        Stoped();
    }

    virtual bool Continue(int continueWithPoint) override
    {
        SketchSegmentCreator::Continue(continueWithPoint);

        _Points[0] = _SketchEditorTool->GetSketch()->Point(continueWithPoint);
        _MergePointIndices[0] = continueWithPoint;

        _Segment.Nullify();
        _Element.Nullify();
        _PointAction->Reset();
        _PointsFinished = 1;
        SetHintMessage("Select end point for bezier curve.");
        return true;
    }

private:
    void _PointAction_Preview(const Handle(SketchPointAction)& sender)
    {
        auto currentPoint = sender->Point();

        switch(_PointsFinished)
        {
        case 1:
            _Points[2] = currentPoint;

            if(_HintLine.IsNull())
            {
                _HintLine = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine);
            }

            _HintLine->Set(_Points[0], _Points[2], _SketchEditorTool->GetSketch()->Plane());

            if(_ValueHudElement == nullptr)
            {
                _ValueHudElement = new ValueHudElement();
                _ValueHudElement->SetLabel("Distance:");
                Add(_ValueHudElement);
            }
            _ValueHudElement->SetValue(_Points[0].Distance(currentPoint));
            break;

        case 2:
            _Points[1] = currentPoint;

            _Element->OnPointsChanged(_Points, {});
            break;
        }

        _Coord2DHudElement->setValues(currentPoint.X(), currentPoint.Y());
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        switch(_PointsFinished)
        {
        case 0:
            _Points[0] = sender->Point();
            _MergePointIndices[0] = sender->MergeCandidateIndex();
            SetHintMessage("Select end point for b¨¦zier curve.");
            _PointAction->Reset();
            _PointsFinished++;
            break;

        case 1:
            _SetEndPoint(sender->Point(), sender->MergeCandidateIndex());
            break;

        case 2:
            _Points[1] = sender->Point();
            _MergePointIndices[1] = sender->MergeCandidateIndex();
            _PointsFinished++;

            _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment},
                                                     _MergePointIndices[2] >= 0 ? -1 : 2);
            break;
        }
    }

    void _SetEndPoint(const gp_Pnt2d& point, int mergeCandidateIndex)
    {
        if(_Points[0].Distance(point) < 0.001)
        {
            _PointAction->Reset();
            return;
        }

        _Points[1] = point;
        _Points[2] = point;
        _MergePointIndices[2] = mergeCandidateIndex;

        _Segment = new SketchSegmentBezier(0, 1, 2);
        _Element = new SketchElementSegment(_Segment);
        _Element->OnPointsChanged(_Points, {});

        Remove(_HintLine);
        _HintLine.Nullify();

        if(_ValueHudElement != nullptr)
        {
            Remove(_ValueHudElement);
            _ValueHudElement = nullptr;
        }

        SetHintMessage("Select control point for bezier curve.");
        _PointAction->Reset();
        _PointsFinished++;
    }

    void _ValueHudElement_ValueEntered(double newValue)
    {
        if(newValue <= 0)
            return;

        gp_Vec2d vec(_Points[0], _Points[2]);
        if(vec.Magnitude() == 0)
            return;

        _SetEndPoint(_Points[0].Translated(vec.Normalized().Scaled(newValue)), -1);
    }

public:
    static Signal<void()> Stoped;

private:
    std::vector<int> _MergePointIndices;
    std::map<int, gp_Pnt2d> _Points;
    Handle(SketchSegmentBezier) _Segment;
    Handle(SketchElementSegment) _Element;
    Handle(SketchPointAction) _PointAction;
    Handle(HintLine) _HintLine;
    Coord2DHudElement* _Coord2DHudElement;
    ValueHudElement* _ValueHudElement;    
    int _PointsFinished;
};

#endif // _SketchSegmentBezier2Creator_Header_File_
