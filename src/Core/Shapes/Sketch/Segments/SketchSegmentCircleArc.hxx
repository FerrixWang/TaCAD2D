#ifndef SketchSegmentCircleArc_Header_File_
#define SketchSegmentCircleArc_Header_File_

#include <ElCLib.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Failure.hxx>
#include <TDataStd_Integer.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchSegmentCircleArc : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor with start, end, and rim points
    SketchSegmentCircleArc(int start, int end, int center,
                     const TDF_Label& label = DocumentTool::SketchSegmentCircleArcsLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentCircleArc");
        PointsAttribute()->Init(0, 2);
        PointsAttribute()->SetValue(0, start);
        PointsAttribute()->SetValue(1, end);
        PointsAttribute()->SetValue(2, center);
    }

    //! Constructor
    SketchSegmentCircleArc(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.IsNull());
    };

    //! Destructor
    virtual ~SketchSegmentCircleArc() = default;

    virtual int StartPoint() const override { return PointsAttribute()->Value(0); }
    virtual int EndPoint() const override { return PointsAttribute()->Value(1); }

    virtual Handle(SketchSegment) Clone() const
    {
        auto newLabel = DocumentTool::SketchSegmentCircleArcsLabel().NewChild();
        DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentCircleArc(newLabel);
    };

    TDF_Label Label() const { return _Label; }

    //! Get rim point index
    int CenterPoint() const { return PointsAttribute()->Value(2); }

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
        return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(0)),
                                            points.at(PointsAttribute()->Value(2)),
                                            points.at(PointsAttribute()->Value(1)));
    }

    //! Create the arc curve geometry
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
    {
        try
        {
            // Print point information with Message::SendInfo()
            Message::SendInfo() << "\nSketchSegmentCircleArc" 
                << "\nStart Point: " << points.at(0).X() << ", " << points.at(0).Y()
                << "\nEnd Point: " << points.at(1).X() << ", " << points.at(1).Y()
                << "\nCenter Point: " << points.at(2).X() << ", " << points.at(2).Y();

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
        auto itCenter = points.find(PointsAttribute()->Value(2));

        if(itStart == points.end() || itEnd == points.end() || itCenter == points.end())
            return gp_Circ2d();

        const gp_Pnt2d& start = itStart->second;
        const gp_Pnt2d& end = itEnd->second;
        const gp_Pnt2d& center = itCenter->second;

        // Check for invalid distances
        double distStartEnd = start.Distance(end);
        double distStartCenter = start.Distance(center);
        double distEndCenter = end.Distance(center);
        if(distStartEnd <= 0 || distStartCenter <= 0 || distEndCenter <= 0)
            return gp_Circ2d();

        // Create circle from center and radius
        gp_Ax2d axis(center, gp_Dir2d(gp_Vec2d(center, start)));
        double radius = distStartCenter;
        gp_Circ2d circ(axis, radius, false);

        // Project end point to find parameter
        Geom2dAPI_ProjectPointOnCurve proj(end, new Geom2d_Circle(circ));
        double endParam = ElCLib::Parameter(circ, proj.NearestPoint());

        // Calculate rim point
        gp_Pnt2d rim = ElCLib::Value(endParam / 2.0, circ);

        // Validate rim point distances
        if(start.Distance(rim) <= 0 || end.Distance(rim) <= 0)
            return gp_Circ2d();

        // Create final circle through three points
        gp_Circ2d circ1 = gce_MakeCirc2d(start, rim, end).Value();

        // Store parameters if required
        if(parameters.size() >= 2)
        {
            parameters[0] = ElCLib::Parameter(circ1, start);
            parameters[1] = ElCLib::Parameter(circ1, end);
        }

        return circ1;
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

DEFINE_STANDARD_HANDLE(SketchSegmentCircleArc, SketchSegment)

#endif // _SketchSegmentCircleArc_Header_File_
