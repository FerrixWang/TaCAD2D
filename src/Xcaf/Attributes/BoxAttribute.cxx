
#include "Xcaf/Attributes/BoxAttribute.hxx"

#include <Standard_GUID.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc_ShapeTool.hxx>

#include "Core/Shapes/Primitives/Box.hxx"
#include "Xcaf/Attributes/ShapeAttribute.hxx"
#include "Core/Document.hxx"

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(BoxAttribute, TDataStd_GenericEmpty, "xcaf", "BoxAttribute")

//=================================================================================================

void BoxAttribute::Init()
{
    ShapeAttribute::Set(ShapeeLabel());
    TDataStd_Real::Set(XLabel(), 111.0);
	TDataStd_Real::Set(YLabel(), 111.0);
	TDataStd_Real::Set(ZLabel(), 111.0);

    TopoDS_Shape aBox = BRepPrimAPI_MakeBox(XAttribute()->Get(), YAttribute()->Get(), ZAttribute()->Get()).Shape();

    gp_Trsf aLoc;
    aLoc.SetTranslation(gp_Vec(XAttribute()->Get(), YAttribute()->Get(), ZAttribute()->Get()));
    aBox.Move(aLoc);
    // Build a TNaming_NamedShape using built box

    TNaming_Builder B(Label());
    B.Generated(aBox);
}

TopoDS_Shape BoxAttribute::GetShape()
{
	return XCAFDoc_ShapeTool::GetShape(Label());
}

const Standard_GUID& BoxAttribute::GetID()
{
    static Standard_GUID BoxAttributeID("d019aa6d-4ece-2bd4-93ac-63c55ab66860");
    return BoxAttributeID;
}

Handle(BoxAttribute) BoxAttribute::Set(const TDF_Label& L, const Handle(Box)& S)
{
    Handle(BoxAttribute) A;
    if(!L.FindAttribute(BoxAttribute::GetID(), A))
    {
        A = new BoxAttribute();
        L.AddAttribute(A);
    }
    A->Init();

    
    return A;
}

void BoxAttribute::SetBox(const Handle(Box)& theBox)
{
    auto aDocument = Handle(Document)::DownCast(TDocStd_Document::Get(Label()));
    aDocument->RegisteredTable()[Label()] = theBox;

    theBox->myX = XAttribute();
    theBox->myY = YAttribute();
    theBox->myX = ZAttribute();
}

//=================================================================================================

BoxAttribute::BoxAttribute()
{}

//=================================================================================================

const Standard_GUID& BoxAttribute::ID() const
{
    return GetID();
}
