
#ifndef _BoxTool_HeaderFile
#define _BoxTool_HeaderFile

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_GenericEmpty.hxx>
#include <TDF_DerivedAttribute.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Primitives/Box.hxx"
#include "Xcaf/Attributes/BoxAttribute.hxx"

class BoxTool : public TDataStd_GenericEmpty
{
public:
    static const Standard_GUID& GetID();

    //! Create (if not exist) BoxTool from XCAFDoc on <L>.
    static Handle(BoxTool) Set(const TDF_Label& L);

    //! Creates an empty tool
    //! Creates a tool to work with a document <Doc>
    //! Attaches to label XCAFDoc::LabelShapes()
    BoxTool();

    //! set hasComponents into false
    void Init();

    //! returns the label under which shapes are stored
    TDF_Label BaseLabel() const;

    static TDF_Label BoxsLabel(const TDF_Label& acces);
   
    static Handle(BoxAttribute) GetBoxAttribute(const TDF_Label& L) { return nullptr; }
	//! add a box to the list of boxes

	static void AddBox(const TDF_Label& L, const Handle(Box)& S);

    const Standard_GUID& ID() const override;

    DEFINE_DERIVED_ATTRIBUTE(BoxTool, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(BoxTool, TDataStd_GenericEmpty)

#endif // _BoxTool_HeaderFile
