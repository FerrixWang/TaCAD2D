#ifndef WorkspaceControl_Header_File_
#define WorkspaceControl_Header_File_

#include <vector>

#include <Standard_Handle.hxx>

#include "Iact/Framework/WorkspaceControl.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"

class SketchTool : public WorkspaceControl
{
public:
	//! Constructor
	SketchTool() = default;

	//! Destructor
	virtual ~SketchTool() = default;

    bool Start(const Handle(SketchEditorTool)& sketchEditorTool)
    {
        _SketchEditorTool = sketchEditorTool;
        _Sketch = _SketchEditorTool->GetSketch();
        m_workspaceController = _SketchEditorTool->GetWorkspaceController();

        if(m_workspaceController.IsNull() || _Sketch.IsNull())
            return false;

        bool started = OnStart();

        _IsActive = started;
        return started;
    }

    void Stop()
    {
        OnStop();
        cleanup();
    }

	virtual void cleanup() override
	{
        _StopToolActions();
		WorkspaceControl::cleanup();
	}

	virtual bool OnStart()
	{
        return false;
	}

    virtual void OnStop() {}

    bool StartAction(const Handle(ToolAction)& toolAction)
    {
        if(_SketchEditorTool->StartToolAction(toolAction))
        {
            _Actions.push_back(toolAction);
            return true;
        }

        return false;
    }

    void StopAction(const Handle(ToolAction)& toolAction)
    {
        if(toolAction.IsNull())
        {
            return;
        }

        _SketchEditorTool->StopToolAction(toolAction);

        // Find and erase the action
        auto it = std::find(_Actions.begin(), _Actions.end(), toolAction);
        if(it != _Actions.end())
        {
            _Actions.erase(it);
        }
    }

    // GetSketch
	Handle(Sketch) GetSketch() const { return _Sketch; }

private:
    void _StopToolActions()
    {
        std::for_each(_Actions.rbegin(), _Actions.rend(), [this](const Handle(ToolAction)& action) {
			StopAction(action);
        });
        _Actions.clear();
    }

protected:
    std::vector<Handle(ToolAction)> _Actions;
    Handle(SketchEditorTool) _SketchEditorTool;
	Handle(Sketch) _Sketch;
    bool _IsActive = false;
};

#endif  // _WorkspaceControll_Header_File_H_
