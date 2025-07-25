
//! class SketchSegment

#ifndef SketchSegment_Header_File_
#define SketchSegment_Header_File_

#include <BRepBuilderAPI_MakeEdge2d.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Standard_Transient.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Wire.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchSegment : public Standard_Transient
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
	DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_IntegerArray, Points)
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Integer, IsAuxilliary)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataXtd_Shape, Shape)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TPrsStd_AISPresentation, Present)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataXtd_Shape, HintShape)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TPrsStd_AISPresentation, HintPresent)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Name, Element)

public:
	//! Constructor
	SketchSegment(const TDF_Label& label)
		: _Label(label)
	{
		if(NameAttribute().IsNull())
			NameAttribute()->Set(_Label, "SketchSegment");

		if(PointsAttribute().IsNull())
		{
			PointsAttribute()->Set(PointsLabel(), 0, 1);
			PointsAttribute()->SetValue(0, -1);
			PointsAttribute()->SetValue(1, -1);
		}
		if(IsAuxilliaryAttribute().IsNull())
			IsAuxilliaryAttribute()->Set(IsAuxilliaryLabel(), 0);

		//if(ShapeAttribute().IsNull())
		//	ShapeAttribute()->Set(ShapeLabel(), {});

		//if(PresentAttribute().IsNull())
		//{
		//	PresentAttribute()->Set(PresentLabel(), TNaming_NamedShape::GetID());
		//}

		//if(HintShapeAttribute().IsNull())
		//	HintShapeAttribute()->Set(HintShapeLabel(), {});

		//if(HintPresentAttribute().IsNull())
		//{
		//	HintPresentAttribute()->Set(HintPresentLabel(), TNaming_NamedShape::GetID());
		//	HintPresentAttribute()->SetWidth(6);
		//	HintPresentAttribute()->SetColor(Quantity_NOC_GRAY);
		//	HintPresentAttribute()->SetSelectionMode(-1);
		//	HintPresentAttribute()->SetTransparency(0.9);
		//}
	};

	//! Destructor
	virtual ~SketchSegment() = default;

	virtual TDF_Label InstanceLabel() const { return _Label.Father(); };
	virtual int StartPoint() const { return -1; }
	virtual int EndPoint() const { return -1; }
	virtual Handle<Geom2d_Curve> MakeCurve(const std::map<int, gp_Pnt2d>& points) = 0;
	virtual TopoDS_Wire CreateHintEdge(const std::map<int, gp_Pnt2d>& points) { return {}; }

	virtual Handle<SketchSegment> Clone(
		const TDF_Label& label = DocumentTool::TrashLabel().NewChild()) const;

	TDF_Label Label() const { return _Label; }

	void ForgetAllAttributes() { InstanceLabel().ForgetAllAttributes(true); }
	bool IsPeriodic() const { return StartPoint() == -1 && EndPoint() == -1; }
	void Invalidate() { CachedCurve.Nullify(); }

	int Point(int index) const { return PointsAttribute()->Value(index); }

	Handle<Prs3d_LineAspect> HintAspect()
	{
		return new Prs3d_LineAspect(Quantity_Color(Quantity_NOC_GRAY), Aspect_TypeOfLine::Aspect_TOL_DASH, 1.0);
	}

	bool IsConnected(const Handle<SketchSegment>& other)
	{
		int s1 = StartPoint();
		int s2 = other->StartPoint();
		int e1 = EndPoint();
		int e2 = other->EndPoint();
		return ((s1 != -1) && ((s1 == e2) || (s1 == s2)))
			|| ((e1 != -1) && ((e1 == e2) || (e1 == s2)));
	}

	TopoDS_Edge MakeEdge(const std::map<int, gp_Pnt2d>& points)
	{
		auto curve = MakeCurve(points);
		if(curve.IsNull()) return {};
		
		CachedCurve = curve;
		//return new BRepBuilderAPI_MakeEdge(GeomAPI.To3d(curve, Pln.XOY)).Edge();
		return BRepBuilderAPI_MakeEdge2d(curve).Edge();
	}

	// Variadic template method to create a polyline edge for HintShape
	template<typename... Points>
	TopoDS_Wire MakeHintEdge(const Points&... points) const
	{
		BRepBuilderAPI_MakePolygon poly;
		(poly.Add(gp_Pnt(points.X(), points.Y(), 0.0)), ...); // Fold expression to add points

		if(!poly.IsDone()) return {};
		return poly.Wire();
	}

private:
	TDF_Label _Label;
	Handle<Geom2d_Curve> CachedCurve;
};

DEFINE_STANDARD_HANDLE(SketchSegment, Standard_Transient)

#endif  // _SketchSegment_Header_File_
