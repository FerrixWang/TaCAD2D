//! class Shape2D

#ifndef Shape2D_Header_File_
#define Shape2D_Header_File_

#include <TDataStd_Name.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Reference.hxx>

#include "Core/Shapes/Shape.hxx"

class Shape2D : public Shape
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Name, Shape)

public:
	//! Constructor
	Shape2D(const TDF_Label& label)
		: Shape(label.FindChild(1))
	{};

	//! Destructor
	virtual ~Shape2D() = default;

private:
	TDF_Label myLabel;
};

DEFINE_STANDARD_HANDLE(Shape2D, Shape)

#endif  // _Shape2D_Header_File_
