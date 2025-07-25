#ifndef MirrorElementSketchTool_Header_File_
#define MirrorElementSketchTool_Header_File_

#include <map>
#include <vector>

#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <TDataXtd_Shape.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Core/Shapes/Sketch/Elements/SketchElementPoint.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchTool.hxx"
#include "Iact/ToolActions/SketchPointAction.hxx"
#include "Iact/Visual/HintLine.hxx"

class SketchToolMirror : public SketchTool
{
public:
    SketchToolMirror()
        : SketchTool()
        , _PreviewPoint(0)
		, _PreviewIsValid(false)
		, _PointAction(nullptr)
		, _Coord2DHudElement(nullptr)
		, _AxisHint(nullptr)
    {}

    virtual ~SketchToolMirror() = default;

    bool OnStart() override
    {    
        _SketchEditorTool->UpdateSelections();
        _OriginalSegments = _SketchEditorTool->SelectedSegments();

        if(_OriginalSegments.empty()) return false;

        std::set<int> uniquePoints;
        for(const auto& seg : _OriginalSegments)
        {
            auto points = seg->PointsAttribute()->Array();
            uniquePoints.insert(points->begin(), points->end());
        }
        _OriginalPoints.assign(uniquePoints.begin(), uniquePoints.end());

        for(int i = 0; i < _OriginalSegments.size(); i++)
        {
			auto segment = _OriginalSegments[i]->Clone();
            if(!Handle<SketchSegmentEllipticalArc>::DownCast(segment).IsNull())
            {
				segment->PointsAttribute()->SetValue(0, _OriginalSegments[i]->PointsAttribute()->Value(1));
				segment->PointsAttribute()->SetValue(1, _OriginalSegments[i]->PointsAttribute()->Value(0));
            }
            _TempSegments.emplace(i, segment);
        }

        _PointAction = new SketchPointAction(_SketchEditorTool);
        if(!StartAction(_PointAction))
            return false;

        _PointAction->Preview.connect([this](const auto& thePointAction) { _PointAction_Preview(thePointAction); });
        _PointAction->Finished.connect([this](const auto& thePointAction) { _PointAction_Finished(thePointAction); });

        _Coord2DHudElement = new Coord2DHudElement();
        Add(_Coord2DHudElement);
        SetHintMessage("Select first point to define the mirror axis.");

        return true;
    }

    virtual void OnStop() override
    {
        SketchTool::OnStop();
        Stoped();
    }

    void cleanup() override
    {
		for(const auto& [key, value] : _PreviewPointElements)
		{
			if(value.IsNull()) continue;
			value->ForgetAllAttributes();
		}
		_PreviewPointElements.clear();
		for(const auto& [key, value] : _PreviewSegmentElements)
		{
			if(value.IsNull()) continue;
			value->ForgetAllAttributes();
		}
		_PreviewSegmentElements.clear();

        SketchTool::cleanup();
    }

private:
    void _PointAction_Preview(const Handle<SketchPointAction>& _PointAction)
    {
        auto senderPoint = _PointAction->Point();

        if(_PreviewPoint == 0)
        {
            _Coord2DHudElement->setValues(senderPoint.X(), senderPoint.Y());
        }
        else if(_PreviewPoint == 1)
        {
            _Points[1] = senderPoint;

            gp_Vec2d p2p(_MirrorAxis.Location(), senderPoint);
            if(p2p.SquareMagnitude() < 1e-5) return;

            double orientation = gp_Dir2d(0, 1).Angle(p2p);

            gp_Vec2d normalized_p2p = p2p.Normalized();
            double distance = _Points[0].Distance(_Points[1]);

            double half_length = 1.5 * distance; // °ë³¤¶È = 1.5 * distance
            gp_Pnt2d end1 = _Points[0].Translated(normalized_p2p * half_length);
            gp_Pnt2d end2 = _Points[0].Translated(normalized_p2p * -half_length);

            _AxisHint->Set(end1, end2, _Sketch->Plane());
            _MirrorAxis = gp_Ax2d(_Points[0], normalized_p2p);

			_UpdateElements();
        }
    }

    void _PointAction_Finished(const Handle<SketchPointAction>& _PointAction)
    {
        auto senderPoint = _PointAction->Point();

        if(_PreviewPoint == 0)
        {
            _MirrorAxis = gp_Ax2d(senderPoint, gp_Dir2d(0, 1));
            _AxisHint = new HintLine(HintStyle::WorkingAxis);
            Add(_AxisHint);
          
			_Points[0] = senderPoint;
            _AxisHint->Set(_MirrorAxis, _Sketch->Plane());
            _PointAction->Reset();
            SetHintMessage("");
            _PreviewPoint++;
        }
        else if(_PreviewPoint == 1)
        {
            // Do mirror
            _UpdateElements();
            _Sketch->AddElements(_TempPoints, {}, _TempSegments, {});
            _OriginalSegments.clear();
            Stop();
        }
    }

    void _UpdateSegmentElementsVisibility(bool show) {}

    // Utility functions
    static double DegToRad(double deg) { return deg * M_PI / 180.0; }
    static double RadToDeg(double rad) { return rad * 180.0 / M_PI; }
    static double RoundToNearest(double value, double step)
    {
        return std::round(value / step) * step;
    }
    static double NormalizeAngleDeg(double angle, double min, double max)
    {
        while(angle > max) angle -= 360.0;
        while(angle < min) angle += 360.0;
        return angle;
    }

    void _UpdateElements()
    {
        if(!_PreviewIsValid)
        {
            _UpdateSegmentElementsVisibility(false);

            // Update point positions
            const auto& sketchPoints = _SketchEditorTool->GetSketch()->Points();
            for(const auto& point : _OriginalPoints)
            {
                _PreviewPointElements[point] = new SketchElementPoint(point, gp::Origin2d());
            }

            for(const auto& [key, seg] : _TempSegments)
            {
                _PreviewSegmentElements[key] = new SketchElementSegment(seg);
            }

            _PreviewIsValid = true;
        }

        // Update point positions
        const auto& sketchPoints = _SketchEditorTool->GetSketch()->Points();
        for(const auto& point : _OriginalPoints)
        {
            _TempPoints[point] = sketchPoints.at(point).Mirrored(_MirrorAxis);
            _PreviewPointElements[point]->OnPointsChanged(_TempPoints, {});
        }

        for(const auto& [key, seg] : _TempSegments)
        {
            _PreviewSegmentElements[key]->OnPointsChanged(_TempPoints, {});    
        }
    }

public:
    static Signal<void()> Stoped;

private:    
    std::vector<Handle<SketchSegment>> _OriginalSegments;
    std::vector<int> _OriginalPoints;
    std::map<int, gp_Pnt2d> _Points;
    std::map<int, gp_Pnt2d> _TempPoints;
    std::map<int, Handle<SketchSegment>> _TempSegments;
    std::map<int, Handle<SketchElementSegment>> _PreviewSegmentElements;
    std::map<int, Handle<SketchElementPoint>> _PreviewPointElements;
    Handle<HintLine> _AxisHint;
    Handle<SketchPointAction> _PointAction;
    gp_Ax2d _MirrorAxis;
    Coord2DHudElement* _Coord2DHudElement;
    int _PreviewPoint;
    bool _PreviewIsValid;
};

#endif // _MirrorElementSketchTool_Header_File_
