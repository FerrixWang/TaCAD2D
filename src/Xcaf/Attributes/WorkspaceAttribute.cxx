
#include "Xcaf/Attributes/WorkspaceAttribute.hxx"

#include <AIS_InteractiveContext.hxx>
#include <Standard_Type.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <V3d_Viewer.hxx>

#include "Core/Document.hxx"

IMPLEMENT_STANDARD_RTTIEXT(WorkspaceAttribute, TDataStd_GenericEmpty)

//=================================================================================================

const Standard_GUID& WorkspaceAttribute::GetID()
{
    static Standard_GUID WorkspaceAttributeID("7daf58e9-8569-92c5-bbac-8d85c97d49c4");
    return WorkspaceAttributeID;
}

//=================================================================================================

Handle(WorkspaceAttribute) WorkspaceAttribute::Set(const TDF_Label& aLabel,
                                                   const Handle(Workspace)& IC)
{

    Handle(WorkspaceAttribute) aAttr;
    aLabel.FindAttribute(WorkspaceAttribute::GetID(), aAttr);
    if(aAttr.IsNull())
    {
        aAttr = new WorkspaceAttribute();
        aLabel.AddAttribute(aAttr);
    }

    aAttr->Init();

    aAttr->SetWorkspace(IC);
    
    return aAttr;
}

//=================================================================================================

void WorkspaceAttribute::SetWorkspace(const Handle(Workspace)& workspace)
{
    auto aDocument = Handle(Document)::DownCast(TDocStd_Document::Get(Label()));
    aDocument->RegisteredTable()[Label()] = workspace;
}

Handle(WorkspaceAttribute) WorkspaceAttribute::New(const TDF_Label& acces,
                                                 const Handle(Workspace)& IC)
{
    Handle(WorkspaceAttribute) V;
    if(acces.FindAttribute(WorkspaceAttribute::GetID(), V))
    {
        throw Standard_DomainError("already done");
    }
    V = new WorkspaceAttribute();
    V->SetWorkspace(IC);
    acces.AddAttribute(V);

    return V;
}

void WorkspaceAttribute::Init() 
{
	Handle(TDF_Attribute) aAttr;
	if(!GridEnabledLabel().FindAttribute(TDataStd_Integer::GetID(), aAttr))
	{
		TDataStd_Integer::Set(GridEnabledLabel(), 1);
	}

	if(!GlobalWorkingContextLabel().FindAttribute(WorkingContextAttribute::GetID(), aAttr))
	{
		WorkingContextAttribute::Set(GlobalWorkingContextLabel());
	}

	if(!CurrentWorkingContextLabel().FindAttribute(WorkingContextAttribute::GetID(), aAttr))
	{
		WorkingContextAttribute::Set(CurrentWorkingContextLabel());
	}
} 

//=================================================================================================

Standard_Boolean WorkspaceAttribute::Find(const TDF_Label& acces, Handle(WorkspaceAttribute)& V)
{
    return (acces.FindAttribute(WorkspaceAttribute::GetID(), V));
}

//=================================================================================================

Standard_Boolean WorkspaceAttribute::Find(const TDF_Label& acces, Handle(Workspace)& IC)
{
    Handle(WorkspaceAttribute) V;
    if(acces.FindAttribute(WorkspaceAttribute::GetID(), V))
    {
        IC = V->GetWorkspace();
        return Standard_True;
    }
    return Standard_False;
}

//=================================================================================================

Standard_Boolean WorkspaceAttribute::Has(const TDF_Label& acces)
{
    return (acces.IsAttribute(WorkspaceAttribute::GetID()));
}

//=================================================================================================

void WorkspaceAttribute::Update(const TDF_Label& acces)
{
    Handle(WorkspaceAttribute) viewer;
    if(Find(acces, viewer))
        viewer->Update();
}

//=================================================================================================

WorkspaceAttribute::WorkspaceAttribute() {}

//=================================================================================================

void WorkspaceAttribute::Update() const
{
    GetWorkspace()->UpdateCurrentViewer();
}

//=================================================================================================

Handle(Workspace) WorkspaceAttribute::GetWorkspace() const
{
    return Document::Get<Workspace>(Label());
}

//=================================================================================================

const Standard_GUID& WorkspaceAttribute::ID() const
{
    return GetID();
}

//=================================================================================================

void WorkspaceAttribute::Restore(const Handle(TDF_Attribute)&) {}

//=================================================================================================

Handle(TDF_Attribute) WorkspaceAttribute::NewEmpty() const
{
    return new WorkspaceAttribute();
}

//=================================================================================================

void WorkspaceAttribute::Paste(const Handle(TDF_Attribute)&,
                              const Handle(TDF_RelocationTable)&) const
{}

//=================================================================================================

void WorkspaceAttribute::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

        OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDataStd_GenericEmpty)
}
