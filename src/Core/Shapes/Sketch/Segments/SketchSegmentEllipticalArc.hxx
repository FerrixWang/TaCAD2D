//! class SketchSegmentEllipticalArc

#ifndef SketchSegmentEllipticalArc_Header_File_
#define SketchSegmentEllipticalArc_Header_File_

#include <ElCLib.hxx>
#include <gce_MakeElips2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataStd_Integer.hxx>

#include "Core/Geom/Geom2dUtils.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

// ================================================================================
//! SketchSegmentEllipticalArc
//!   p0, p1 = end points (rim and major©\axis points)
//!   p2       = center point
// ================================================================================
class SketchSegmentEllipticalArc : public SketchSegment
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
public:
    //! Constructor
    SketchSegmentEllipticalArc(int p0, int p1, int center,
                               const TDF_Label& label = DocumentTool::SketchSegmentEllipticalArcsLabel().NewChild())
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(_Label, "SketchSegmentEllipticalArc");
        PointsAttribute()->Init(0, 2);
        PointsAttribute()->SetValue(0, p0);
        PointsAttribute()->SetValue(1, p1);
        PointsAttribute()->SetValue(2, center);
    }

    //! Constructor
    SketchSegmentEllipticalArc(const TDF_Label& label)
        : SketchSegment(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.IsNull());
    };

    virtual ~SketchSegmentEllipticalArc() = default;

    virtual int StartPoint() const override { return PointsAttribute()->Value(0); }
    virtual int EndPoint() const override { return PointsAttribute()->Value(1); }

    virtual Handle(SketchSegment) Clone() const
    {
        auto newLabel = DocumentTool::SketchSegmentEllipticalArcsLabel().NewChild();
        DocumentTool::CopyLabel(_Label, newLabel);
        return new SketchSegmentEllipticalArc(newLabel);
    };

    TDF_Label Label() const { return _Label; }

    int CenterPoint() const { return PointsAttribute()->Value(2); }

    virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) override
    {
        return SketchSegment::MakeHintEdge(points.at(PointsAttribute()->Value(0)),
                                           points.at(PointsAttribute()->Value(2)),
                                           points.at(PointsAttribute()->Value(1)));
    }

    //! Build the 2d ellipse from the three defining points.
    gp_Elips2d GetEllipse(const std::map<int, gp_Pnt2d>& points, 
                          std::vector<double>& parameters) const
    {
        auto it0 = points.find(PointsAttribute()->Value(0));
        auto it1 = points.find(PointsAttribute()->Value(1));
        auto itC = points.find(PointsAttribute()->Value(2));
        if(it0 == points.end() || it1 == points.end() || itC == points.end())
            return {};

        const gp_Pnt2d& p0 = it0->second;
        const gp_Pnt2d& p1 = it1->second;
        const gp_Pnt2d& c = itC->second;

        // Must have non©\degenerate radii
        if(p0.Distance(c) <= Precision::Confusion() ||
           p1.Distance(c) <= Precision::Confusion() ||
           p0.Distance(p1) <= Precision::Confusion())
        {
            return {};
        }

        // center, first end (rim), second end (major axis)
        gp_Elips2d ellipse = Geom2dUtils::MakeEllipse(c, p0, p1);

        if(ellipse.MajorRadius() <= 0.0 || ellipse.MinorRadius() <= 0.0)
            return {};

        if (parameters.size() == 2) {
            parameters[0] = ElCLib::Parameter(ellipse, p0);
            parameters[1] = ElCLib::Parameter(ellipse, p1);
        }

        return ellipse;
    }

    //! Create the trimmed 2d curve for display or modeling.
    virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
    {
        std::vector<double> parms(2);
        gp_Elips2d ellipse = GetEllipse(points, parms);

		if(Geom2dUtils::isInvalidEllipse(ellipse))
		{
			return nullptr;
		}

        return new Geom2d_TrimmedCurve(new Geom2d_Ellipse(ellipse), parms[0], parms[1]);
    }

    //! Determine which of the two end points lies on the major axis.
    int GetMajorAxisPoint(const std::map<int, gp_Pnt2d>& points) const
    {
        auto it0 = points.find(PointsAttribute()->Value(0));
        auto it1 = points.find(PointsAttribute()->Value(1));
        auto itC = points.find(PointsAttribute()->Value(2));
        if(it0 == points.end() || it1 == points.end() || itC == points.end())
            return -1;

        const gp_Pnt2d& p0 = it0->second;
        const gp_Pnt2d& p1 = it1->second;
        const gp_Pnt2d& c = itC->second;

        Standard_Real d0 = p0.Distance(c);
        Standard_Real d1 = p1.Distance(c);
        if(d0 <= Precision::Confusion() || d1 <= Precision::Confusion())
            return -1;

        return (d0 < d1) ? PointsAttribute()->Value(1) : PointsAttribute()->Value(0);
    }
private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentEllipticalArc, SketchSegment)

#endif  // SketchSegmentEllipticalArc_Header_File_
