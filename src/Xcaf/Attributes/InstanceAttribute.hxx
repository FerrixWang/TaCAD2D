
#ifndef _InstanceAttribute_HeaderFile
#define _InstanceAttribute_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>
#include <Standard_Transient.hxx>
#include <TDF_Attribute.hxx>

class InstanceAttribute : public TDF_Attribute
{
public:
    static const Standard_GUID& GetID();
    //! Create (if not exist) InstanceAttribute from XCAFDoc on <L>.
    static Handle(InstanceAttribute) Set(const TDF_Label& L) {
		Set(L, nullptr);
    }

    //! Create (if not exist) InstanceAttribute from XCAFDoc on <L>.
    static Handle(InstanceAttribute) Set(const TDF_Label& L, const Handle(Standard_Transient)& instance);

    //! Creates an empty tool
    //! Creates a tool to work with a document <Doc>
    //! Attaches to label XCAFDoc::LabelShapes()
    InstanceAttribute();

    //! set hasComponents into false
    void Init();

    const Standard_GUID& ID() const override;

    Handle(Standard_Transient) GetInstance() const { return myInstance; }

    void SetInstance(const Handle(Standard_Transient)& instance)
    {
		myInstance = instance;
	}

	template <typename T>
    Handle(T) Get() const
    {
		return Handle(T)::DownCast(myInstance);
    }

	template <typename T>
    void SetInstance(const Handle(T)& instance)
    {
		myInstance = instance;
    }

    Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

    Standard_EXPORT void Restore(const Handle(TDF_Attribute)& with) Standard_OVERRIDE;

    Standard_EXPORT void Paste(const Handle(TDF_Attribute)& into,
                               const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

    DEFINE_STANDARD_RTTIEXT(InstanceAttribute, TDF_Attribute)

private:
    Handle(Standard_Transient) myInstance;
};

DEFINE_STANDARD_HANDLE(InstanceAttribute, TDataStd_GenericEmpty)

#endif // _InstanceAttribute_HeaderFile
