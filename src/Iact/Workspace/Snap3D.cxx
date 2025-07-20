#include "Iact/Workspace/Snap3D.hxx"

#include "Iact/InteractiveContext.hxx"
#include "Iact/MouseEventData.hxx"

IMPLEMENT_STANDARD_RTTIEXT(Snap3D, SnapBase)

SnapInfo Snap3D::Snap(const Handle(MouseEventData)& mouseEvent)
{
	if(InteractiveContext::Current()->GetEditorState()->IsSnappingEnabled())
	{
		return SnapInfo::Empty;
	}

	SnapInfo snapInfo = SnapInfo::Empty;

	auto screenPoint = mouseEvent->GetScreenPoint();
	auto shape = mouseEvent->GetDetectedBrepShape();
	auto aisObject = mouseEvent->GetDetectedAisObject();

	snapInfo = Snap(screenPoint, shape, aisObject);

	if(snapInfo.mode != SnapMode::None)
	{
		snapInfo.targetName = mouseEvent->GetDetectedEntity()->get_type_name();
	}
	else
	{
		snapInfo = _SnapGrid(screenPoint);
	}

	return snapInfo;
}

SnapInfo Snap3D::_SnapGrid(Graphic3d_Vec2i point)
{
	//workspaceController_->workspace()->Projec


	return SnapInfo::Empty;
}