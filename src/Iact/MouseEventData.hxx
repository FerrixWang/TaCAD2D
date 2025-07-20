//! class MouseEventData
//! 

#ifndef IACT_MOUSEEVENTDATA_HXX
#define IACT_MOUSEEVENTDATA_HXX

#include <AIS_InteractiveObject.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Graphic3d_Vec2.hxx>

#include <string>
#include <vector>

#include "Core/Topology/InteractiveEntity.hxx"
#include "Core/Viewport.hxx"


// 鼠标事件数据类
class MouseEventData : public Standard_Transient
{
public:
    // 元素结构体
    struct Element
    {
        Handle(AIS_InteractiveObject) aisObject;
        Handle(InteractiveEntity) entity;
        TopoDS_Shape brepShape;

        Element(Handle(AIS_InteractiveObject) ais = nullptr, Handle(InteractiveEntity) ent = nullptr, const TopoDS_Shape& shape = TopoDS_Shape())
            : aisObject(ais), entity(ent), brepShape(shape)
        {}

        Element(Handle(AIS_InteractiveObject) ais, const TopoDS_Shape& shape)
            : aisObject(ais), brepShape(shape), entity(nullptr)
        {}

        Element(Handle(InteractiveEntity) ent)
            : entity(ent), aisObject(nullptr)
        {}
    };

    // 返回选项类
    class ReturnOptions
    {
    public:
        std::vector<Element> additionalHighlights;
        bool forceReDetection = false;
        bool removeHighlighting = false;

        void Clear()
        {
            additionalHighlights.clear();
            forceReDetection = false;
            removeHighlighting = false;
        }
    };

    // RTTI 支持
    DEFINE_STANDARD_RTTIEXT(MouseEventData, Standard_Transient)

        MouseEventData() : viewport_(nullptr), modifierKeys_(0) {}

    // 获取和设置方法
    Handle(Viewport) GetViewport() const { return viewport_; }
    Graphic3d_Vec2i GetScreenPoint() const { return screenPoint_; }
    gp_Pnt PointOnPlane() const { return pointOnPlane_; }
    unsigned int GetModifierKeys() const { return modifierKeys_; }
    const std::vector<Element>& GetDetectedElements() const { return detectedElements_; }
    ReturnOptions& GetReturnOptions() { return return_; }

    // 计算拾取轴
    gp_Ax1 GetPickAxis() const;

    // 便捷访问检测到的元素
    Handle(InteractiveEntity) GetDetectedEntity() const
    {
        return !detectedElements_.empty() ? detectedElements_[0].entity : nullptr;
    }

    TopoDS_Shape GetDetectedBrepShape() const
    {
        return !detectedElements_.empty() ? detectedElements_[0].brepShape : TopoDS_Shape();
    }

    Handle(AIS_InteractiveObject) GetDetectedAisObject() const
    {
        return !detectedElements_.empty() ? detectedElements_[0].aisObject : nullptr;
    }

    // 清理数据
    void Clear()
    {
        viewport_ = nullptr;
        screenPoint_ = Graphic3d_Vec2i();
        pointOnPlane_ = gp_Pnt();
        detectedElements_.clear();
        return_.Clear();
    }

    // 设置基本数据
    void Set(const Handle(Viewport)& viewport, const Graphic3d_Vec2i& screenPoint, const gp_Pnt& pointOnPlane, Aspect_VKeyFlags modifierKeys)
    {
        Clear();
        viewport_ = viewport;
        screenPoint_ = screenPoint;
        pointOnPlane_ = pointOnPlane;
        modifierKeys_ = modifierKeys;
    }

    // 设置检测到的元素（多元素版本）
    void SetDetectedElements(const std::vector<Handle(AIS_InteractiveObject)>& aisObjects,
                             const std::vector<Handle(InteractiveEntity)>& entities,
                             const std::vector<TopoDS_Shape>& brepShapes)
    {
        detectedElements_.clear();
        size_t maxSize = std::max({aisObjects.size(), entities.size(), brepShapes.size()});

        for(size_t i = 0; i < maxSize; ++i)
        {
            Handle(AIS_InteractiveObject) ais = (i < aisObjects.size()) ? aisObjects[i] : nullptr;
            Handle(InteractiveEntity) ent = (i < entities.size()) ? entities[i] : nullptr;
            TopoDS_Shape shape = (i < brepShapes.size()) ? brepShapes[i] : TopoDS_Shape();
            detectedElements_.emplace_back(ais, ent, shape);
        }
    }

    // 设置单个检测元素
    void SetDetectedElement(Handle(AIS_InteractiveObject) aisObject, Handle(InteractiveEntity) entity, const TopoDS_Shape& brepShape)
    {
        detectedElements_.clear();
        detectedElements_.emplace_back(aisObject, entity, brepShape);
    }

private:
    Handle(Viewport) viewport_;           // 使用 Handle 管理 Viewport
    Graphic3d_Vec2i screenPoint_;                // 屏幕坐标
    gp_Pnt pointOnPlane_;                 // 平面上的3D点
    Aspect_VKeyFlags modifierKeys_;           // 修饰键（整数表示）
    std::vector<Element> detectedElements_; // 检测到的元素列表
    ReturnOptions return_;                // 返回选项
};

DEFINE_STANDARD_HANDLE(MouseEventData, Standard_Transient)

#endif  // IACT_MOUSEEVENTDATA_HXX
