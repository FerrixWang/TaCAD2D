#ifndef SketchSegmentEllipseCenterCreator_Header_File_
#define SketchSegmentEllipseCenterCreator_Header_File_

#include <array>
#include <map>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>

#include "Core/Shapes/Sketch/Segments/SketchSegmentEllipse.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/ValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintCircle.hxx"
#include "Iact/Visual/HintLine.hxx"
#include "Iact/Visual/Marker.hxx"

class SketchSegmentEllipseCenterCreator : public SketchSegmentCreator
{
public:
    SketchSegmentEllipseCenterCreator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Segment(nullptr)
        , _Coord2DHudElement(nullptr)
        , _ValueHudElement(nullptr)
        , _PointsCompleted(0)
    {
        _MergePointIndices = {-1, -1, -1};
    }

    virtual ~SketchSegmentEllipseCenterCreator() override = default;

    // Start the center©\based ellipse creation
    virtual bool OnStart() override
    {
		SketchSegmentCreator::OnStart();

        // Point©\picking action
        _PointAction = new SketchPointAction(_SketchEditorTool);
        if(!StartAction(_PointAction))
            return false;

        // Connect preview and finished events
        _PointAction->Preview.connect(
            [this](const Handle(SketchPointAction)& action) { _PointAction_Preview(action); });
        _PointAction->Finished.connect(
            [this](const Handle(SketchPointAction)& action) { _PointAction_Finished(action); });

        // Coordinate HUD
        _Coord2DHudElement = new Coord2DHudElement();
        Add(_Coord2DHudElement);

        SetHintMessage("Select center point of the ellipse.");
        return true;
    }

    // Cleanup all temporary visuals
    virtual void cleanup() override
    {
        if(!_Segment.IsNull()) _Segment->ForgetAllAttributes();
        if(!_Element.IsNull()) _Element->ForgetAllAttributes();
        if(_ValueHudElement)
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

private:
    // Preview handler: show hint circle or update ellipse preview
    void _PointAction_Preview(const Handle(SketchPointAction) pointaction)
    {
        const gp_Pnt2d pt = pointaction->Point();

        if(_PointsCompleted == 1)
        {
            if(_HintCircle.IsNull())
            {
                _HintCircle = new HintCircle(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintCircle);
            }

            if(_HintLine.IsNull())
            {
                _HintLine = new HintLine(HintStyle::ThinDashed | HintStyle::Topmost);
                Add(_HintLine);
            }

            gp_Circ2d circ = gce_MakeCirc2d(_Points[0], pt).Value();
            _HintCircle->Set(circ, _SketchEditorTool->GetSketch()->Plane());
            _HintLine->Set(_Points[0], pointaction->Point(), _SketchEditorTool->GetSketch()->Plane());

            // Distance HUD
            if(!_ValueHudElement)
            {
                _ValueHudElement = new ValueHudElement();
                _ValueHudElement->SetLabel("Distance:");

                Add(_ValueHudElement);
            }
            _ValueHudElement->SetValue(_Points[0].Distance(pt));

            // Store the first rim candidate
            _Points[1] = pt;
        }
        else if(_PointsCompleted == 2)
        {
            if(!_Segment.IsNull())
            {
                _Points[2] = pointaction->Point();
                _Element->OnPointsChanged(_Points, {});
            }
            // Update the previewed ellipse segment
            _Points[2] = pt;
            // Update distance HUD to reflect major axis length
            _ValueHudElement->SetValue(_Points[0].Distance(pt));
        }
    }

    // Finished handler: advance creation state
    void _PointAction_Finished(const Handle(SketchPointAction)& pointAction)
    {
        switch(_PointsCompleted)
        {
        case 0:
            _SetCenter(pointAction->Point(), pointAction->MergeCandidateIndex());
            break;
        case 1:
            _SetFirstRim(pointAction->Point(), pointAction->MergeCandidateIndex());
            break;
        case 2:
            _SetSecondRim(pointAction->Point(), pointAction->MergeCandidateIndex());
            break;
        }
    }

    // Record center point
    void _SetCenter(const gp_Pnt2d& pt, int mergeIndex)
    {
        _Points[0] = pt;
        _MergePointIndices[0] = mergeIndex;
        _PointsCompleted = 1;

        // Marker for center
        _Marker[0] = new Marker( MarkerStyle::Bitmap, Marker::BallImage());
        _Marker[0]->SetWorkspaceController(m_workspaceController);
        _Marker[0]->Set(pt, _SketchEditorTool->GetSketch()->Plane());
        Add(_Marker[0]);

        SetHintMessage("Select first rim point of the ellipse.");
        _PointAction->Reset();
    }

    // Record first rim point, create the segment element
    void _SetFirstRim(const gp_Pnt2d& pt, int mergeIndex)
    {
        if(_Points[0].Distance(pt) < Precision::Confusion())
        {
            _PointAction->Reset();
            return;
        }

        Remove(_HintCircle);
        _HintCircle.Nullify();
        Remove(_HintLine);
        _HintLine.Nullify();

        _Points[1] = pt;
        _MergePointIndices[1] = mergeIndex;
        _PointsCompleted = 2;

        // Marker for first rim
        _Marker[1] = new Marker(
            MarkerStyle::Bitmap,
            Marker::BallImage());
        _Marker[1]->SetWorkspaceController(m_workspaceController);
        _Marker[1]->Set(pt, _SketchEditorTool->GetSketch()->Plane());
        Add(_Marker[1]);

        // Initialize the ellipse segment with a degenerate second rim = first rim
        _Points[2] = pt;
        _Segment = new SketchSegmentEllipse(0, 1, 2);
        _Element = new SketchElementSegment(_Segment);
        _Element->OnPointsChanged(_Points, {});

        SetHintMessage("Select second rim point of the ellipse.");
        _PointAction->Reset();
        InteractiveContext::Current()->GetWorkspaceController()->Invalidate();
    }

    // Finalize with the second rim point
    void _SetSecondRim(const gp_Pnt2d& pt, int mergeIndex)
    {
        if(_Points[0].Distance(pt) < Precision::Confusion() ||
           _Points[1].Distance(pt) < Precision::Confusion())
        {
            _PointAction->Reset();
            return;
        }

        _Points[2] = pt;
        _MergePointIndices[2] = mergeIndex;

        StopAction(_PointAction);

        _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, {_Segment}, 1);
    }

    // Handle manual value entry for distance
    void _OnValueEntered(ValueHudElement&, double newValue)
    {
        if(newValue <= 0)
            return;

        if(_PointsCompleted == 1)
        {
            gp_Vec2d vec(_Points[0], _Points[1]);
            if(vec.Magnitude() <= Precision::Confusion())
                return;

            gp_Pnt2d pt = _Points[0].Translated(vec.Normalized().Scaled(newValue));
            _SetFirstRim(pt, -1);
        }
        else if(_PointsCompleted == 2)
        {
            gp_Vec2d vec(_Points[0], _Points[2]);
            if(vec.Magnitude() <= Precision::Confusion())
                return;

            gp_Pnt2d pt = _Points[0].Translated(vec.Normalized().Scaled(newValue));
            _SetSecondRim(pt, -1);
        }
    }
public:
    static Signal<void()> Stoped;

private:
    std::vector<int>                  _MergePointIndices;
    std::map<int, gp_Pnt2d>           _Points;
    Handle(SketchPointAction)         _PointAction;
    Handle(SketchSegmentEllipse)      _Segment;
    Handle(SketchElementSegment)      _Element;
    Handle(HintCircle)                _HintCircle;
    Handle(HintLine)                  _HintLine;
    Coord2DHudElement*                _Coord2DHudElement;
    ValueHudElement*                  _ValueHudElement;
    Marker*                           _Marker[2];
    int                               _PointsCompleted;
};

#endif  // SketchSegmentEllipseCenterCreator_Header_File_
