
#include "Xcaf/Attributes/InstanceAttribute.hxx"

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>
#include <TDataXtd_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(InstanceAttribute, TDF_Attribute)

//=================================================================================================

void InstanceAttribute::Init()
{}

const Standard_GUID& InstanceAttribute::GetID()
{
    static Standard_GUID InstanceAttributeID("e47b7ac8-bc85-6818-e019-bb863997f154");
    return InstanceAttributeID;
}

//=================================================================================================

Handle(InstanceAttribute) InstanceAttribute::Set(const TDF_Label& L, const Handle(Standard_Transient)& instance)
{
    Handle(InstanceAttribute) A;
    if(!L.FindAttribute(InstanceAttribute::GetID(), A))
    {
        A = new InstanceAttribute();
        L.AddAttribute(A);
    }
    A->Init();

	A->myInstance = instance;
    return A;
}

//=================================================================================================

InstanceAttribute::InstanceAttribute()
{}

//=================================================================================================

const Standard_GUID& InstanceAttribute::ID() const
{
    return GetID();
}

Handle(TDF_Attribute) InstanceAttribute::NewEmpty() const
{
    return new InstanceAttribute();
}

//=================================================================================================

void InstanceAttribute::Restore(const Handle(TDF_Attribute)& /*theWith*/)
{
    myInstance.Nullify();
}

//=================================================================================================

void InstanceAttribute::Paste(const Handle(TDF_Attribute)& theInto,
                                    const Handle(TDF_RelocationTable)&) const
{
}
