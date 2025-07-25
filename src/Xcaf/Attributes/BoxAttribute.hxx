
#ifndef _BoxAttribute_HeaderFile
#define _BoxAttribute_HeaderFile

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Primitives/Box.hxx"
#include "Xcaf/Attributes/ShapeAttribute.hxx"

class BoxAttribute : public TDataStd_GenericEmpty
{
    DEFINE_ATTRIBUTE_ON_CHILD(1, ShapeAttribute, Shapee)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Real, X)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Real, Y)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real, Z)

public:
    static const Standard_GUID& GetID();

    static Handle<BoxAttribute> Set(const TDF_Label& L, const Handle<Box>& S);

    void SetBox(const Handle<Box>& workspace);

    BoxAttribute();

    //! set hasComponents into false
    void Init();

    TopoDS_Shape GetShape();

    const Standard_GUID& ID() const override;

    DEFINE_DERIVED_ATTRIBUTE(BoxAttribute, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(BoxAttribute, TDataStd_GenericEmpty)

#endif // _BoxAttribute_HeaderFile
