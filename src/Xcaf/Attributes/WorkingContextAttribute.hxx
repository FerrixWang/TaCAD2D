
#ifndef _WorkingContextAttribute_HeaderFile
#define _WorkingContextAttribute_HeaderFile

#include <Standard.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

#include "Core/Workspace.hxx"

class WorkingContextAttribute : public TDataStd_GenericEmpty
{
public:
    DEFINE_ATTRIBUTE_ON_CHILD(1, TDataXtd_Plane  , WorkingPlane)
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Real   , GridStep)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Integer, GridType)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real   , GridRotation)
	DEFINE_ATTRIBUTE_ON_CHILD(5, TDataStd_Integer, GridDivisions)

public:
    //! class methods
    //! =============
    static const Standard_GUID& GetID();

    static Standard_Boolean Has(const TDF_Label& acces);

    static Handle(WorkingContextAttribute) Set(const TDF_Label& acces);

    static Handle(WorkingContextAttribute) Set(const TDF_Label& aLabel, 
                                        const Handle(WorkingContext)& value);

    void SetWorkingContext(const Handle(WorkingContext)& value);

    Handle(WorkingContext) GetWorkingContext();

    void Init();

    static Standard_Boolean Find(const TDF_Label& acces,
                                 Handle(WorkingContextAttribute)& A);

    WorkingContextAttribute();

    void Restore(const Handle(WorkingContextAttribute)& with);
     
    const Standard_GUID& ID() const Standard_OVERRIDE;

    void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

    Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

    void Paste(const Handle(TDF_Attribute)& into,
               const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

    virtual void DumpJson(Standard_OStream& theOStream,
                          Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(WorkingContextAttribute, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(WorkingContextAttribute, TDataStd_GenericEmpty)

#endif // _WorkingContextAttribute_HeaderFile
