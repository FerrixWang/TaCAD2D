// ! class ToolAction

#ifndef _IACT_FRAMEWORK_TOOLACTION_HXX_
#define _IACT_FRAMEWORK_TOOLACTION_HXX_

#include "Iact/Framework/WorkspaceControl.hxx"

class ToolAction : public WorkspaceControl
{
public:
	// Constructor
	ToolAction() : WorkspaceControl()
	{}

	bool start() { return onStart(); }
	void stop() { 
		onStop(); 
		cleanup(); 
	}

	void Reset()
	{
		m_isFinished = false;
	}

protected:
	virtual bool onStart() { return false; }
	virtual void onStop() {}
	virtual void reset() { m_isFinished = false; }

protected:
	bool m_isFinished = false;
};

DEFINE_STANDARD_HANDLE(ToolAction, WorkspaceControl)

#endif  // _IACT_FRAMEWORK_TOOLACTION_HXX_
