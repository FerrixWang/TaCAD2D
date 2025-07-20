//! Editor class
//! This class is used to create a GUI editor for the Iact framework.

#ifndef IACT_FRAMEWORK_EDITOR_HXX
#define IACT_FRAMEWORK_EDITOR_HXX

#include "Iact/Framework/WorkspaceControl.hxx"

class Editor : public WorkspaceControl
{
public:
	//! Constructor
	Editor() = default;

	//! Destructor
	virtual ~Editor() = default;
};

DEFINE_STANDARD_HANDLE(Editor, WorkspaceControl)

#endif // IACT_FRAMEWORK_EDITOR_HXX
