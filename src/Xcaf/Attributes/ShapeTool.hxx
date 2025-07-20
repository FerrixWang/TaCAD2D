
#ifndef _ShapeTool_HeaderFile
#define _ShapeTool_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDataStd_GenericEmpty.hxx>

class ShapeTool : public TDataStd_GenericEmpty
{
public:
    static const Standard_GUID& GetID();

    //! Create (if not exist) ShapeTool from XCAFDoc on <L>.
    static Handle(ShapeTool) Set(const TDF_Label& L);

    //! Creates an empty tool
    //! Creates a tool to work with a document <Doc>
    //! Attaches to label XCAFDoc::LabelShapes()
    ShapeTool();

    //! set hasComponents into false
    void Init();

    //! returns the label under which shapes are stored
    TDF_Label BaseLabel() const;

    static TDF_Label ShapesLabel(const TDF_Label& acces);

    static TDF_Label BoxsLabel(const TDF_Label& acces);

    const Standard_GUID& ID() const override;

    DEFINE_DERIVED_ATTRIBUTE(ShapeTool, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(ShapeTool, TDataStd_GenericEmpty)

#endif // _ShapeTool_HeaderFile
