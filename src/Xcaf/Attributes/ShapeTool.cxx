
#include "Xcaf/Attributes/ShapeTool.hxx"

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>

#include "Xcaf/Attributes/BoxTool.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(ShapeTool, TDataStd_GenericEmpty, "xcaf", "ShapeTool")

//=================================================================================================

TDF_Label ShapeTool::BaseLabel() const
{
    return Label();
}

TDF_Label ShapeTool::ShapesLabel(const TDF_Label& acces)
{
    return DocumentTool::ShapesLabel(acces);
}

TDF_Label ShapeTool::BoxsLabel(const TDF_Label& acces)
{
    TDF_Label L = ShapesLabel(acces).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "Boxs");
    return L;
}

void ShapeTool::Init()
{
}

const Standard_GUID& ShapeTool::GetID()
{
    static Standard_GUID ShapeToolID("7daf58e9-8569-92c5-bbdc-8d85c97d39c4");
    return ShapeToolID;
}

//=================================================================================================

Handle(ShapeTool) ShapeTool::Set(const TDF_Label& L)
{
    Handle(ShapeTool) A;
    if(!L.FindAttribute(ShapeTool::GetID(), A))
    {
        A = new ShapeTool();
        L.AddAttribute(A);
    }
    A->Init();

    BoxTool::Set(BoxsLabel(L));

    return A;
}

//=================================================================================================

ShapeTool::ShapeTool()
{}

//=================================================================================================

const Standard_GUID& ShapeTool::ID() const
{
    return GetID();
}
