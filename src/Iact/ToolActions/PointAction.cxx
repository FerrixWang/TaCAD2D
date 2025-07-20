#include "Iact/ToolActions/PointAction.hxx"

#include <Geom_CartesianPoint.hxx>
#include <Message.hxx>

#include "Iact/Visual/Marker.hxx"
#include "Iact/WorkspaceController.hxx"

// Constructor

PointAction::PointAction()
	: ToolAction(),
	myPoint(),
	_Marker(nullptr),
	_SnapHandler(nullptr)
{
}

bool PointAction::onMouseMove(const Handle(MouseEventData)& data)
{
	if(!m_isFinished)
	{
		ProcessMouseInput(data);
		_EnsureMarker();

		// Emit the signal to preview the point
		Preview(this);

		_Marker->Set(myPoint);
		Message::SendInfo("PointAction");

		return ToolAction::onMouseMove(data);
	}
	return false;
}

bool PointAction::onMouseRelease(const Handle(MouseEventData)& data)
{
	if(!m_isFinished)
	{
		ProcessMouseInput(data);
		m_isFinished = true;
		Finished(this);
	}
	return true;
}

bool PointAction::onMousePress(const Handle(MouseEventData)& data)
{
	return true;
}


void PointAction::ProcessMouseInput(const Handle(MouseEventData)& data)
{
	myPoint = data->PointOnPlane();

	//HasControlModifier
	myKeyFlags = data->GetModifierKeys();
}

void PointAction::_EnsureMarker()
{
	if(_Marker.IsNull())
	{
		_Marker = new Marker(MarkerStyle::Bitmap, Marker::PlusImage());
		_Marker->SetWorkspaceController(m_workspaceController);

		Add(_Marker);
	}
}
