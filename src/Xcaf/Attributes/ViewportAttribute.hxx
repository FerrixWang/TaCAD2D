
#ifndef _ViewportAttribute_HeaderFile
#define _ViewportAttribute_HeaderFile

#include <Standard.hxx>
#include <TDataXtd_Point.hxx>
#include <Standard_Type.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDataStd_Integer.hxx>

#include "Core/Viewport.hxx"
#include "Xcaf/Attributes/WorkingContextAttribute.hxx"

class ViewportAttribute : public TDF_Attribute
{
public:
    DEFINE_ATTRIBUTE_ON_CHILD(1, TDataXtd_Point, EyePoint)
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataXtd_Point, TargetPoints)
    DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Real, Twist)
    DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real, Scale)
	DEFINE_ATTRIBUTE_ON_CHILD(5, TDataStd_Integer, RenderMode)

public:
    //! class methods
    //! =============
    static const Standard_GUID& GetID();

    static Standard_Boolean Has(const TDF_Label& acces);

    static Handle(ViewportAttribute) Set(const TDF_Label& acces, const Handle(Viewport)& IC);

    static Handle(ViewportAttribute) New(const TDF_Label& acces,
                                          const Handle(Viewport)& viewer);
    void Init();

    static Standard_Boolean Find(const TDF_Label& acces,
                                 Handle(ViewportAttribute)& A);

    static Standard_Boolean Find(const TDF_Label& acces, Handle(Viewport)& V);

    ViewportAttribute();

    void SetViewport(const Handle(Viewport)& Viewport);

    Handle(Viewport) GetViewport() const;

    const Standard_GUID& ID() const Standard_OVERRIDE;

    void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

    Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

    void Paste(const Handle(TDF_Attribute)& into,
               const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

    virtual void DumpJson(Standard_OStream& theOStream,
                          Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(ViewportAttribute, TDF_Attribute)

protected:
};

DEFINE_STANDARD_HANDLE(ViewportAttribute, TDF_Attribute)

#endif // _ViewportAttribute_HeaderFile
