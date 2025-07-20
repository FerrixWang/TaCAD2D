
#include "Xcaf/Attributes/BoxTool.hxx"

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>

#include "Xcaf/Attributes/ShapeTool.hxx"
#include "Core/Shapes/Primitives/Box.hxx"

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(BoxTool, TDataStd_GenericEmpty, "xcaf", "BoxTool")

//=================================================================================================

TDF_Label BoxTool::BaseLabel() const
{
    return Label();
}

void BoxTool::Init()
{}

const Standard_GUID& BoxTool::GetID()
{
    static Standard_GUID BoxToolID("7675db5f-9c74-17ce-9d05-ae49d3447c10");
    return BoxToolID;
}

Handle(BoxTool) BoxTool::Set(const TDF_Label& L)
{
    Handle(BoxTool) A;
    if(!L.FindAttribute(BoxTool::GetID(), A))
    {
        A = new BoxTool();
        L.AddAttribute(A);
    }
    A->Init();

    return A;
}

TDF_Label BoxTool::BoxsLabel(const TDF_Label& acces)
{
    return ShapeTool::BoxsLabel(acces);
}

//=================================================================================================

BoxTool::BoxTool()
{}

//=================================================================================================

void BoxTool::AddBox(const TDF_Label& L, const Handle(Box)& S)
{

    Handle(BoxAttribute) boxAttr = GetBoxAttribute(L);
    if(boxAttr.IsNull())
    {
        boxAttr = BoxAttribute::Set(L, S);
    }

}

const Standard_GUID& BoxTool::ID() const
{
    return GetID();
}
