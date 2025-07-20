#include "Iact/Visual/VisualObject.hxx"

#include "Iact/WorkspaceController.hxx"

VisualObject::VisualObject(const Handle(InteractiveEntity)& entity)
	: myEntity(entity)
{}

Handle(AIS_InteractiveContext) VisualObject::AisContext() const
{
	return GetWorkspaceController()->workspace()->AisContext();
}

void VisualObject::SetLocalTransformation(gp_Trsf trsf)
{
	if(myAISObject.IsNull())
	{
		return;
	}
	myAISObject->SetLocalTransformation(trsf);
}
