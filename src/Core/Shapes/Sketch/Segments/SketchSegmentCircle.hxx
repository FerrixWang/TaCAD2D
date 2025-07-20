//! class SketchSegmentCircle

#ifndef SketchSegmentCircle_Header_File_
#define SketchSegmentCircle_Header_File_

#include <TDataStd_Integer.hxx>
#include <gce_MakeCirc2d.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

// ================================================================================
//! SketchSegmentCircle
//  p0 = Center point
//  p1 = Rim point
// ================================================================================
class SketchSegmentCircle : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor
    SketchSegmentCircle(int center, int rim, const TDF_Label& label = DocumentTool::SketchSegmentCirclesLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentCircle");
        PointsAttribute()->Init(0, 1);
        PointsAttribute()->SetValue(0, center);
        PointsAttribute()->SetValue(1, rim);
    }
    //! Constructor
    SketchSegmentCircle(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.IsNull());
    };

    //! Destructor
    virtual ~SketchSegmentCircle() = default;

    virtual Handle(SketchSegment) Clone() const
    {
        auto newLabel = DocumentTool::SketchSegmentCirclesLabel().NewChild();
        DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentCircle(newLabel);
    };

    TDF_Label Label() const { return _Label; }

    int CenterPoint() const { return PointsAttribute()->Value(0); }
    int RimPoint() const { return PointsAttribute()->Value(1); }

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
            return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(0)),
                                               points.at(PointsAttribute()->Value(1)));
    }

    //! Create a gp_Circ2d from the points
    gp_Circ2d GetCircle(const std::map<int, gp_Pnt2d>& points) const
    {
        auto itCenter = points.find(PointsAttribute()->Value(0));
        auto itRim = points.find(PointsAttribute()->Value(1));
        if (itCenter == points.end() || itRim == points.end())
            return {};

        const gp_Pnt2d& pCenter = itCenter->second;
        const gp_Pnt2d& pRim = itRim->second;
        
        if (pCenter.Distance(pRim) <= 0)
            return {};
        // Print point information with Message::SendInfo()
        TCollection_AsciiString anInfo;
        anInfo += "Center Point: ";
        anInfo += std::to_string(pCenter.X()).c_str();
        anInfo += ", ";
        anInfo += std::to_string(pCenter.Y()).c_str();
        anInfo += "Rim Point: ";
        anInfo += std::to_string(pRim.X()).c_str();
        anInfo += ", ";
        anInfo += std::to_string(pRim.Y()).c_str();
        Message::SendInfo(anInfo);

        return gce_MakeCirc2d(pCenter, pRim, true).Value();
    }

    //! Create the corresponding Geom2d curve
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
    {
        gp_Circ2d circ = GetCircle(points);
        if (circ.Radius() <= 0)
            return nullptr;

        return new Geom2d_Circle(circ);
    }

    //! Return the radius
    double Radius(const std::map<int, gp_Pnt2d>& points) const
    {
        auto itCenter = points.find(CenterPoint());
        auto itRim = points.find(RimPoint());
        if (itCenter == points.end() || itRim == points.end())
            return 0.0;

        return itCenter->second.Distance(itRim->second);
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentCircle, SketchSegment)

#endif  // SketchSegmentCircle_Header_File_
