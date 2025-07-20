//! class EditorState
//! 

#ifndef IACT_EDITORSTATE_HXX
#define IACT_EDITORSTATE_HXX

#include <string>
#include <vector>

#include <QSettings>

#include <AIS_InteractiveObject.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>

#include "Iact/WorkspaceController.hxx"

// 前向声明
class SnapInfo;
class ViewportController;

// 定义句柄
DEFINE_STANDARD_HANDLE(EditorState, Standard_Transient)

// 假设的枚举类型（仅保留必要部分）
enum class RubberbandSelectionMode { Rectangle, Polygon  };
enum class DocumentFilterFlags { None = 0, /* 其他标志 */ };

// EditorState 类
class EditorState : public Standard_Transient
{
public:
    // 构造函数
    EditorState();

    // 析构函数
    virtual ~EditorState();

    // RTTI 支持
    DEFINE_STANDARD_RTTIEXT(EditorState, Standard_Transient)

    // 获取和设置当前活动工具ID
    std::string GetActiveTool() const { return activeTool_; }
    void SetActiveTool(const std::string& toolId);

    // 橡皮筋选择模式
    RubberbandSelectionMode GetRubberbandSelectionMode() const { return rubberbandSelectionMode_; }
    void SetRubberbandSelectionMode(RubberbandSelectionMode mode);

    // 橡皮筋选择是否包含触碰的对象
    bool IsRubberbandIncludeTouched() const { return rubberbandIncludeTouched_; }
    void SetRubberbandIncludeTouched(bool include);

    // 是否启用网格捕捉
    bool IsSnapToGridSelected() const { return snapToGridSelected_; }
    void SetSnapToGridSelected(bool selected);

    // 是否启用顶点捕捉
    bool IsSnapToVertexSelected() const { return snapToVertexSelected_; }
    void SetSnapToVertexSelected(bool selected);

    // 是否启用边捕捉
    bool IsSnapToEdgeSelected() const { return snapToEdgeSelected_; }
    void SetSnapToEdgeSelected(bool selected);

    // 是否启用捕捉功能
    bool IsSnappingEnabled() const { return snappingEnabled_; }
    void SetSnappingEnabled(bool enabled);

    // 获取捕捉信息
    SnapInfo* GetSnapInfo() const { return snapInfo_; }

    // 文档过滤器标志
    DocumentFilterFlags GetDocumentFilterFlags() const { return documentFilterFlags_; }
    void SetDocumentFilterFlags(DocumentFilterFlags flags);

	void LoadFromQSettings(const std::shared_ptr<QSettings>& settings);
	void SaveToQSettings(const std::shared_ptr<QSettings>& settings) const;

public:
    Signal<void(const std::string&)> OnActiveToolChanged;
    Signal<void(RubberbandSelectionMode)> OnRubberbandSelectionModeChanged;
    Signal<void(bool)> OnRubberbandIncludeTouchedChanged;
    Signal<void(bool)> OnSnapToGridSelectedChanged;
    Signal<void(bool)> OnSnapToVertexSelectedChanged;
    Signal<void(bool)> OnSnapToEdgeSelectedChanged;
    Signal<void(bool)> OnSnappingEnabledChanged;
    Signal<void(SnapInfo*)> OnSnapInfoChanged;
    Signal<void(DocumentFilterFlags)> OnDocumentFilterFlagsChanged;

private:
    // 更新活动工具
    void UpdateActiveTool(const std::string& toolId);

    // 工作区控制器属性变化回调
    void OnWorkspaceControllerPropertyChanged(const std::string& propertyName);

    // 数据成员
    std::string activeTool_;                        // 当前活动工具ID
    RubberbandSelectionMode rubberbandSelectionMode_; // 橡皮筋选择模式
    bool rubberbandIncludeTouched_ = false;         // 橡皮筋选择是否包含触碰
    Handle(WorkspaceController) workspaceController_ = nullptr; // 工作区控制器
    bool snapToGridSelected_ = false;               // 网格捕捉开关
    bool snapToVertexSelected_ = false;             // 顶点捕捉开关
    bool snapToEdgeSelected_ = false;               // 边捕捉开关
    bool snappingEnabled_ = false;                  // 捕捉总开关
    SnapInfo* snapInfo_ = nullptr;                  // 捕捉信息
    DocumentFilterFlags documentFilterFlags_ = DocumentFilterFlags::None; // 文档过滤器标志
};

#endif  // IACT_EDITORSTATE_HXX