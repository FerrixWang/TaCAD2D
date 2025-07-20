
#include "Xcaf/Attributes/WorkingContextAttribute.hxx"

#include <AIS_InteractiveContext.hxx>
#include <Standard_Type.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <V3d_Viewer.hxx>
#include <TDataXtd_Geometry.hxx>

#include "Core/Document.hxx"
#include "Core/Project/WorkingContext.hxx"

IMPLEMENT_STANDARD_RTTIEXT(WorkingContextAttribute, TDataStd_GenericEmpty)

//=================================================================================================

const Standard_GUID& WorkingContextAttribute::GetID()
{
    static Standard_GUID WorkingContextAttributeID("7daf51e9-8539-92c5-bbac-8d85c97d49c4");
    return WorkingContextAttributeID;
}

//=================================================================================================

Handle(WorkingContextAttribute) WorkingContextAttribute::Set(const TDF_Label& aLabel)
{
    Handle(WorkingContextAttribute) aAttr;
    aLabel.FindAttribute(WorkingContextAttribute::GetID(), aAttr);
    if(aAttr.IsNull())
    {
        aAttr = new WorkingContextAttribute();
        aLabel.AddAttribute(aAttr);
    }

    aAttr->Init();

    return aAttr;
}

Handle(WorkingContextAttribute) WorkingContextAttribute::Set(const TDF_Label& aLabel, const Handle(WorkingContext)& value)
{
    Handle(WorkingContextAttribute) aAttr;
    aLabel.FindAttribute(WorkingContextAttribute::GetID(), aAttr);
    if(aAttr.IsNull())
    {
        aAttr = new WorkingContextAttribute();
        aLabel.AddAttribute(aAttr);
    }

    aAttr->Init();
    aAttr->SetWorkingContext(value);
    
    return aAttr;
}

void WorkingContextAttribute::SetWorkingContext(const Handle(WorkingContext)& value)
{
    auto aDocument = Handle(Document)::DownCast(TDocStd_Document::Get(Label()));
    aDocument->RegisteredTable()[Label()] = value;
}

Handle(WorkingContext) WorkingContextAttribute::GetWorkingContext()
{
     return Document::Get<WorkingContext>(Label());
}

void WorkingContextAttribute::Init()
{
    Handle(TDF_Attribute) aAttr;
	if(!WorkingPlaneLabel().FindAttribute(TDataXtd_Plane::GetID(), aAttr))
	{
        TDataXtd_Plane::Set(WorkingPlaneLabel(), gp_Pln(gp::XOY()));
	}

	if(!GridStepLabel().FindAttribute(TDataStd_Real::GetID(), aAttr))
	{
		TDataStd_Real::Set(GridStepLabel(), 1.0);
	}

	if(!GridTypeLabel().FindAttribute(TDataStd_Integer::GetID(), aAttr))
	{
		TDataStd_Integer::Set(GridTypeLabel(), 0);
	}

	if(!GridRotationLabel().FindAttribute(TDataStd_Real::GetID(), aAttr))
	{
		TDataStd_Real::Set(GridRotationLabel(), 0.0);
	}

	if(!GridDivisionsLabel().FindAttribute(TDataStd_Integer::GetID(), aAttr))
	{
		TDataStd_Integer::Set(GridDivisionsLabel(), 8);
	}
}

//=================================================================================================

Standard_Boolean WorkingContextAttribute::Find(const TDF_Label& acces, Handle(WorkingContextAttribute)& V)
{
    return (acces.FindAttribute(WorkingContextAttribute::GetID(), V));
}


//=================================================================================================

Standard_Boolean WorkingContextAttribute::Has(const TDF_Label& acces)
{
    return (acces.IsAttribute(WorkingContextAttribute::GetID()));
}


//=================================================================================================

WorkingContextAttribute::WorkingContextAttribute()
{
}


//=================================================================================================

const Standard_GUID& WorkingContextAttribute::ID() const
{
    return GetID();
}

void WorkingContextAttribute::Restore(const Handle(WorkingContextAttribute)& with)
{
	gp_Pln Pln;
    TDataXtd_Geometry::Plane(with->Label(), Pln);
	// Restore the attributes from the given WorkingContextAttribute
	TDataXtd_Plane::Set(WorkingPlaneLabel(), Pln);
	TDataStd_Real::Set(GridStepLabel(), with->GridStepAttribute()->Get());
	TDataStd_Integer::Set(GridTypeLabel(), with->GridTypeAttribute()->Get());
	TDataStd_Real::Set(GridRotationLabel(), with->GridRotationAttribute()->Get());
	TDataStd_Integer::Set(GridDivisionsLabel(), with->GridDivisionsAttribute()->Get());
}

//=================================================================================================

void WorkingContextAttribute::Restore(const Handle(TDF_Attribute)& with)
{
}

//=================================================================================================

Handle(TDF_Attribute) WorkingContextAttribute::NewEmpty() const
{
    return new WorkingContextAttribute();
}

//=================================================================================================

void WorkingContextAttribute::Paste(const Handle(TDF_Attribute)&,
                               const Handle(TDF_RelocationTable)&) const
{}

//=================================================================================================

void WorkingContextAttribute::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

        OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDataStd_GenericEmpty)

}
