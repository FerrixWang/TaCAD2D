#ifndef SketchSegmentBezier_Header_File_
#define SketchSegmentBezier_Header_File_

#include <map>
#include <vector>

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchSegmentBezier : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor for quadratic Bezier (3 points)
    SketchSegmentBezier(int p1, int cp, int p2, 
                        const TDF_Label& label = DocumentTool::SketchSegmentBeziersLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentBezier");
        PointsAttribute()->Init(0, 2);
        PointsAttribute()->SetValue(0, p1);
        PointsAttribute()->SetValue(1, cp);
        PointsAttribute()->SetValue(2, p2);
    }

    //! Constructor for cubic Bezier (4 points)
    SketchSegmentBezier(int p1, int cp1, int cp2, int p2, 
                        const TDF_Label& label = DocumentTool::SketchSegmentBeziersLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentBezier");
        PointsAttribute()->Init(0, 3);
        PointsAttribute()->SetValue(0, p1);
        PointsAttribute()->SetValue(1, cp1);
        PointsAttribute()->SetValue(2, cp2);
        PointsAttribute()->SetValue(3, p2);
    }

    //! Constructor
    SketchSegmentBezier(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.IsNull());
    };

    //! Destructor
    virtual ~SketchSegmentBezier() = default;

    virtual int StartPoint() const override { return PointsAttribute()->Value(0); }
    virtual int EndPoint() const override { return PointsAttribute()->Value(PointsAttribute()->Length() - 1); }

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
        if(Degree() == 2)
        {
			return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(0)),
											   points.at(PointsAttribute()->Value(1)),
											   points.at(PointsAttribute()->Value(2)));
        }
        else if(Degree() == 3)
        {
			return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(0)),
											   points.at(PointsAttribute()->Value(1)),
											   points.at(PointsAttribute()->Value(2)),
											   points.at(PointsAttribute()->Value(3)));
        }	
        return {};
    }

    virtual Handle(SketchSegment) Clone() const
    {
        auto newLabel = DocumentTool::SketchSegmentBeziersLabel().NewChild();
        DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentBezier(newLabel);
    };

    TDF_Label Label() const { return _Label; }

    //! Get Bezier degree
    int Degree() const { return PointsAttribute()->Length() - 1; }

    //! Create the Bezier curve geometry
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points)  override
    {
        auto itStart = points.find(StartPoint());
        auto itEnd = points.find(EndPoint());
        if(itStart == points.end() || itEnd == points.end())
            return nullptr;
        const gp_Pnt2d& p1 = itStart->second;
        const gp_Pnt2d& p2 = itEnd->second;
        if(p1.Distance(p2) <= 0)
            return nullptr;

        TColgp_Array1OfPnt2d curvePoles(1, PointsAttribute()->Length());
        for(int i = 0; i < PointsAttribute()->Length(); ++i)
        {
            auto it = points.find(PointsAttribute()->Value(i));
            if(it == points.end())
                return nullptr;
            curvePoles.SetValue(i + 1, it->second);
        }

        return new Geom2d_BezierCurve(curvePoles);
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentBezier, SketchSegment)

#endif // _SketchSegmentBezier_Header_File_
