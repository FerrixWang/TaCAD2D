//! class VisualObject

#ifndef  _VisualObject_Header_File_
#define  _VisualObject_Header_File_

#include <AIS_InteractiveContext.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Transient.hxx>

#include "Core/Topology/InteractiveEntity.hxx"

class WorkspaceController;

class VisualObject : public Standard_Transient
{
public:
	explicit VisualObject() {}

	explicit VisualObject(const TDF_Label& label) {}

	explicit VisualObject(const Handle(InteractiveEntity)& entity);

	void SetWorkspaceController(const Handle(WorkspaceController)& controller)
	{
		myController = controller;
	}

	Handle(InteractiveEntity) GetEntity() const
	{
		return myEntity;
	}

	Handle(WorkspaceController) GetWorkspaceController() const
	{
		return myController;
	}

	Handle(AIS_InteractiveContext) AisContext() const;

	virtual Handle(AIS_InteractiveObject) AisObject() const { return nullptr; }

	void SetLocalTransformation(gp_Trsf trsf);

	virtual void Remove() {}

	virtual void Update() {}

	virtual bool IsSelectable() const { return false; }
	virtual void SetSelectable(bool isSelectable) {}

	virtual bool IsSelected() const 
	{ 
		return myContext->IsSelected(myAISObject);
	}

	virtual void SetSelected(bool value) const {
	
		if(myContext->IsSelected(myAISObject) != value)
		{
			myContext->AddOrRemoveSelected(myAISObject, false);
		}	
	}

public:
	Signal<void(Handle(VisualObject))> AisObjectChanged;

private:
	Handle(InteractiveEntity)   myEntity;     //!< The entity to be visualized
	Handle(WorkspaceController) myController; //!< The controller to manage the visualization
	Handle(AIS_InteractiveContext) myContext;    //!< The context to manage the visualization
	Handle(AIS_InteractiveObject) myAISObject;   //!< The AIS object to be visualized
};

DEFINE_STANDARD_HANDLE(VisualObject, Standard_Transient)

#endif  // _VisualObject_Header_File_
