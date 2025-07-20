#ifndef SketchSegmentRectangleCreator_Header_File_
#define SketchSegmentRectangleCreator_Header_File_

#include <map>
#include <vector>

#include <AIS_Shape.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_GeometryEnum.hxx>
#include <TPrsStd_GeometryDriver.hxx>

#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/MultiValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCreator.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"

class SketchSegmentRectangleCreator : public SketchSegmentCreator
{
public:
    //! Constructor
    SketchSegmentRectangleCreator()
        : SketchSegmentCreator()
        , _PointAction(nullptr)
        , _Coord2DHudElement(nullptr)
        , _MultiValueHudElement(nullptr)
    {
        _MergePointIndices = {-1, -1, -1, -1};
    }

    //! Destructor
    virtual ~SketchSegmentRectangleCreator() = default;

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
        SetHintMessage("Select first corner of the rectangle.");

        return true;
    }

    virtual void OnStop() override
    {
        SketchSegmentCreator::OnStop();
        Stoped();
    }

    virtual void cleanup() override
    {
        for(const auto& segment : _Segments)
        {
            if(!segment.IsNull()) segment->ForgetAllAttributes();
        }
        for(const auto& element : _Elements)
        {
            if(!element.IsNull()) element->ForgetAllAttributes();
        }
        SketchSegmentCreator::cleanup();
    }

private:
    void _PointAction_Preview(const Handle(SketchPointAction)& sender)
    {
        auto point = sender->Point();

        if(!_Segments.empty())
        {
            _UpdateCornerPoints(point);
            for(const auto& element : _Elements)
            {
				element->OnPointsChanged(_Points, {});
            }

            if(_MultiValueHudElement == nullptr)
            {
                _MultiValueHudElement = new MultiValueHudElement();
                _MultiValueHudElement->SetLabel1("Size X:");
                _MultiValueHudElement->SetLabel2("Size Y:");
                Add(_MultiValueHudElement);
            }
            _MultiValueHudElement->SetValue1(_Points[2].X() - _Points[0].X());
            _MultiValueHudElement->SetValue2(_Points[2].Y() - _Points[0].Y());
        }
        _Coord2DHudElement->setValues(point.X(), point.Y());
    }

    void _PointAction_Finished(const Handle(SketchPointAction)& sender)
    {
        if(_Segments.empty())
        {
            _Points.emplace(0, sender->Point());
            _MergePointIndices[0] = sender->MergeCandidateIndex();
            _UpdateCornerPoints(sender->Point());

            _Segments = {
                new SketchSegmentLine(0, 1),
                new SketchSegmentLine(1, 2),
                new SketchSegmentLine(2, 3),
                new SketchSegmentLine(3, 0)
            };

            for(const auto& _Segment: _Segments)
            {
                _Elements.push_back(new SketchElementSegment(_Segment));
                _Elements.back()->OnPointsChanged(_Points, {});
            }

            SetHintMessage("Select second corner of the rectangle.");
            _PointAction->Reset();
        }
        else
        {
            _SetSecondPoint(sender->Point(), sender->MergeCandidateIndex());
        }
    }

    void _SetSecondPoint(const gp_Pnt2d& point, int mergeCandidateIndex)
    {
        if(std::abs(_Points[0].X() - point.X()) < 0.001 ||
           std::abs(_Points[0].Y() - point.Y()) < 0.001)
        {
            _PointAction->Reset();
            return;
        }

        StopAction(_PointAction);

        _UpdateCornerPoints(point);
        _MergePointIndices[1] = -1;
        _MergePointIndices[2] = mergeCandidateIndex;
        _MergePointIndices[3] = -1;

        // convert SketchSegmentLines to SketchSegment
		std::vector<Handle(SketchSegment)> segments;
        for(const auto& s : _Segments)
        {
			segments.push_back(s);
        }

        _SketchEditorTool->FinishSegmentCreation(_Points, _MergePointIndices, segments, 1);
    }

    void _UpdateCornerPoints(const gp_Pnt2d& second)
    {
        auto first = _Points[0];
        _Points[1] = gp_Pnt2d(first.X(), second.Y());
        _Points[2] = gp_Pnt2d(second.X(), second.Y());
        _Points[3] = gp_Pnt2d(second.X(), first.Y());
    }

    void _MultiValueHudElement_ValueEntered(double newValue1, double newValue2)
    {
        if(newValue1 == 0 || newValue2 == 0)
            return;

        gp_Vec2d vec(newValue1, newValue2);
        _SetSecondPoint(_Points[0].Translated(vec), -1);
    }

public:
    static Signal<void()> Stoped;

private:
    std::map<int, gp_Pnt2d> _Points;
    std::vector<Handle(SketchSegmentLine)> _Segments;
    std::vector<Handle(SketchElementSegment)> _Elements;
    Handle(SketchPointAction) _PointAction;
    Coord2DHudElement* _Coord2DHudElement;
    MultiValueHudElement* _MultiValueHudElement;
    std::vector<int> _MergePointIndices;
};

#endif // _SketchSegmentRectangleCreator_Header_File_