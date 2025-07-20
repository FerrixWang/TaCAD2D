#ifndef _WorkspaceControll_Header_File_H_
#define _WorkspaceControll_Header_File_H_

// stl includes
#include <algorithm>
#include <vector>

#include <Standard_Transient.hxx>

#include "Iact/HudElements/HudElement.hxx"
#include "Iact/HudElements/HudManager.hxx"
#include "Iact/MouseEventData.hxx"
#include "Iact/Visual/VisualObject.hxx"
#include "Iact/Workspace/SnapBase.hxx"

class WorkspaceController;

class WorkspaceControl : public Standard_Transient
{
public:
	//! Constructor
	WorkspaceControl() = default;

	// Get the workspace controller
	Handle(WorkspaceController) GetWorkspaceController() const
	{
		return m_workspaceController;
	}

	void setWorkspaceController(const Handle(WorkspaceController)& controller)
	{
		m_workspaceController = controller;
	}

	virtual std::vector<Handle(WorkspaceControl)> children() { return {}; }

	virtual bool onMouseMove(const Handle(MouseEventData)& theViewportController)
	{
		auto arr = children();
		return std::any_of(arr.begin(), arr.end(), [&](const auto& child) {
			return child->onMouseMove(theViewportController);
		});
	}

	virtual bool onMousePress(const Handle(MouseEventData)& theViewportController)
	{
		auto arr = children();
		return std::any_of(arr.begin(), arr.end(), [&](const auto& child) {
			return child->onMousePress(theViewportController);
		});
	}

	virtual bool onMouseRelease(const Handle(MouseEventData)& theViewportController)
	{
		auto arr = children();
		return std::any_of(arr.begin(), arr.end(), [&](const auto& child) {
			return child->onMouseRelease(theViewportController);
		});
	}

	Handle(SnapBase) SetSnapHandler(const Handle(SnapBase)& snapHandler)
	{
		RemoveSnapHandler();
		snapHandler->SetWorkspaceController(m_workspaceController);
		_SnapHandler = snapHandler;
		return snapHandler;
	}

	void RemoveSnapHandler()
	{
		if(_SnapHandler.IsNull())
			return;

		_SnapHandler.Nullify();
	}

	Handle(SnapBase) GetSnapHandler()
	{
		// if null, Get it from children which is not null
		if(_SnapHandler.IsNull())
		{
			auto arr = children();
			auto it = std::find_if(arr.begin(), arr.end(), [](const auto& child) {
				return !child->GetSnapHandler().IsNull();
			});
			if(it != arr.end())
				_SnapHandler = (*it)->GetSnapHandler();
		}
		return _SnapHandler;
	}

protected:
	void Add(HudElement* hudElement);
	void Add(const Handle(VisualObject)& visual)
	{
		if(visual.IsNull())
			return;
		visual->SetWorkspaceController(m_workspaceController);
		m_visuals.push_back(visual);
	}

	void Remove(HudElement* hudElement);
	void Remove(const Handle(VisualObject)& visual)
	{
		if(visual.IsNull())
			return;
		visual->Remove();
		auto it = std::find(m_visuals.begin(), m_visuals.end(), visual);
		if(it != m_visuals.end())
			m_visuals.erase(it);
	}

	void SetHintMessage(const std::string& message);

protected:
	virtual void cleanup()
	{
		RemoveVisuals();
		RemoveHudElements();
	}

	void RemoveVisuals()
	{
		for(auto& visual : m_visuals)
		{
			if(visual.IsNull())
				continue;
			visual->Remove();
		}
		m_visuals.clear();
	}

	void RemoveHudElements();

	void OpenCommand();

	void CommitCommand();

protected:
	std::vector<Handle(VisualObject)> m_visuals;
	Handle(WorkspaceController) m_workspaceController;
	Handle(SnapBase) _SnapHandler;
};

#endif // !_WorkspaceControll_Header_File_H_
