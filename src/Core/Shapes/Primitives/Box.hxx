//! class Box

#ifndef _Box_Header_File_
#define _Box_Header_File_

#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>

#include "Core/Shapes/Shape.hxx"

class BoxAttribute;

class Box : public Shape 
{
	using Attribute = BoxAttribute;

public:
	Box(const TDF_Label& label);

private:
	friend BoxAttribute;

	Handle(TDataStd_Real) myX;
	Handle(TDataStd_Real) myY;
	Handle(TDataStd_Real) myZ;
};


class NewBox : public Shape
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Name, Shape)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Real, X)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Real, Y)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real, Z)
public:
	NewBox(const TDF_Label& label )
		:Shape(label.FindChild(1))
	{
		myLabel = label;
	}

	TDF_Label Label() const
	{
		return myLabel;
	}

	bool MakeInternal()
	{
		TDataStd_Real::Set(XLabel(), 111.0);
		TDataStd_Real::Set(YLabel(), 111.0);
		TDataStd_Real::Set(ZLabel(), 111.0);

		TopoDS_Shape aBox = BRepPrimAPI_MakeBox(XAttribute()->Get(), YAttribute()->Get(), ZAttribute()->Get()).Shape();

		gp_Trsf aLoc;
		aLoc.SetTranslation(gp_Vec(XAttribute()->Get(), YAttribute()->Get(), ZAttribute()->Get()));
		aBox.Move(aLoc);
		// Build a TNaming_NamedShape using built box

		Shape::SetBRep(aBox);

		return true;
	}
private:
	TDF_Label myLabel;
};

DEFINE_STANDARD_HANDLE(NewBox, Shape)

DEFINE_STANDARD_HANDLE(Box, Shape)

#endif  // _Box_Header_File_
