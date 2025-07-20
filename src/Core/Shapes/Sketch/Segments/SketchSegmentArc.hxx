#ifndef SketchSegmentArc_Header_File_
#define SketchSegmentArc_Header_File_

#include <ElCLib.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Failure.hxx>
#include <TDataStd_Integer.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchSegmentArc : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor with start, end, and rim points
    SketchSegmentArc(int start, int end, int rim, 
                     const TDF_Label& label = DocumentTool::SketchSegmentArcsLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentArc");
        PointsAttribute()->Init(0, 2);
        PointsAttribute()->SetValue(0, start);
        PointsAttribute()->SetValue(1, end);
        PointsAttribute()->SetValue(2, rim);	
    }

    //! Constructor
    SketchSegmentArc(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    virtual Handle(SketchSegment) Clone() const
    { 
        auto newLabel = DocumentTool::SketchSegmentArcsLabel().NewChild();
		DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentArc(newLabel);
    };

    //! Destructor
    virtual ~SketchSegmentArc() = default;

    virtual int StartPoint() const override { return PointsAttribute()->Value(0); }
    virtual int EndPoint() const override { return PointsAttribute()->Value(1); }

    TDF_Label Label() const { return _Label; }

    //! Get rim point index
    int RimPoint() const { return PointsAttribute()->Value(2); }

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
        return SketchSegment::MakeHintEdge(Center(points), points.at(RimPoint()));
    }

    //! Create the arc curve geometry
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
    {
        try
        {
            std::vector<double> parameters(2, 0.0);
            gp_Circ2d circ = GetCircle(points, parameters);

            return new Geom2d_TrimmedCurve(new Geom2d_Circle(circ), parameters[0], parameters[1]);
        }
        catch(...)
        {
            return nullptr;
        }
    }

    //! Get the radius of the arc
    double Radius(const std::map<int, gp_Pnt2d>& points) const
    {
        try
        {
            std::vector<double> parameters(2, 0.0);
            gp_Circ2d circ = GetCircle(points, parameters);

            return circ.Radius();
        }
        catch(...)
        {
            return 0.0;
        }
    }

    //! Get the center point of the arc
    gp_Pnt2d Center(const std::map<int, gp_Pnt2d>& points) const
    {
        try
        {
			std::vector<double> parameters(2, 0.0);
            gp_Circ2d circ = GetCircle(points, parameters);

            return circ.Location();
        }
        catch(...)
        {
            return gp_Pnt2d(0.0, 0.0);
        }
    }

    //! Create a gp_Circ2d from the points
    gp_Circ2d GetCircle(const std::map<int, gp_Pnt2d>& points, std::vector<double>& parameters) const
    {
        auto itStart = points.find(PointsAttribute()->Value(0));
        auto itEnd = points.find(PointsAttribute()->Value(1));
        auto itRim = points.find(PointsAttribute()->Value(2));
        if(itStart == points.end() || itEnd == points.end() || itRim == points.end())
            return gp_Circ2d();

        const gp_Pnt2d& start = itStart->second;
        const gp_Pnt2d& end = itEnd->second;
        const gp_Pnt2d& rim = itRim->second;

        if(start.Distance(end) <= 0 || end.Distance(rim) <= 0 || start.Distance(rim) <= 0)
            return gp_Circ2d();

        gp_Circ2d circ = gce_MakeCirc2d(start, rim, end).Value();

        if(parameters.size() >= 2)
        {
            parameters[0] = ElCLib::Parameter(circ, start);
            parameters[1] = ElCLib::Parameter(circ, end);
        }

        return circ;
    }

    //! Get the angle of the arc
    double Angle(const std::map<int, gp_Pnt2d>& points) const
    {
        std::vector<double> parameters(2, 0.0);
        gp_Circ2d circ = GetCircle(points, parameters);

        int sense = circ.Position().YAxis().Angle(circ.Position().XAxis()) > 0 ? 1 : -1;
        return (parameters[1] - parameters[0]) * (sense ? 1.0 : -1.0);
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentArc, SketchSegment)

#endif // _SketchSegmentArc_Header_File_
