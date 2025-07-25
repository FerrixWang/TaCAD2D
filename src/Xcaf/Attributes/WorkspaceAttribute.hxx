
#ifndef _WorkspaceAttribute_HeaderFile
#define _WorkspaceAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

#include "Core/Workspace.hxx"
#include "Xcaf/Attributes/WorkingContextAttribute.hxx"

class WorkspaceAttribute : public TDataStd_GenericEmpty
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Integer, GridEnabled)
    DEFINE_ATTRIBUTE_ON_CHILD(2, WorkingContextAttribute, GlobalWorkingContext)
    DEFINE_ATTRIBUTE_ON_CHILD(3, WorkingContextAttribute, CurrentWorkingContext)

public:
    //! class methods
    //! =============
     static const Standard_GUID& GetID();

     static Standard_Boolean Has(const TDF_Label& acces);

     static Handle(WorkspaceAttribute) Set(const TDF_Label& acces, const Handle(Workspace)& IC);

     static Handle(WorkspaceAttribute) New(const TDF_Label& acces,
                                                         const Handle(Workspace)& viewer);
     void Init();

     static Standard_Boolean Find(const TDF_Label& acces,
                                                 Handle(WorkspaceAttribute)& A);

     static Standard_Boolean Find(const TDF_Label& acces, Handle(Workspace)& V);

     static void Update(const TDF_Label& acces);

     WorkspaceAttribute();

     void Update() const;

     void SetWorkspace(const Handle(Workspace)& workspace);

     Handle(Workspace) GetWorkspace() const;

     const Standard_GUID& ID() const Standard_OVERRIDE;

     void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

     Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

     void Paste(const Handle(TDF_Attribute)& into,
                               const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

     virtual void DumpJson(Standard_OStream& theOStream,
                                          Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(WorkspaceAttribute, TDataStd_GenericEmpty)

protected:
};

DEFINE_STANDARD_HANDLE(WorkspaceAttribute, TDataStd_GenericEmpty)

#endif // _WorkspaceAttribute_HeaderFile
