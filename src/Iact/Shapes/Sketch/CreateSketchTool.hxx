#ifndef CreateSketchTool_Header_File_
#define CreateSketchTool_Header_File_

#include "Iact/Framework/Tool.hxx"
#include "Iact/WorkspaceController.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx""
#include "Core/Shapes/Sketch/Sketch.hxx"

class CreateSketchTool : public Tool
{
public:
	CreateSketchTool() = default;
	~CreateSketchTool() override = default;

	bool onStart() override
	{
		// Initialize the tool
		m_workspaceController->StartTool(new SketchEditorTool(new Sketch()));

		return true;
	}

	void onStop() override
	{
		// Cleanup the tool
	}

	bool onCancel() override
	{
		// Handle cancel action
		return true;
	}
};

#endif  // _CreateSketchTool_Header_File_
