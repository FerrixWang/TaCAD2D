#include "Iact/MouseEventData.hxx"

#include "Core/Viewport.hxx"

IMPLEMENT_STANDARD_RTTIEXT(MouseEventData, Standard_Transient)

// ¼ÆËãÊ°È¡Öá

gp_Ax1 MouseEventData::GetPickAxis() const
{
    if(!viewport_.IsNull() && !viewport_->V3dView().IsNull())
    {
        return viewport_->GetViewAxis((Standard_Integer)screenPoint_.x(), (Standard_Integer)screenPoint_.y());
    }
    return gp_Ax1();
}
