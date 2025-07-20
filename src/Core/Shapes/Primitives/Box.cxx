#include "Core/Shapes/Primitives/Box.hxx"

#include <TDataStd_Real.hxx>

#include "Xcaf/Attributes/BoxAttribute.hxx"

Box::Box(const TDF_Label& label)
	: Shape(label)
{
	BoxAttribute::Set(label, this);
}
