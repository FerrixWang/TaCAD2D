#include "Iact/Framework/WorkspaceControl.hxx"

#include "Iact/HudElements/HudElement.hxx"
#include "Iact/HudElements/HudManager.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/WorkspaceController.hxx"

void WorkspaceControl::Add(HudElement* hudElement)
{
	if(hudElement == nullptr)
		return;
	m_workspaceController->GetHudManager()->AddElement(hudElement);
}

void WorkspaceControl::Remove(HudElement* hudElement)
{
	if(hudElement == nullptr)
		return;
	m_workspaceController->GetHudManager()->RemoveElement(hudElement);
}

void WorkspaceControl::SetHintMessage(const std::string& message)
{
	m_workspaceController->GetHudManager()->SetHintMessage(this, message);
}

void WorkspaceControl::RemoveHudElements()
{
	auto arr = m_workspaceController->GetHudManager();
	if(!arr.IsNull())
	{
		arr->ClearElements();
	}
}

void WorkspaceControl::OpenCommand()
{
	// if not opened, open the command
	if(!InteractiveContext::Current()->GetDocument()->HasOpenCommand())
	{
		InteractiveContext::Current()->GetDocument()->OpenCommand();
	}
}

void WorkspaceControl::CommitCommand()
{
	InteractiveContext::Current()->GetDocument()->CommitCommand();
}
