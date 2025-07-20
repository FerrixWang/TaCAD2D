#ifndef SketchSegmentEllipse_Header_File_
#define SketchSegmentEllipse_Header_File_

#include <vector>

#include <TDataStd_Integer.hxx>
#include <Geom2d_Ellipse.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>
#include <TCollection_AsciiString.hxx>
#include <Message.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Core/Geom/Geom2dUtils.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

// ================================================================================
//! SketchSegmentEllipse
//  p0 = Center point
//  p1, p2 = Rim point and major axis point
// ================================================================================
class SketchSegmentEllipse : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor
    SketchSegmentEllipse(int center, int p1, int p2,
                         const TDF_Label& label = DocumentTool::SketchSegmentEllipsesLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentEllipse");
        PointsAttribute()->Init(0, 2);
        PointsAttribute()->SetValue(0, center);
        PointsAttribute()->SetValue(1, p1);
        PointsAttribute()->SetValue(2, p2);
    }

    //! Constructor
    SketchSegmentEllipse(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.IsNull());
    };

    //! Destructor
    virtual ~SketchSegmentEllipse() = default;

    virtual int StartPoint() const override { return -1; }
    virtual int EndPoint() const override { return -1; }

    virtual Handle(SketchSegment) Clone() const
    {
        auto newLabel = DocumentTool::SketchSegmentEllipsesLabel().NewChild();
        DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentEllipse(newLabel);
    };

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
        return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(1)),
                                            points.at(PointsAttribute()->Value(0)),
                                            points.at(PointsAttribute()->Value(2)));
    }

    TDF_Label Label() const { return _Label; }

    int CenterPoint() const { return PointsAttribute()->Value(0); }
    int RimPoint1() const { return PointsAttribute()->Value(1); }
    int RimPoint2() const { return PointsAttribute()->Value(2); }

    //! Create a gp_Elips2d from the points
    gp_Elips2d GetEllipse(const std::map<int, gp_Pnt2d>& points) const
    {
        auto itCenter = points.find(PointsAttribute()->Value(0));
        auto itRim1 = points.find(PointsAttribute()->Value(1));
        auto itRim2 = points.find(PointsAttribute()->Value(2));
        if(itCenter == points.end() || itRim1 == points.end() || itRim2 == points.end())
            return gp_Elips2d();

        const gp_Pnt2d& pCenter = itCenter->second;
        const gp_Pnt2d& pRim1 = itRim1->second;
        const gp_Pnt2d& pRim2 = itRim2->second;

        if(pRim1.Distance(pRim2) <= 0.0 || pRim2.Distance(pCenter) <= 0.0 || pRim1.Distance(pCenter) <= 0.0)
            return gp_Elips2d();

        // Print point information with Message::SendInfo()
        TCollection_AsciiString anInfo;
        anInfo += "Center Point: ";
        anInfo += TCollection_AsciiString(pCenter.X());
        anInfo += ", ";
        anInfo += TCollection_AsciiString(pCenter.Y());
        anInfo += " | Rim Point 1: ";
        anInfo += TCollection_AsciiString(pRim1.X());
        anInfo += ", ";
        anInfo += TCollection_AsciiString(pRim1.Y());
        anInfo += " | Rim Point 2: ";
        anInfo += TCollection_AsciiString(pRim2.X());
        anInfo += ", ";
        anInfo += TCollection_AsciiString(pRim2.Y());
        Message::SendInfo(anInfo);

        return Geom2dUtils::MakeEllipse(pCenter, pRim1, pRim2);
    }

    //! Create the corresponding Geom2d curve
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
    {
        gp_Elips2d ellipse = GetEllipse(points);

        return new Geom2d_Ellipse(ellipse);
    }

    //! Get the index of the major axis point
    int GetMajorAxisPoint(const std::map<int, gp_Pnt2d>& points) const
    {
        auto itCenter = points.find(PointsAttribute()->Value(0));
        auto itRim1 = points.find(PointsAttribute()->Value(1));
        auto itRim2 = points.find(PointsAttribute()->Value(2));
        if(itCenter == points.end() || itRim1 == points.end() || itRim2 == points.end())
            return -1;

        const gp_Pnt2d& center = itCenter->second;
        const gp_Pnt2d& p1 = itRim1->second;
        const gp_Pnt2d& p2 = itRim2->second;

        if(p1.Distance(p2) <= 0.0 || p2.Distance(center) <= 0.0 || p1.Distance(center) <= 0.0)
            return -1;

        return (p1.Distance(center) < p2.Distance(center)) 
            ? PointsAttribute()->Value(2) 
            : PointsAttribute()->Value(1);
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentEllipse, SketchSegment)

#endif // _SketchSegmentEllipse_Header_File_
