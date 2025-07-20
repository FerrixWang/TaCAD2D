#ifndef SketchSegmentArcCenterCreator_Header_File_
#define SketchSegmentArcCenterCreator_Header_File_

#include <map>
#include <vector>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Shapes/Sketch/Segments/SketchSegmentCircleArc.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintCircle.hxx"
#include "Iact/Visual/HintLine.hxx"

class SketchSegmentArcCenterCreator : public SketchSegmentCreator
{
public:
    //! Constructor
    SketchSegmentArcCenterCreator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Coord2DHudElement(nullptr)
        , _ValueHudElement(nullptr)
        , _PointsCompleted(0)
        , _LastEndParameter(0.0)
        , _ArcDirection(false)
        , _StartPointFirst(false)
    {
        _MergePointIndices = {-1, -1, -1};
    }

    //! Destructor
    virtual ~SketchSegmentArcCenterCreator() = default;

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
        SetHintMessage("Select center point for circular arc.");

        return true;
    }

    virtual bool Continue(int continueWithPoint) override
    {
        SketchSegmentCreator::Continue(continueWithPoint);

        _Points[0] = _SketchEditorTool->GetSketch()->Point(continueWithPoint);
        _MergePointIndices[0] = continueWithPoint;

        _Segment.Nullify();
        _Element.Nullify();
        _PointAction->Reset();
        _PointsCompleted = 0;
        _StartPointFirst = true;
        SetHintMessage("Select center point for circular arc.");
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
        auto currentPoint = sender->Point();

        if(_PointsCompleted == 0)
        {
            if(_StartPointFirst)
            {
                auto p1 = _Points[0];

                if(_HintLine0.IsNull())
                {
                    _HintLine0 = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                    Add(_HintLine0);
                }
                _HintLine0->Set(_Points[0], currentPoint, _SketchEditorTool->GetSketch()->Plane());

                if(_ValueHudElement == nullptr)
                {
                    _ValueHudElement = new ValueHudElement();
                    _ValueHudElement->SetLabel("Radius:");

                    Add(_ValueHudElement);
                }
                _ValueHudElement->SetValue(p1.Distance(currentPoint));
            }
        }
        else if(_PointsCompleted == 1)
        {
            if(_HintLine0.IsNull())
            {
                _HintLine0 = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine0);
            }
            _HintLine0->Set(_Points[2], sender->Point(), GetSketch()->Plane());

            gp_Circ2d circ = gce_MakeCirc2d(_Points[2], currentPoint).Value();
            if(_PreviewCircle.IsNull())
            {
                _PreviewCircle = new HintCircle(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_PreviewCircle);
            }
            _PreviewCircle->Set(circ, GetSketch()->Plane());

            if(_ValueHudElement == nullptr)
            {
                _ValueHudElement = new ValueHudElement();
                _ValueHudElement->SetLabel("Radius:");
                Add(_ValueHudElement);
            }                
            _ValueHudElement->SetValue(circ.Radius());
        }
        else if(_PointsCompleted == 2)
        {
            if(_HintLine1.IsNull())
            {
                _HintLine1 = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine1);
            }
            _HintLine1->Set(_Points[2], sender->Point(), GetSketch()->Plane());

            if(_ValueHudElement == nullptr)
            {
                _ValueHudElement = new ValueHudElement();
                _ValueHudElement->SetLabel("Angle:");

                Add(_ValueHudElement);
            }

            if(!_Segment.IsNull() && !_Element.IsNull())
            {
                if(_CalcArcRimPoints(currentPoint))
                {
					auto points = _Points;

                    if(_ArcDirection == true)
                    {
						std::swap(points[0], points[1]);
                    }

                    _Element->OnPointsChanged(_Points, {});

                    _ValueHudElement->SetValue(_Segment->Angle(_Points) * 180.0 / M_PI);
                }
                else
                {
                    _ValueHudElement->SetValue(0.0);
                }
            }
        }

        _Coord2DHudElement->setValues(currentPoint.X(), currentPoint.Y());
        _LastActionPoint = currentPoint;
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        auto point = sender->Point();
        int mergeCandidateIndex = sender->MergeCandidateIndex();

        if(_PointsCompleted == 0)
        {
            _Points[2] = point;

            if(_StartPointFirst)
            {
                if(_Points[0].Distance(_Points[2]) < 0.001)
                {
                    _PointAction->Reset();
                    return;
                }
                _PointsCompleted++;
                _SetStartPoint(_Points[0], _MergePointIndices[0]);
            }
            else
            {
                _PointsCompleted++;
                SetHintMessage("Select start point for circular arc.");
            }
            _PointAction->Reset();
        }
        else if(_PointsCompleted == 1)
        {
            _SetStartPoint(point, mergeCandidateIndex);
        }
        else if(_PointsCompleted == 2)
        {
            if(!_CalcArcRimPoints(point) || _Points[0].Distance(_Points[1]) < 0.001)
            {
                _PointAction->Reset();
                return;
            }

            if(_ValueHudElement != nullptr)
            {
                Remove(_ValueHudElement);
                _ValueHudElement = nullptr;
            }

            _HintLine0.Nullify();
            _HintLine1.Nullify();
            _MergePointIndices[1] = mergeCandidateIndex;
            _MergePointIndices[2] = -1;

            _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment},
                                                     _MergePointIndices[1] >= 0 ? -1 : 1);
        }
    }

    bool _CalcArcRimPoints(const gp_Pnt2d& endPoint)
    {
        auto startPoint = _Points[0];
        if(startPoint.Distance(endPoint) <= 0 || endPoint.Distance(_Points[2]) <= 0 ||
           startPoint.Distance(_Points[2]) <= 0)
            return false;

        gp_Ax2d xAxis(_Points[2], gp_Dir2d(gp_Vec2d(_Points[2], startPoint)));
        double radius = _Points[2].Distance(startPoint);
        gp_Circ2d circ(xAxis, radius, _ArcDirection);

        Geom2dAPI_ProjectPointOnCurve proj(endPoint, new Geom2d_Circle(circ));
        gp_Pnt2d projEndPoint = proj.NearestPoint();


        double endParameter = ElCLib::Parameter(circ, projEndPoint);

        // Print point information with Message::SendInfo()
        Message::SendInfo() << "\nLastEndParameter: " << _LastEndParameter;
        Message::SendInfo() << "\nEndParameter: " << endParameter;
        Message::SendInfo() << "\nArcDirection: " << _ArcDirection;

        if((_LastEndParameter < 1.0 && endParameter > 5.0) ||
           (endParameter < 1.0 && _LastEndParameter > 5.0))
        {
            _ArcDirection = !_ArcDirection;

            circ = gp_Circ2d(xAxis, radius, _ArcDirection);
            endParameter = ElCLib::Parameter(circ, projEndPoint);
        }
        _LastEndParameter = endParameter;

        Message::SendInfo() << "\n_CalcArcRimPoints"
            << "\nStart Point: " << _Points.at(0).X() << ", " << _Points.at(0).Y()
            << "\nEnd Point: " << _Points.at(1).X() << ", " << _Points.at(1).Y()
            << "\nCenter Point: " << _Points.at(2).X() << ", " << _Points.at(2).Y();

        gp_Pnt2d rimPoint = ElCLib::Value(endParameter / 2.0, circ);

        _Points[1] = projEndPoint;
        return true;
    }

    void _SetStartPoint(const gp_Pnt2d& point, int mergeCandidateIndex)
    {
        if(_Points[2].Distance(point) < 0.001)
        {
            _PointAction->Reset();
            return;
        }
        Remove(_PreviewCircle);
        _PreviewCircle.Nullify();

        _HintLine0->Set(_Points[2], point, GetSketch()->Plane());

        if(_ValueHudElement != nullptr)
        {
            Remove(_ValueHudElement);
            _ValueHudElement = nullptr;
        }

        _Points[0] = point;
        _MergePointIndices[0] = mergeCandidateIndex;
        _PointsCompleted++;

        _Points[1] = point;
        _Segment = new SketchSegmentCircleArc(0, 1, 2);
        _Element = new SketchElementSegment(_Segment);
        _Element->OnPointsChanged(_Points, {});

        SetHintMessage("Select end point for circular arc.");

        _PointAction->Reset();
    }

    void _PointActionOnConstraintPoint(const Handle(SketchPointAction)& /*sender*/, gp_Pnt2d& point)
    {
        if(_PointsCompleted == 2)
        {
            if(_CalcArcRimPoints(point))
                point = _Points[1];
        }
    }

    void _ValueHudElement_RadiusEntered(double newValue)
    {
        if(newValue <= 0)
            return;

        if(_StartPointFirst)
        {
            gp_Vec2d vec(_Points[0], _LastActionPoint);
            if(vec.Magnitude() == 0)
                return;

            _Points[2] = _Points[0].Translated(vec.Normalized().Scaled(newValue));
            _PointsCompleted = 1;
            _SetStartPoint(_Points[0], _MergePointIndices[0]);
            SetHintMessage("Select start point for circular arc.");
            _PointAction->Reset();
        }
        else
        {
            gp_Vec2d vec(_Points[2], _LastActionPoint);
            if(vec.Magnitude() == 0)
                return;

            _SetStartPoint(_Points[2].Translated(vec.Normalized().Scaled(newValue)), -1);
        }
    }

    void _ValueHudElement_AngleEntered(double newValue)
    {
        if(newValue == 0)
            return;

        auto startPoint = _Points[0];
        if(startPoint.Distance(_Points[2]) <= 0)
            return;

        gp_Ax2d xAxis(_Points[2], gp_Dir2d(gp_Vec2d(_Points[2], startPoint)));
        double radius = _Points[2].Distance(startPoint);
        gp_Circ2d circ(xAxis, radius, _ArcDirection);
        double startParameter = ElCLib::Parameter(circ, startPoint);

        double endParameter = startParameter + (newValue * M_PI / 180.0);
        _ArcDirection = newValue < 0;
        gp_Pnt2d endPoint = ElCLib::Value(endParameter, circ);

        if(!_CalcArcRimPoints(endPoint))
            return;

        if(_ValueHudElement != nullptr)
        {
            Remove(_ValueHudElement);
            _ValueHudElement = nullptr;
        }

        _MergePointIndices[1] = -1;
        _MergePointIndices[2] = -1;
        _PointsCompleted++;

        _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, 1);
    }

public:
    static Signal<void()> Stoped;

private:
    std::vector<int> _MergePointIndices;
    std::map<int, gp_Pnt2d> _Points;
    Handle(SketchSegmentCircleArc) _Segment;
    Handle(SketchElementSegment) _Element;
    Handle(SketchPointAction) _PointAction;
	Handle(HintCircle) _PreviewCircle;
	Handle(HintLine) _HintLine0;
    Handle(HintLine) _HintLine1;
    gp_Pnt2d _LastActionPoint;
    Coord2DHudElement* _Coord2DHudElement;
    ValueHudElement* _ValueHudElement;
    double _LastEndParameter;
    int _PointsCompleted;
    bool _ArcDirection;
    bool _StartPointFirst;
};

#endif // _SketchSegmentArcCenterCreator_Header_File_
