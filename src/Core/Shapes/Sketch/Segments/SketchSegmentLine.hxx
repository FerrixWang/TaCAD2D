//! class SketchSegmentLine

#ifndef SketchSegmentLine_Header_File_
#define SketchSegmentLine_Header_File_

#include <ElCLib.hxx>
#include <gce_MakeLin2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Lin2d.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>

#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchSegmentLine : public SketchSegment
{	
	DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
	DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_Name, Base)
public:
	//! Constructor
	SketchSegmentLine(int begin, int end, 
					  const TDF_Label& label = DocumentTool::SketchSegmentLinesLabel().NewChild())
		: SketchSegment(label.FindChild(0))
		, _Label(label)
	{
		assert(!label.HasAttribute());

		NameAttribute()->Set(_Label, "SketchSegmentLine");
		PointsAttribute()->Init(0, 1);
		PointsAttribute()->SetValue(0, begin);
		PointsAttribute()->SetValue(1, end);
	};

	//! Constructor
	SketchSegmentLine(const TDF_Label& label)
		: SketchSegment(label.FindChild(0))
		, _Label(label)
	{
		assert(!label.IsNull());
	};

	//! Destructor
	virtual ~SketchSegmentLine() = default;

	virtual int StartPoint() const override { return PointsAttribute()->Value(0); }
	virtual int EndPoint() const override { return PointsAttribute()->Value(1); }

	virtual Handle(SketchSegment) Clone() const
	{
		auto newLabel = DocumentTool::SketchSegmentLinesLabel().NewChild();
		DocumentTool::CopyLabel(_Label, newLabel);
		return new SketchSegmentLine(newLabel);
	};

	TDF_Label Label() const { return _Label; }

    gp_Lin2d GetLine(const std::map<int, gp_Pnt2d>& points)
    {
		auto p1 = points.at(StartPoint());
        auto p2 = points.at(EndPoint());

        if(p1.Distance(p2) <= 0)
			return {};

        return gce_MakeLin2d(p1, p2).Value();
    }

	virtual Handle(Geom2d_Curve) MakeCurve(const std::map<int, gp_Pnt2d>& points) override
	{
		auto p1 = points.at(StartPoint());
		auto p2 = points.at(EndPoint());

		if(p1.Distance(p2) <= 0)
			return {};

		auto line = GetLine(points);
		return new Geom2d_TrimmedCurve(new Geom2d_Line(line), ElCLib::Parameter(line, p1), ElCLib::Parameter(line, p2));
	}

	double Length(const std::map<int, gp_Pnt2d>& points)
	{
		auto p1 = points.at(StartPoint());
		auto p2 = points.at(EndPoint());

		if(p1.Distance(p2) <= 0)
			return 0.0;

		return p1.Distance(p2);
	}
private:
	TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchSegmentLine, SketchSegment)

#endif  // _SketchSegmentLine_Header_File_
