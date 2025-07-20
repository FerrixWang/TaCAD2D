#ifndef CommandHelper_Header_File_
#define CommandHelper_Header_File_

#include <Standard_Handle.hxx>

#include "Iact/InteractiveContext.hxx"
#include "Iact/WorkspaceController.hxx"

class CommandHelper
{
public:
	template<typename T>
	static bool CurrentToolIs()
	{
		return !Handle(T)::DownCast(InteractiveContext::Current()->GetWorkspaceController()->CurrentTool()).IsNull();
	}

	static bool ApplicationExists()
	{
		return !InteractiveContext::Current()->GetApplication().IsNull();
	}
};

#endif  // CommandHelper_Header_File_
