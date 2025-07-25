//#ifndef SKETCHTOOLSCALE_HXX
//#define SKETCHTOOLSCALE_HXX
//
//#include <map>
//#include <set>
//#include <vector>
//
//#include <Bnd_Box2d.hxx>
//#include <BndLib_Add2dCurve.hxx>
//#include <gp_Ax2d.hxx>
//#include <gp_Pnt2d.hxx>
//#include <gp_XY.hxx>
//#include <Standard_Handle.hxx>
//
//#include "Core/Geom/MathUtils.hxx"
//#include "Core/Shapes/Sketch/Elements/SketchElementPoint.hxx"
//#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
//#include "Core/Shapes/Sketch/Segments/SketchSegment.hxx"
//#include "Core/Shapes/Sketch/Sketch.hxx"
//#include "Iact/HudElements/LabelHudElement.hxx"
//#include "Iact/InteractiveContext.hxx"
//#include "Iact/Shapes/Sketch/SketchTool.hxx"
//#include "Iact/ToolActions/ScaleSketchElementAction.hxx"
//
//// 缩放草图元素的工具，允许用户通过拖动句柄缩放选定的点和线段
//class SketchToolScale : public SketchTool
//{
//public:
//    // 构造函数，初始化成员变量
//    SketchToolScale()
//        : _ToolAction(nullptr)
//        , _HudElement(nullptr)
//        , _PreviewIsValid(false)
//    {}
//
//    // 析构函数
//    virtual ~SketchToolScale() = default;
//
//    // 初始化工具，设置缩放操作
//    virtual bool OnStart() override
//    {
//        // 隐藏草图约束以减少视觉干扰
//        SketchEditorTool->Elements()->SetConstraintsVisible(false);
//
//        // 收集选中的线段和点
//        _OriginalSegments = SketchEditorTool->SelectedSegments();
//        std::set<int> pointSet;
//        for(const auto& seg : _OriginalSegments)
//        {
//            auto points = seg->PointsAttribute()->Array();
//            pointSet.insert(points.begin(), points.end());
//        }
//        for(int point : SketchEditorTool->SelectedPoints())
//            pointSet.insert(point);
//        _SelectedPoints.assign(pointSet.begin(), pointSet.end());
//
//        if(_SelectedPoints.empty() && _OriginalSegments.empty())
//            return false;
//
//        // 克隆线段，创建临时副本用于缩放预览
//        _TempSegments.clear();
//        for(size_t i = 0; i < _OriginalSegments.size(); ++i)
//        {
//            _TempSegments[static_cast<int>(i)] = _OriginalSegments[i]->Clone();
//        }
//
//        // 初始化缩放动作
//        _ToolAction = new ScaleSketchElementAction(SketchEditorTool);
//        if(!_UpdateBoundingBox())
//            return false;
//
//        // 连接预览和完成信号
//        _ToolAction->Preview.connect([this](const auto& sender, const auto& args) { _PointAction_Preview(sender, args); });
//        _ToolAction->Finished.connect([this](const auto& sender, const auto& args) { _PointAction_Finished(sender, args); });
//
//        if(!StartAction(_ToolAction))
//            return false;
//
//        // 禁用草图元素选择
//        SketchEditorTool->Elements()->Activate(false, false, false);
//
//        // 设置用户提示
//        SetHintMessage("Scale selected elements by dragging handles, press Ctrl to round to 5%, press Shift to scale relative to center.");
//
//        return true;
//    }
//
//    // 处理草图变化，更新边界框
//    virtual void OnSketchChanged(const Handle<Sketch>& sketch, Sketch::ElementType types) override
//    {
//        _UpdateBoundingBox();
//    }
//
//    // 停止工具
//    virtual void OnStop() override
//    {
//        SketchTool::OnStop();
//        Stoped();
//    }
//
//    // 清理资源
//    virtual void cleanup() override
//    {
//        // 移除预览点元素
//        for(const auto& [key, value] : _PreviewPointElements)
//        {
//            if(!value.IsNull())
//                value->ForgetAllAttributes();
//        }
//        _PreviewPointElements.clear();
//
//        // 移除预览线段元素
//        for(const auto& [key, value] : _PreviewSegmentElements)
//        {
//            if(!value.IsNull())
//                value->ForgetAllAttributes();
//        }
//        _PreviewSegmentElements.clear();
//
//        // 移除HUD元素
//        if(_HudElement)
//        {
//            Remove(_HudElement);
//            _HudElement = nullptr;
//        }
//
//        // 清除缩放动作
//        if(_ToolAction)
//        {
//            _ToolAction->Reset();
//            _ToolAction = nullptr;
//        }
//
//        // 清空临时数据
//        _TempPoints.clear();
//        _TempSegments.clear();
//        _SelectedPoints.clear();
//        _OriginalSegments.clear();
//
//        SketchTool::cleanup();
//    }
//
//private:
//    // 更新边界框，基于选中的点和线段
//    bool _UpdateBoundingBox()
//    {
//        Bnd_Box2d box;
//        const auto& points = _TempPoints.empty() ? SketchEditorTool->GetSketch()->Points() : _TempPoints;
//        for(int i : _SelectedPoints)
//        {
//            box.Add(points.at(i));
//        }
//        for(const auto& seg : _OriginalSegments)
//        {
//            if(!seg->CachedCurve().IsNull())
//            {
//                BndLib_Add2dCurve::Add(seg->CachedCurve(), 0.01, box);
//            }
//        }
//        if(box.IsVoid())
//            return false;
//
//        _ToolAction->SetBox(box);
//        return true;
//    }
//
//    // 更新原始线段及其关联点的可见性
//    void _UpdateSegmentElementsVisibility(bool show)
//    {
//        if(_OriginalSegments.empty())
//            return;
//
//        // 显示/隐藏线段元素
//        auto segmentElements = SketchEditorTool->Elements()->SegmentElements();
//        for(const auto& seg : _OriginalSegments)
//        {
//            for(auto& element : segmentElements)
//            {
//                if(element->Segment() == seg)
//                    element->SetVisible(show);
//            }
//        }
//
//        // 显示/隐藏仅由原始线段使用的点
//        std::set<int> allPoints;
//        for(const auto& seg : _OriginalSegments)
//        {
//            auto points = seg->PointsAttribute()->Array();
//            allPoints.insert(points.begin(), points.end());
//        }
//
//        for(int pointIndex : allPoints)
//        {
//            std::vector<Handle<SketchSegment>> segmentsWithPoint;
//            for(const auto& [idx, seg] : SketchEditorTool->GetSketch()->Segments())
//            {
//                auto segPoints = seg->PointsAttribute()->Array();
//                if(std::find(segPoints.begin(), segPoints.end(), pointIndex) != segPoints.end())
//                    segmentsWithPoint.push_back(seg);
//            }
//
//            bool allContained = true;
//            for(const auto& seg : segmentsWithPoint)
//            {
//                if(std::find(_OriginalSegments.begin(), _OriginalSegments.end(), seg) == _OriginalSegments.end())
//                {
//                    allContained = false;
//                    break;
//                }
//            }
//
//            if(allContained)
//            {
//                for(auto& element : SketchEditorTool->Elements()->PointElements())
//                {
//                    if(element->PointIndex() == pointIndex)
//                        element->SetVisible(show);
//                }
//            }
//        }
//
//        if(show)
//        {
//            SketchEditorTool->Elements()->OnPointsChanged(SketchEditorTool->GetSketch()->Points(),
//                                                          SketchEditorTool->GetSketch()->Segments());
//        }
//    }
//
//    // 处理缩放动作的预览更新
//    void _PointAction_Preview(const Handle<ScaleSketchElementAction>& sender, const ScaleSketchElementAction::EventArgs& args)
//    {
//        double scale = _UpdatePoints(args);
//        _UpdateElements();
//
//        if(!_HudElement)
//        {
//            _HudElement = new LabelHudElement();
//            Add(_HudElement);
//        }
//        _HudElement->SetText("Scale: " + std::to_string(static_cast<int>(scale * 100)) + "%");
//    }
//
//    // 处理缩放动作完成
//    void _PointAction_Finished(const Handle<ScaleSketchElementAction>& sender, const ScaleSketchElementAction::EventArgs& args)
//    {
//        if(_HudElement)
//        {
//            Remove(_HudElement);
//            _HudElement = nullptr;
//        }
//
//        _UpdatePoints(args);
//
//        // 删除原始线段
//        for(const auto& seg : _OriginalSegments)
//            SketchEditorTool->GetSketch()->DeleteSegment(seg);
//
//        // 清除选中的点
//        SketchEditorTool->SelectedPoints().clear();
//
//        // 添加缩放后的点和线段
//        SketchEditorTool->GetSketch()->AddElements(_TempPoints, {}, _TempSegments, {});
//
//        // 清除临时数据
//        _OriginalSegments.clear();
//        _TempPoints.clear();
//        _TempSegments.clear();
//
//        // 提交撤销历史
//        InteractiveContext::Current()->UndoHandler()->Commit();
//
//        Stop();
//    }
//
//    // 更新点位置，计算缩放变换
//    double _UpdatePoints(const ScaleSketchElementAction::EventArgs& args)
//    {
//        if(_TempPoints.empty())
//        {
//            _TempPoints = SketchEditorTool->GetSketch()->Points();
//        }
//
//        gp_XY scaleCenter = args.Axis.Location().Coord();
//        double scaleFactor = args.Scaled / args.Reference;
//
//        if(!InteractiveContext::IsModifierKeyPressed(ModifierKey::Shift))
//        {
//            scaleCenter.Add(args.Axis.Direction().Coord().Multiplied(-args.Reference));
//            scaleFactor = (args.Scaled + args.Reference) / (args.Reference * 2.0);
//        }
//
//        if(InteractiveContext::IsModifierKeyPressed(ModifierKey::Control))
//        {
//            scaleFactor = MathUtils::RoundToNearest(scaleFactor, 0.05);
//        }
//
//        scaleFactor = MathUtils::Clamp(scaleFactor, 0.01, 10000.0);
//        gp_XY scale(args.Axis.Direction().X() == 0 ? 1.0 : scaleFactor,
//                    args.Axis.Direction().Y() == 0 ? 1.0 : scaleFactor);
//
//        const auto& sketchPoints = SketchEditorTool->GetSketch()->Points();
//        for(int i : _SelectedPoints)
//        {
//            gp_XY coord = sketchPoints.at(i).Coord();
//            coord.Subtract(scaleCenter);
//            coord.Multiply(scale);
//            coord.Add(scaleCenter);
//            _TempPoints[i] = gp_Pnt2d(coord);
//        }
//
//        return scaleFactor;
//    }
//
//    // 更新预览元素，显示缩放效果
//    void _UpdateElements()
//    {
//        if(!_PreviewIsValid)
//        {
//            // 隐藏原始元素
//            _UpdateSegmentElementsVisibility(false);
//
//            // 创建点预览元素
//            const auto& sketchPoints = SketchEditorTool->GetSketch()->Points();
//            for(const auto& point : _SelectedPoints)
//            {
//                _PreviewPointElements[point] = new SketchElementPoint(point, gp::Origin2d());
//            }
//
//            // 创建线段预览元素
//            for(const auto& [key, seg] : _TempSegments)
//            {
//                _PreviewSegmentElements[key] = new SketchElementSegment(seg);
//            }
//
//            _PreviewIsValid = true;
//        }
//
//        // 更新点预览
//        for(const auto& point : _SelectedPoints)
//        {
//            _PreviewPointElements[point]->OnPointsChanged(_TempPoints, {});
//        }
//
//        // 更新线段预览
//        for(const auto& [key, seg] : _TempSegments)
//        {
//            _PreviewSegmentElements[key]->OnPointsChanged(_TempPoints, {});
//        }
//    }
//
//private:
//    std::vector<int> _SelectedPoints; // 选中的点索引
//    std::vector<Handle<SketchSegment>> _OriginalSegments; // 原始线段
//    std::map<int, gp_Pnt2d> _TempPoints; // 缩放后的临时点
//    std::map<int, Handle<SketchSegment>> _TempSegments; // 缩放后的临时线段
//    std::map<int, Handle<SketchElementPoint>> _PreviewPointElements; // 预览点元素
//    std::map<int, Handle<SketchElementSegment>> _PreviewSegmentElements; // 预览线段元素
//    Handle<ScaleSketchElementAction> _ToolAction; // 缩放动作
//    Handle<LabelHudElement> _HudElement; // HUD元素，显示缩放比例
//    bool _PreviewIsValid; // 预览有效标志
//
//public:
//    static Signal<void()> Stoped;
//};
//
//#endif // SKETCHTOOLSCALE_HXX
