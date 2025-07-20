#include "Iact/Workspace/EditorState.hxx"

#include <QSettings>

#include "Iact/WorkspaceController.hxx"
#include "Iact/InteractiveContext.hxx"

IMPLEMENT_STANDARD_RTTIEXT(EditorState, Standard_Transient)

// 实现文件
EditorState::EditorState()
{
    workspaceController_ = InteractiveContext::Current()->GetWorkspaceController();
    if(!workspaceController_.IsNull())
    {
    }
}

EditorState::~EditorState()
{
    // 清理信号连接（由 boost::signals2 自动管理，无需手动清理）
}

void EditorState::SetActiveTool(const std::string& toolId)
{
    activeTool_ = toolId;
    OnActiveToolChanged(toolId); // 触发信号
}

void EditorState::SetRubberbandSelectionMode(RubberbandSelectionMode mode)
{
    rubberbandSelectionMode_ = mode;
    OnRubberbandSelectionModeChanged(mode); // 触发信号
}

void EditorState::SetRubberbandIncludeTouched(bool include)
{
    rubberbandIncludeTouched_ = include;
    OnRubberbandIncludeTouchedChanged(include); // 触发信号
}

void EditorState::SetSnapToGridSelected(bool selected)
{
    if(snapToGridSelected_ != selected)
    {
        snapToGridSelected_ = selected;
        OnSnapToGridSelectedChanged(selected); // 触发信号
    }
}

void EditorState::SetSnapToVertexSelected(bool selected)
{
    if(snapToVertexSelected_ != selected)
    {
        snapToVertexSelected_ = selected;
        OnSnapToVertexSelectedChanged(selected); // 触发信号
        if(!workspaceController_.IsNull()) workspaceController_->InvalidateSelection();
    }
}

void EditorState::SetSnapToEdgeSelected(bool selected)
{
    if(snapToEdgeSelected_ != selected)
    {
        snapToEdgeSelected_ = selected;
        OnSnapToEdgeSelectedChanged(selected); // 触发信号
        if(!workspaceController_.IsNull()) workspaceController_->InvalidateSelection();
    }
}

void EditorState::SetSnappingEnabled(bool enabled)
{
    if(snappingEnabled_ != enabled)
    {
        snappingEnabled_ = enabled;
        OnSnappingEnabledChanged(enabled); // 触发信号
        if(!workspaceController_.IsNull()) workspaceController_->InvalidateSelection();
    }
}

void EditorState::SetDocumentFilterFlags(DocumentFilterFlags flags)
{
    if(documentFilterFlags_ != flags)
    {
        documentFilterFlags_ = flags;
        OnDocumentFilterFlagsChanged(flags); // 触发信号
    }
}

void EditorState::UpdateActiveTool(const std::string& toolId)
{
    activeTool_ = toolId;
    OnActiveToolChanged(toolId); // 触发信号
}

void EditorState::OnWorkspaceControllerPropertyChanged(const std::string& propertyName)
{
}

void EditorState::LoadFromQSettings(const std::shared_ptr<QSettings>& settings)
{
    if(settings)
    {
        // 设置活动工具
        this->SetActiveTool(settings->value("activeTool", "").toString().toStdString());

        // 设置橡皮筋选择模式
        QString rubberbandMode = settings->value("rubberbandSelectionMode", "Rectangle").toString();
        if(rubberbandMode == "Polygon")
        {
            this->SetRubberbandSelectionMode(RubberbandSelectionMode::Polygon);
        }
        else
        {
            this->SetRubberbandSelectionMode(RubberbandSelectionMode::Rectangle);
        }

        // 设置橡皮筋是否包含触碰对象
        this->SetRubberbandIncludeTouched(settings->value("rubberbandIncludeTouched", false).toBool());

        // 设置捕捉选项
        this->SetSnapToGridSelected(settings->value("snapToGridSelected", false).toBool());
        this->SetSnapToVertexSelected(settings->value("snapToVertexSelected", false).toBool());
        this->SetSnapToEdgeSelected(settings->value("snapToEdgeSelected", false).toBool());
        this->SetSnappingEnabled(settings->value("snappingEnabled", false).toBool());

        // 设置文档过滤器标志（假设用整数表示）
        int filterFlags = settings->value("documentFilterFlags", 0).toInt();
        this->SetDocumentFilterFlags(static_cast<DocumentFilterFlags>(filterFlags));
    }
}

void EditorState::SaveToQSettings(const std::shared_ptr<QSettings>&settings) const
{
    // 活动工具
    settings->setValue("activeTool", QString::fromStdString(this->GetActiveTool()));

    // 橡皮筋选择模式
    const QString modeStr = (this->GetRubberbandSelectionMode() == RubberbandSelectionMode::Polygon)
        ? "Polygon" : "Rectangle";
    settings->setValue("rubberbandSelectionMode", modeStr);

    // 包含触碰对象
    settings->setValue("rubberbandIncludeTouched", this->IsRubberbandIncludeTouched());

    // 捕捉设置
    settings->setValue("snapToGridSelected", this->IsSnapToGridSelected());
    settings->setValue("snapToVertexSelected", this->IsSnapToVertexSelected());
    settings->setValue("snapToEdgeSelected", this->IsSnapToEdgeSelected());
    settings->setValue("snappingEnabled", this->IsSnappingEnabled());

    // 文档过滤器标志
    settings->setValue("documentFilterFlags", static_cast<int>(this->GetDocumentFilterFlags()));

    settings->sync(); // 立即保存到磁盘
}
