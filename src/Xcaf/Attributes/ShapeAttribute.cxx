
#include "Xcaf/Attributes/ShapeAttribute.hxx"

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>
#include <TDataXtd_Shape.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(ShapeAttribute, TDataStd_GenericEmpty, "xcaf", "ShapeAttribute")

//=================================================================================================

TDF_Label ShapeAttribute::BaseLabel() const
{
    return Label();
}

void ShapeAttribute::Init()
{}

const Standard_GUID& ShapeAttribute::GetID()
{
    static Standard_GUID ShapeAttributeID("e47b7ac8-bc85-6818-e0d9-bb863997f154");
    return ShapeAttributeID;
}

//=================================================================================================

Handle(ShapeAttribute) ShapeAttribute::Set(const TDF_Label& L)
{
    Handle(ShapeAttribute) A;
    if(!L.FindAttribute(ShapeAttribute::GetID(), A))
    {
        A = new ShapeAttribute();
        L.AddAttribute(A);
    }
    A->Init();

	TDataXtd_Shape::New(L.FindChild(1));
    TDataXtd_Shape::New(L.FindChild(2));
    return A;
}

//=================================================================================================

ShapeAttribute::ShapeAttribute()
{}

//=================================================================================================

const Standard_GUID& ShapeAttribute::ID() const
{
    return GetID();
}
