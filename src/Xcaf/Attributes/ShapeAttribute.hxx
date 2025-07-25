
#ifndef _ShapeAttribute_HeaderFile
#define _ShapeAttribute_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>

class ShapeAttribute : public TDataStd_GenericEmpty
{
public:
    static const Standard_GUID& GetID();

    //! Create (if not exist) ShapeAttribute from XCAFDoc on <L>.
    static Handle<ShapeAttribute> Set(const TDF_Label& L);

    //! Creates an empty tool
    //! Creates a tool to work with a document <Doc>
    //! Attaches to label XCAFDoc::LabelShapes()
    ShapeAttribute();

    //! set hasComponents into false
    void Init();

    //! returns the label under which shapes are stored
    TDF_Label BaseLabel() const;

    const Standard_GUID& ID() const override;

    DEFINE_DERIVED_ATTRIBUTE(ShapeAttribute, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(ShapeAttribute, TDataStd_GenericEmpty)

#endif // _ShapeAttribute_HeaderFile
