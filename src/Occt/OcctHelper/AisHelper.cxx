// Copyright [2025] TaCAD

// Own include
#include "Occt/OcctHelper/AisHelper.hxx"

// Occt includes
#include <PrsMgr_PresentableObject.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>

void AisHelper::DisableGlobalClipPlanes(const Handle(PrsMgr_PresentableObject)& object)
{
	auto seq = new Graphic3d_SequenceOfHClipPlane();
	seq->SetOverrideGlobal(true);
	object->SetClipPlanes(seq);
}

TopoDS_Shape AisHelper::GetShapeFromEntityOwner(const Handle(SelectMgr_EntityOwner) owner)
{
	if (owner.IsNull())
		return {};

	auto brepOwner = Handle(StdSelect_BRepOwner)::DownCast(owner);
	if (brepOwner.IsNull() || !brepOwner->HasShape())
		return {};

	return TopoDS_Shape(brepOwner->Shape());
}

int AisHelper::GetPickedFromSelector(const Handle(StdSelect_ViewerSelector3d)& selector,
                          std::vector<Handle(AIS_InteractiveObject)>& interactives,
                          std::vector<TopoDS_Shape>& shapes)
{
    int count = 0;
    bool doInteractives = !interactives.empty();
    bool doShapes = !shapes.empty();

    // 清空输出向量
    interactives.clear();
    shapes.clear();

    // 遍历拾取结果
    for(Standard_Integer aPickIter = 1; aPickIter <= selector->NbPicked(); ++aPickIter)
    {
        Handle(SelectMgr_EntityOwner) owner = selector->Picked(aPickIter);
        if(owner.IsNull() || !owner->HasSelectable())
        {
            continue;
        }

        count++;

        // 处理交互对象
        if(doInteractives)
        {
            Handle(AIS_InteractiveObject) interactive =
                Handle(AIS_InteractiveObject)::DownCast(owner->Selectable());
            interactives.push_back(interactive); // 空 Handle 表示 nullptr
        }

        // 处理形状
        if(doShapes)
        {
            Handle(StdSelect_BRepOwner) brepOwner =
                Handle(StdSelect_BRepOwner)::DownCast(owner);
            if(!brepOwner.IsNull() && brepOwner->HasShape())
            {
                shapes.push_back(brepOwner->Shape());
            }
            else
            {
                shapes.push_back(TopoDS_Shape()); // 空形状表示 nullptr
            }
        }
    }

    return count;
}