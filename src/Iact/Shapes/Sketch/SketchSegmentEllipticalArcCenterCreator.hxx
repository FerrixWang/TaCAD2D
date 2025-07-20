#ifndef SketchSegmentEllipticalArcCenterCreator_Header_File_
#define SketchSegmentEllipticalArcCenterCreator_Header_File_

#include <array>
#include <map>

#include <AIS_Shape.hxx>
#include <ElCLib.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>

#include "Core/Geom/Geom2dUtils.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipticalArc.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintLine.hxx"
#include "Iact/Visual/Marker.hxx"

class SketchSegmentEllipticalArcCenterCreator : public SketchSegmentCreator
{
public:
    SketchSegmentEllipticalArcCenterCreator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Segment(nullptr)
        , _Coord2DHudElement(nullptr)
        , _PointsCompleted(0)
        , _StartPointMergeIndex(-1)
        , _EndPointIndex(1)
        , _ArcDirection(false)
        , _StartPointFirst(false)
        , _LastEndParameter(0.0)
        , _LastParameterWasEnd(false)
    {}

    virtual ~SketchSegmentEllipticalArcCenterCreator() override = default;

    virtual bool OnStart() override
    {
        SketchSegmentCreator::OnStart();

        _PointAction = new SketchPointAction(_SketchEditorTool);
        if(!StartAction(_PointAction))
            return false;

        _PointAction->Preview.connect([this](const Handle(SketchPointAction)& sender) {
            _PointAction_Preview(sender);
        });

        _PointAction->Finished.connect([this](const Handle(SketchPointAction)& sender) {
            _PointAction_Finished(sender);
        });

        _Coord2DHudElement = new Coord2DHudElement();
        Add(_Coord2DHudElement);

        SetHintMessage("Select center point for elliptical arc.");
        return true;
    }

    virtual bool Continue(int continueWithPoint) override
    {
        auto& points = _SketchEditorTool->GetSketch()->Points();
        _Points[1] = points.at(continueWithPoint);
        _MergePointIndices[1] = continueWithPoint;

        _Segment.Nullify();
        _Segment.Nullify();
        _PointAction->Reset();
        _PointsCompleted = 0;
        _StartPointFirst = true;
        SetHintMessage("Select center point for elliptical arc.");
        return true;
    }

    virtual void cleanup() override
    {
        if(!_Segment.IsNull()) _Segment->ForgetAllAttributes();
        if(!_Element.IsNull()) _Element->ForgetAllAttributes();
        SketchSegmentCreator::cleanup();
    }

    virtual void OnStop() override
    {
        SketchSegmentCreator::OnStop();
        Stoped();
    }

private:
    void _PointAction_Preview(const Handle(SketchPointAction)& sender)
    {
        if(_PointsCompleted == 0)
        {
            if(_StartPointFirst)
            {
                const gp_Pnt2d& p1 = _Points[1];
                const gp_Pnt2d& p2 = sender->Point();

                if(_HintLine0.IsNull())
                {
                    _HintLine0 = new HintLine( HintStyle::ThinDashed | HintStyle::Topmost);
                    Add(_HintLine0);
                }

                _HintLine0->Set(p1, p2, _SketchEditorTool->GetSketch()->Plane());
            }
        }
        else if(_PointsCompleted == 1)
        {
            if(_HintLine0.IsNull())
            {
                _HintLine0 = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine0);
            }

            _HintLine0->Set(_CenterPoint, sender->Point(), _SketchEditorTool->GetSketch()->Plane());
        }
        else if(_PointsCompleted == 2)
        {
            if(_HintLine1.IsNull())
            {
                _HintLine1 = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine1);
            }

            _HintLine1->Set(_CenterPoint, sender->Point(), _SketchEditorTool->GetSketch()->Plane());

            if(!_Segment.IsNull())
            {
                if(_CalcArcRimPoints(sender->Point()))
                {
                    auto edge = _Segment->MakeEdge(_Points);
                    _Element->OnPointsChanged(_Points, {});
                }
            }
        }
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        if(_PointsCompleted == 0)
        {
            _CenterPoint = sender->Point();
            _Points[0] = sender->Point();
            _MergePointIndices[0] = sender->MergeCandidateIndex();

            if(_StartPointFirst)
            {
                if(_Points[1].Distance(_CenterPoint) < 0.001)
                {
                    _PointAction->Reset();
                    return;
                }

                _PointsCompleted++;
                _SetStartPoint(_Points[1], _MergePointIndices[1]);
            }
            else
            {
                _PointsCompleted++;
            }

            SetHintMessage("Select start point for elliptical arc.");
            _PointAction->Reset();
        }
        else if(_PointsCompleted == 1)
        {
            if(_CenterPoint.Distance(sender->Point()) < 0.001)
            {
                _PointAction->Reset();
                return;
            }

            _SetStartPoint(sender->Point(), sender->MergeCandidateIndex());
        }
        else if(_PointsCompleted == 2)
        {
            if(!_CalcArcRimPoints(sender->Point(), sender->MergeCandidateIndex()))
            {
                _PointAction->Reset();
                return;
            }

            _PointsCompleted++;

            RemoveVisuals();
            _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, 1);
        }
    }

    bool _CalcArcRimPoints(const gp_Pnt2d& endPoint, int mergeCandidateIndex = -1)
    {
        if(_StartPoint.Distance(endPoint) <= 0 || endPoint.Distance(_CenterPoint) <= 0 || _StartPoint.Distance(_CenterPoint) <= 0)
            return false;

        gp_Elips2d ellipse = Geom2dUtils::MakeEllipse(_CenterPoint, _StartPoint, endPoint, _ArcDirection);

        // Toggle direction based on parameters
        Standard_Real startParameter = ElCLib::Parameter(ellipse, _StartPoint);
        Standard_Real endParameter = ElCLib::Parameter(ellipse, endPoint);
        Standard_Real maxParameter = std::max(startParameter, endParameter);
        if((startParameter > endParameter && _LastParameterWasEnd) || (startParameter < endParameter && !_LastParameterWasEnd))
        {
            _LastEndParameter = maxParameter;
            _LastParameterWasEnd = !_LastParameterWasEnd;
        }
        else
        {
            if(maxParameter > 0.0001)
            {
                Standard_Real qa = ellipse.MajorRadius() / 4;
                if((_LastEndParameter < qa && maxParameter > qa * 2) || (maxParameter < qa && _LastEndParameter > qa * 2))
                {
                    _ArcDirection = !_ArcDirection;
                }
                _LastEndParameter = maxParameter;
            }
        }

        if(_ArcDirection)
        {
            _EndPointIndex = 2;
            _Points[1] = _StartPoint;
            _MergePointIndices[1] = _StartPointMergeIndex;
            _Points[2] = endPoint;
            _MergePointIndices[2] = mergeCandidateIndex;
        }
        else
        {
            _EndPointIndex = 1;
            _Points[1] = endPoint;
            _MergePointIndices[1] = mergeCandidateIndex;
            _Points[2] = _StartPoint;
            _MergePointIndices[2] = _StartPointMergeIndex;
        }
        return true;
    }

    void _SetStartPoint(const gp_Pnt2d& point, int mergeCandidateIndex)
    {
        _StartPoint = point;
        _StartPointMergeIndex = mergeCandidateIndex;
        _PointsCompleted++;

        if(!_StartPointFirst)
        {
            _Points[1] = point;
        }

        _Points[2] = point;

        _Segment = new SketchSegmentEllipticalArc(1, 2, 0);
        _Element = new SketchElementSegment(_Segment);
        _Element->OnPointsChanged(_Points, {});

        SetHintMessage("Select end point for elliptical arc.");
        _PointAction->Reset();
    }

public:
    static Signal<void()> Stoped;

private:
    std::map<int, gp_Pnt2d> _Points;
    std::vector<int> _MergePointIndices = {-1, -1, -1};
    gp_Pnt2d _CenterPoint;
    gp_Pnt2d _StartPoint;
    Handle(SketchPointAction) _PointAction;
    Handle(SketchSegmentEllipticalArc) _Segment;
	Handle(SketchElementSegment) _Element;
	Handle(HintLine) _HintLine0;
	Handle(HintLine) _HintLine1;
    Coord2DHudElement* _Coord2DHudElement;
    double _LastEndParameter;
    int _PointsCompleted;
    int _StartPointMergeIndex;
    int _EndPointIndex;
    bool _ArcDirection;
    bool _StartPointFirst;
    bool _LastParameterWasEnd;
};

#endif // SketchSegmentEllipticalArcCenterCreator_Header_File_
