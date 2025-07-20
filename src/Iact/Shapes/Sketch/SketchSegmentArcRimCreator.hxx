#ifndef SketchSegmentArcRimCreator_Header_File_
#define SketchSegmentArcRimCreator_Header_File_

#include <map>
#include <vector>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Geom/Geom2dUtils.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintLine.hxx"

class SketchSegmentArcRimCreator : public SketchSegmentCreator
{
public:
    //! Constructor
    SketchSegmentArcRimCreator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Coord2DHudElement(nullptr)
        , _ValueHudElement(nullptr)
        , _PointsCompleted(0)
    {
        _MergePointIndices = {-1, -1, -1};
    }

    //! Destructor
    virtual ~SketchSegmentArcRimCreator() = default;

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
        SetHintMessage("Select start point for circular arc.");

        return true;
    }

    virtual void cleanup() override
    {
        if(!_Segment.IsNull())
            _Segment->ForgetAllAttributes();
		if(_Element.IsNull())
			_Element->ForgetAllAttributes();
        if(_ValueHudElement != nullptr)
        {
            Remove(_ValueHudElement);
            _ValueHudElement = nullptr;
        }
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
        _PointsCompleted = 1;
        SetHintMessage("Select end point for circular arc.");
        return true;
    }

private:
    void _PointAction_Preview(const Handle(SketchPointAction)& sender)
    {
        auto point = sender->Point();

        if(_PointsCompleted == 1)
        {
            auto p1 = _Points[0];
            if(_HintLine.IsNull())
            {
                _HintLine = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine);
            }
			_HintLine->Set(_Points[0], point, _SketchEditorTool->GetSketch()->Plane());
        }
        else if(_PointsCompleted == 2)
        {
            if(!_Segment.IsNull())
            {
                _Points[2] = point;

                _Element->OnPointsChanged(_Points, {});

                if(_ValueHudElement == nullptr)
                {
                    _ValueHudElement = new ValueHudElement();
                    _ValueHudElement->SetLabel("Radius:");

                    Add(_ValueHudElement);
                }
                _ValueHudElement->SetValue(_Segment->Radius(_Points));
            }
        }

        _Coord2DHudElement->setValues(point.X(), point.Y());
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        auto point = sender->Point();
        int mergeCandidateIndex = sender->MergeCandidateIndex();

        if(_PointsCompleted == 0)
        {
            _Points[0] = point;
            _MergePointIndices[0] = mergeCandidateIndex;
            _PointsCompleted++;
            SetHintMessage("Select end point for circular arc.");
            _PointAction->Reset();
        }
        else if(_PointsCompleted == 1)
        {
            if(_Points[0].Distance(point) < 0.001)
            {
                _PointAction->Reset();
                return;
            }

            _Points[1] = point;
            _MergePointIndices[1] = mergeCandidateIndex;
            _PointsCompleted++;

            _Points[2] = point;
            _Segment = new SketchSegmentArc(0, 1, 2);
            _Element = new SketchElementSegment(_Segment);
            _Element->OnPointsChanged(_Points, {});

            SetHintMessage("Select rim point for circular arc.");
            _PointAction->Reset();
        }
        else if(_PointsCompleted == 2)
        {
            if(_Points[0].Distance(point) < 0.001 || _Points[1].Distance(point) < 0.001)
            {
                _PointAction->Reset();
                return;
            }

            if(_ValueHudElement != nullptr)
            {
                Remove(_ValueHudElement);
                _ValueHudElement = nullptr;
            }

            _Points[2] = point;
            _MergePointIndices[2] = mergeCandidateIndex;
            _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment},
                                                     _MergePointIndices[1] >= 0 ? -1 : 1);
        }
    }

    void _ValueHudElement_RadiusEntered(double newValue)
    {
        if(newValue == 0 || _Points[0].Distance(_Points[1]) == 0)
            return;

        gp_Pnt2d center = Geom2dUtils::FindEllipseCenterFromEndpoints(_Points[0], _Points[1], newValue, newValue, 0.0, newValue > 0);
        gp_Ax2d xAxis(center, gp_Dir2d(gp_Vec2d(center, _Points[0])));
        double radius = center.Distance(_Points[0]);
        gp_Circ2d circ(xAxis, radius, newValue < 0);
        double endParameter = ElCLib::Parameter(circ, _Points[1]);
        _Points[2] = ElCLib::Value(endParameter / 2.0, circ);
        _MergePointIndices[2] = -1;

        if(_ValueHudElement != nullptr)
        {
            Remove(_ValueHudElement);
            _ValueHudElement = nullptr;
        }

        _PointsCompleted++;
        _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment},
                                                 _MergePointIndices[1] >= 0 ? -1 : 1);
    }

public:
    static Signal<void()> Stoped;

private:
    std::vector<int> _MergePointIndices;
    std::map<int, gp_Pnt2d> _Points;
    Handle(SketchSegmentArc) _Segment;
    Handle(SketchElementSegment) _Element;
    Handle(SketchPointAction) _PointAction;
    Handle(HintLine) _HintLine;
    Coord2DHudElement* _Coord2DHudElement;
    ValueHudElement* _ValueHudElement;
    int _PointsCompleted;
};

#endif // _SketchSegmentArcRimCreator_Header_File_
