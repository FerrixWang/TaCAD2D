//! Tool class

#ifndef _Tool_Header_File_
#define _Tool_Header_File_

#include "Iact/Framework/ToolAction.hxx"
#include "Iact/Framework/WorkspaceControl.hxx"

class Tool : public WorkspaceControl
{
public:
	//! Constructor
	Tool() = default;

	//! Destructor
	virtual ~Tool() = default;

public:

	//! current toolaction	
	Handle(ToolAction) currentAction() const
	{
		return m_toolActions.empty() ? nullptr : m_toolActions.front();
	}

	bool start()
	{
		if(onStart())
		{
			m_isActive = true;
			return true;
		}
		return false;
	}

	void stop()
	{
		m_isActive = false;
		onStop();
		cleanup();
	}

	bool cancel(bool force)
	{
		if(!onCancel() && !force)
			return false;

		if(m_isActive)
			stop();
		return true;
	}

protected:
	virtual bool onStart() { return false; }
	virtual void onStop() { }
	virtual bool onCancel() { return true; }

	//! start toolaction
	bool startAction(const Handle(ToolAction)& theToolAction, bool exclusive = true)
	{
		// if contains toolaction, return true
		if(std::find(m_toolActions.begin(), m_toolActions.end(), theToolAction) != m_toolActions.end())
			return true;

		try
		{
			if(exclusive)
			{
				stopAllActions();
			}
			if(!theToolAction.IsNull())
			{
				theToolAction->setWorkspaceController(m_workspaceController);
				if(!theToolAction->start())
					return false;

				// add to the begin of toolactions
				m_toolActions.insert(m_toolActions.begin(), theToolAction);
				toolActionChanged(theToolAction);
			}
		}
		catch(const std::bad_alloc&)
		{
			return false;
		
		}
		return true;
	}

	//! stop toolaction
	
	void stopAction(const Handle(ToolAction)& theToolAction)
	{
		if(theToolAction.IsNull())
			return;
		auto it = std::find(m_toolActions.begin(), m_toolActions.end(), theToolAction);
		if(it != m_toolActions.end())
		{
			theToolAction->stop();
			m_toolActions.erase(it);
		}
	}

	//! stop all toolactions
	
	void stopAllActions()
	{
		for(auto& action : m_toolActions)
		{
			stopAction(action);
		}
		m_toolActions.clear();		
	}
	void RestoreAllVisualShapes()
	{}

	virtual std::vector<Handle(WorkspaceControl)> children() override 
	{ 
		// return all toolactions with convert to WorkspaceControl
		std::vector<Handle(WorkspaceControl)> arr;
		// using stl transform to copy and convert with same order
		std::transform(m_toolActions.begin(), m_toolActions.end(), std::back_inserter(arr),
					   [](const Handle(ToolAction)& action) { return Handle(WorkspaceControl)::DownCast(action); });
		return arr;
	}

protected:
	virtual void cleanup() override
	{
		stopAllActions();
		RestoreAllVisualShapes();
		WorkspaceControl::cleanup();
	}

public:
	Signal<void(const Handle(ToolAction)&)> toolActionChanged;

private:
	bool m_isActive = false;

	std::vector<Handle(ToolAction)> m_toolActions;
};

DEFINE_STANDARD_HANDLE(Tool, WorkspaceControl)

#endif // _Tool_Header_File_
