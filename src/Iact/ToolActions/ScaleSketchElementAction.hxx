#ifndef IACT_TOOLACTIONS_ScaleSketchElementAction_HXX
#define IACT_TOOLACTIONS_ScaleSketchElementAction_HXX

#include <array>
#include <optional>

#include <Bnd_Box2d.hxx>
#include <Extrema_ExtPElC2d.hxx>
#include <ElSLib.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_XY.hxx>
#include <Standard_Handle.hxx>

#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"
#include "Iact/Framework/ToolAction.hxx"
#include "Iact/Visual/HintLine.hxx"
#include "Iact/Visual/Marker.hxx"
//#include "Iact/Snapping/SnapOnCurve2D.hxx"

// 缩放草图元素的交互动作，通过拖动边界框句柄调整缩放比例
class ScaleSketchElementAction : public ToolAction
{
public:
    // 构造函数，初始化动作
    ScaleSketchElementAction(const Handle<SketchEditorTool>& sketchEditorTool)
        : _SketchEditorTool(sketchEditorTool)
        , _MoveMode(-1)
        , _LengthOfOne(0.0)
        , _LastValue(0.0)
        //, _SnapHandler(nullptr)
    {
        for(auto& line : _Lines)
            line = nullptr;
        for(auto& handle : _Handles)
            handle = nullptr;
    }

    // 析构函数
    virtual ~ScaleSketchElementAction() = default;

    // 设置边界框并更新视觉元素
    void SetBox(const Bnd_Box2d& box)
    {
        myBox = box;
        if(GetWorkspaceController())
            _Update();
    }

    // 获取边界框
    const Bnd_Box2d& Box() const { return myBox; }

    // 获取缩放轴
    const gp_Ax2d& Axis() const { return _Axis; }

    // 获取缩放值
    double Scaled() const { return _LastValue; }

    // 获取参考长度
    double Reference() const { return _LengthOfOne; }

    // 获取鼠标事件数据
    const Handle<MouseEventData>& GetMouseEventData() const { return _LastMouseEventData; }

protected:
    // 初始化动作，创建边界框和句柄
    virtual bool onStart() override
    {
        if(myBox.IsVoid())
            return false;

        _Update();
        return true;
    }

    // 清理资源
    virtual void cleanup() override
    {
        // 清理信号
        Preview.disconnect_all_slots();
        Finished.disconnect_all_slots();

        // 移除线条
        for(auto& line : _Lines)
        {
            if(!line.IsNull())
                Remove(line);
            line = nullptr;
        }

        // 移除句柄
        for(auto& handle : _Handles)
        {
            if(!handle.IsNull())
                Remove(handle);
            handle = nullptr;
        }

        // 移除捕捉处理器
        //if (!_SnapHandler.IsNull())
        //{
        //    RemoveSnapHandler();
        //    _SnapHandler = nullptr;
        //}

        ToolAction::cleanup();
    }

public:
    // 信号定义
    Signal<void(const Handle<ScaleSketchElementAction>&)> Preview;
    Signal<void(const Handle<ScaleSketchElementAction>&)> Finished;

    // 鼠标移动事件
    virtual bool onMouseMove(const Handle<MouseEventData>& data) override
    {
        if(_MoveMode >= 0 && !m_isFinished)
        {
            _EnsureVisuals();
            ProcessMouseInput(data);

            if(_LastValue != 0.0)
            {
                _Handles[_MoveMode]->Set(ElSLib::Value(_Axis.Location().X() + _LastValue * _Axis.Direction().X(),
                                         _Axis.Location().Y() + _LastValue * _Axis.Direction().Y(),
                                         _SketchEditorTool->GetSketch()->Plane()));
                Preview(this);
            }

            return true;
        }
        return ToolAction::onMouseMove(data);
    }

    // 鼠标按下事件
    virtual bool onMousePress(const Handle<MouseEventData>& data) override
    {
        _MoveMode = _DetectHandle(data);

        if(_MoveMode >= 0)
        {
            _EnsureVisuals();
            _Handles[_MoveMode]->SetSelectable(false);
            _Handles[_MoveMode]->SetSelected(true);

            //_SnapHandler = new SnapOnCurve2D();
            //_SnapHandler->SetPlane(_SketchEditorTool->GetSketch()->Plane());
            //SetSnapHandler(_SnapHandler);

            ProcessMouseInput(data);

            if(_LastValue != 0.0)
            {
                //SetCursor(CursorStyle::Move);
                Preview(this);
            }

            return true;
        }
        return ToolAction::onMousePress(data);
    }

    // 鼠标释放事件
    virtual bool onMouseRelease(const Handle<MouseEventData>& data) override
    {
        if(_MoveMode >= 0 && !m_isFinished)
        {
            ProcessMouseInput(data);
            m_isFinished = true;
            Finished(this);

            Remove(_Handles[_MoveMode]);
            _Handles[_MoveMode] = nullptr;

            _MoveMode = -1;
            //SetCursor(CursorStyle::Default);

            _Update();
            //data->ForceReDetection = true;

            return true;
        }
        return ToolAction::onMouseRelease(data);
    }

private:
    // 处理鼠标输入，投影到缩放轴
    void ProcessMouseInput(const Handle<MouseEventData>& data)
    {
        _LastMouseEventData = data;

        double u, v;
        ElSLib::Parameters(_SketchEditorTool->GetSketch()->Plane(), data->PointOnPlane(), u, v);
        gp_Pnt2d sketchPoint(u, v);

        Extrema_ExtPElC2d extrema(sketchPoint, gp_Lin2d(_Axis), 1e-5, -1e10, 1e10);
        if(extrema.IsDone() && extrema.NbExt() > 0)
        {
            _LastValue = extrema.Point(1).Parameter();
        }
        else
        {
            _LastValue = 0.0;
        }

        //gp_Pnt2d sketchPoint;
        //SnapInfo snapInfo = _SnapHandler->Snap(new Geom2d_Line(_Axis), data);
        //if (snapInfo.Mode != SnapMode::None)
        //{
        //    sketchPoint = snapInfo.Point;
        //}
        //else
        //{
        //    double u, v;
        //    ElSLib::Parameters(_SketchEditorTool->GetSketch()->Plane(), data->PointOnPlane(), u, v);
        //    sketchPoint = gp_Pnt2d(u, v);
        //}
    }

    // 检测用户点击的句柄
    int _DetectHandle(const Handle<MouseEventData>& data)
    {
        for(size_t i = 0; i < _Handles.size(); ++i)
        {
            //if(!_Handles[i].IsNull() && !data->DetectedAisObject().IsNull() &&
            //   data->DetectedAisObject() == _Handles[i]->AisObject())
            //{
            //    double minX, minY, maxX, maxY;
            //    myBox.Get(minX, minY, maxX, maxY);
            //    double extentX = maxX - minX;
            //    double extentY = maxY - minY;
            //    gp_Pnt2d startPoint(minX + extentX * _HandlePositions[i].X(),
            //                        minY + extentY * _HandlePositions[i].Y());
            //    gp_Vec2d vec(myBox.Center(), startPoint);
            //    _LengthOfOne = vec.Magnitude();
            //    if(_LengthOfOne < 1e-5)
            //        _LengthOfOne = 1.0; // 防止除零
            //    _Axis = gp_Ax2d(myBox.Center(), vec);
            //    return static_cast<int>(i);
            //}
        }
        return -1;
    }

    // 确保视觉元素存在
    void _EnsureVisuals()
    {
        // 确保线条
        for(size_t i = 0; i < _Lines.size(); ++i)
        {
            if(_Lines[i].IsNull())
            {
                _Lines[i] = new HintLine(HintStyle::Solid);
                Add(_Lines[i]);
            }
        }

        // 确保句柄
        for(size_t i = 0; i < _Handles.size(); ++i)
        {
            if(_MoveMode >= 0 && static_cast<int>(i) != _MoveMode)
                continue;

            if(_Handles[i].IsNull())
            {
                _Handles[i] = new Marker(MarkerStyle::Topmost, "Ball", 12);
                _Handles[i]->SetSelectable(true);
                Add(_Handles[i]);
            }
        }

        _Update();
    }

    // 更新边界框的线条和句柄
    void _Update()
    {
        double minX, minY, maxX, maxY;
        myBox.Get(minX, minY, maxX, maxY);
        double extentX = maxX - minX;
        double extentY = maxY - minY;
        gp_Trsf transform/* = _SketchEditorTool->GetSketch()->Get()*/;

        // 更新线条（边界框四边）
        for(size_t i = 0; i < _Lines.size(); ++i)
        {
            if(!_Lines[i].IsNull())
            {
                gp_Pnt start(minX + extentX * _HandlePositions[i].X(),
                             minY + extentY * _HandlePositions[i].Y(), 0.0);
                gp_Pnt end(minX + extentX * _HandlePositions[(i + 1) % 4].X(),
                           minY + extentY * _HandlePositions[(i + 1) % 4].Y(), 0.0);
                _Lines[i]->Set(start, end);
                _Lines[i]->SetLocalTransformation(transform);
            }
        }

        // 更新句柄（八个点）
        for(size_t i = 0; i < _Handles.size(); ++i)
        {
            if(_MoveMode >= 0 && static_cast<int>(i) != _MoveMode)
                continue;

            if(!_Handles[i].IsNull())
            {
                gp_Pnt pos(minX + extentX * _HandlePositions[i].X(),
                           minY + extentY * _HandlePositions[i].Y(), 0.0);
                _Handles[i]->Set(pos);
                _Handles[i]->SetLocalTransformation(transform);
            }
        }
    }

private:
    Handle<SketchEditorTool> _SketchEditorTool; // 草图编辑工具
    std::array<Handle<HintLine>, 4> _Lines; // 边界框四边
    std::array<Handle<Marker>, 8> _Handles; // 八个缩放句柄
    std::array<gp_XY, 8> _HandlePositions = { // 句柄相对位置
        gp_XY(0.0, 0.0), gp_XY(1.0, 0.0), gp_XY(1.0, 1.0), gp_XY(0.0, 1.0), // 角点
        gp_XY(0.5, 0.0), gp_XY(0.5, 1.0), gp_XY(0.0, 0.5), gp_XY(1.0, 0.5)  // 边中点
    };
    int _MoveMode; // 当前拖动的句柄索引（-1表示未选中）
    Bnd_Box2d myBox; // 边界框
    double _LengthOfOne; // 参考长度
    gp_Ax2d _Axis; // 缩放轴
    double _LastValue; // 上次缩放值
    //Handle<SnapOnCurve2D> _SnapHandler; // 捕捉处理器
    Handle<MouseEventData> _LastMouseEventData; // 最后鼠标事件数据
};

DEFINE_STANDARD_HANDLE(ScaleSketchElementAction, ToolAction)

#endif // IACT_TOOLACTIONS_ScaleSketchElementAction_HXX
