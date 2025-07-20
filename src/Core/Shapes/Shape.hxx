//! class Shape

#ifndef Shape_Header_File_
#define Shape_Header_File_

#include <TDataStd_Name.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Reference.hxx>

#include "Core/Topology/Body.hxx"
#include "Core/Topology/Entity.hxx"
#include "Iact/InteractiveContext.hxx"

class Shape : public Entity
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Name, Entity)
	DEFINE_ATTRIBUTE_ON_CHILD(2, TDataXtd_Shape, BRep)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataXtd_Shape, TransformedBRep)
	DEFINE_ATTRIBUTE_ON_CHILD(4, TDF_Reference, Body)

public:
	//! Constructor
	Shape(const TDF_Label& label)
		: Entity(label.FindChild(1))
	{};

	//! Destructor
	virtual ~Shape() = default;

	void SetBRep(const TopoDS_Shape& shape)
	{
		TDataXtd_Shape::Set(BRepLabel(), shape);
	}

	TopoDS_Shape GetBRep()
	{
		return TDataXtd_Shape::Get(BRepLabel());
	}

	virtual bool IsValid()
	{
		return !GetBRep().IsNull();
	}

	Handle(Body) GetBody() const
	{
		return MakeHandle<Body>(BodyAttribute()->Get());
	}

	virtual bool Skip()
	{
		// Skip not supported
		return false;
	}

	bool Make()
	{
		return MakeInternal();
	}

	virtual bool MakeInternal()
	{
		return false;
	}

	bool _EnsureBRep()
	{
		try
		{
			if(!IsValid())
			{
				if(!Make())
				{
					return false;
				}
			}
		}
		catch(std::exception& e)
		{
			return false;
		}
		return true;
	}
	virtual void Invalidate() 
	{
		InteractiveContext::Current()->GetWorkspaceController()->Invalidate();
	}

private:
	TDF_Label myLabel;
};

DEFINE_STANDARD_HANDLE(Shape, Entity)

#endif  // _Shape_Header_File_
