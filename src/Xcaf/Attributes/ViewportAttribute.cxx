
#include "Xcaf/Attributes/ViewportAttribute.hxx"

#include <AIS_InteractiveContext.hxx>
#include <Standard_Type.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <V3d_Viewer.hxx>

#include "Core/Document.hxx"

IMPLEMENT_STANDARD_RTTIEXT(ViewportAttribute, TDF_Attribute)

//=================================================================================================

const Standard_GUID& ViewportAttribute::GetID()
{
    static Standard_GUID ViewportAttributeID("7daf58e9-8569-92c5-bbac-8d85c97d49c4");
    return ViewportAttributeID;
}

//=================================================================================================

Handle(ViewportAttribute) ViewportAttribute::Set(const TDF_Label& aLabel,
                                                   const Handle(Viewport)& IC)
{

    Handle(ViewportAttribute) aAttr;
    aLabel.FindAttribute(ViewportAttribute::GetID(), aAttr);
    if(aAttr.IsNull())
    {
        aAttr = new ViewportAttribute();
        aLabel.AddAttribute(aAttr);
    }

    aAttr->Init();

    aAttr->SetViewport(IC);

    return aAttr;
}

//=================================================================================================

void ViewportAttribute::SetViewport(const Handle(Viewport)& Viewport)
{
    auto aDocument = Handle(Document)::DownCast(TDocStd_Document::Get(Label()));
    aDocument->RegisteredTable()[Label()] = Viewport;
}

Handle(ViewportAttribute) ViewportAttribute::New(const TDF_Label& acces,
                                                   const Handle(Viewport)& IC)
{
    Handle(ViewportAttribute) V;
    if(acces.FindAttribute(ViewportAttribute::GetID(), V))
    {
        throw Standard_DomainError("already done");
    }
    V = new ViewportAttribute();
    V->SetViewport(IC);
    acces.AddAttribute(V);

    return V;
}

void ViewportAttribute::Init()
{
    Handle(TDF_Attribute) aAttr;
	if(!EyePointLabel().FindAttribute(TDataXtd_Point::GetID(), aAttr))
	{
		TDataXtd_Point::Set(EyePointLabel(), gp_Pnt());
	}

	if(!TargetPointsLabel().FindAttribute(TDataXtd_Point::GetID(), aAttr))
	{
		TDataXtd_Point::Set(TargetPointsLabel(), gp_Pnt());
	}

	if(!TwistLabel().FindAttribute(TDataStd_Real::GetID(), aAttr))
	{
		TDataStd_Real::Set(TwistLabel(), 0.);
	}

	if(!ScaleLabel().FindAttribute(TDataStd_Real::GetID(), aAttr))
	{
		TDataStd_Real::Set(ScaleLabel(), 1.);
	}

	if(!RenderModeLabel().FindAttribute(TDataStd_Integer::GetID(), aAttr))
	{
		TDataStd_Integer::Set(RenderModeLabel(), 0);
	}
}

//=================================================================================================

Standard_Boolean ViewportAttribute::Find(const TDF_Label& acces, Handle(ViewportAttribute)& V)
{
    return (acces.FindAttribute(ViewportAttribute::GetID(), V));
}

//=================================================================================================

Standard_Boolean ViewportAttribute::Find(const TDF_Label& acces, Handle(Viewport)& IC)
{
    Handle(ViewportAttribute) V;
    if(acces.FindAttribute(ViewportAttribute::GetID(), V))
    {
        IC = V->GetViewport();
        return Standard_True;
    }
    return Standard_False;
}

//=================================================================================================

Standard_Boolean ViewportAttribute::Has(const TDF_Label& acces)
{
    return (acces.IsAttribute(ViewportAttribute::GetID()));
}

//=================================================================================================

ViewportAttribute::ViewportAttribute() {}

//=================================================================================================

Handle(Viewport) ViewportAttribute::GetViewport() const
{
    return Document::Get<Viewport>(Label());
}

//=================================================================================================

const Standard_GUID& ViewportAttribute::ID() const
{
    return GetID();
}

//=================================================================================================

void ViewportAttribute::Restore(const Handle(TDF_Attribute)&) {}

//=================================================================================================

Handle(TDF_Attribute) ViewportAttribute::NewEmpty() const
{
    return new ViewportAttribute();
}

//=================================================================================================

void ViewportAttribute::Paste(const Handle(TDF_Attribute)&,
                               const Handle(TDF_RelocationTable)&) const
{}

//=================================================================================================

void ViewportAttribute::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

        OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)
}
