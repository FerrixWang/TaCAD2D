#ifndef SketchElement_HXX
#define SketchElement_HXX

#include <map>

#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Core/Shapes/Sketch/SketchFactory.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

class SketchElement : public Standard_Transient
{
	DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
	DEFINE_ATTRIBUTE_ON_LABEL(TDF_Reference, Reference)
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataXtd_Shape, HintShape)
	DEFINE_ATTRIBUTE_ON_CHILD(1, TPrsStd_AISPresentation, HintPresent)

public:
	SketchElement(const TDF_Label& label)
		: _Label(label)
	{
		if(NameAttribute().IsNull())
		{
			NameAttribute()->Set(_Label, "SketchElement");
		}
		if(ReferenceAttribute().IsNull())
		{
			ReferenceAttribute()->Set(_Label, TDF_Label());
		}
		if(HintShapeAttribute().IsNull())
		{
			HintShapeAttribute()->Set(HintShapeLabel(), {});
		}
		if(HintPresentAttribute().IsNull())
		{
			HintPresentAttribute()->Set(HintPresentLabel(), TNaming_NamedShape::GetID());
			HintPresentAttribute()->SetWidth(6);
			HintPresentAttribute()->SetColor(Quantity_NOC_GRAY);
			HintPresentAttribute()->SetSelectionMode(-1);
			HintPresentAttribute()->SetTransparency(0.9);
		}
	}

	~SketchElement() override = default;

	virtual TDF_Label InstanceLabel() const { return _Label.Father(); }
	virtual void ForgetAllAttributes() const { InstanceLabel().ForgetAllAttributes(true); }
	virtual void OnPointsChanged(std::map<int, gp_Pnt2d> points, 
								 std::map<int, Handle(SketchSegment)> segments) {}

	Handle(Prs3d_LineAspect) HintAspect()
	{
		return new Prs3d_LineAspect(Quantity_Color(Quantity_NOC_GRAY), Aspect_TypeOfLine::Aspect_TOL_DASH, 1.0);
	}

	TDF_Label Label() const { return _Label; }

private:
	TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchElement, Standard_Transient)

#endif // SketchElement_HXX
