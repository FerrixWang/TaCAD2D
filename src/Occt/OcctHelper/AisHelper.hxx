// Copyright [2025] TaCAD

#ifndef OCCT_OCCTHELPER_AISHELPER_H_
#define OCCT_OCCTHELPER_AISHELPER_H_

// Occt includes
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TopoDS_Shape.hxx>

class AisHelper
{
public:
	static void DisableGlobalClipPlanes(const Handle(PrsMgr_PresentableObject)& object);
	static TopoDS_Shape GetShapeFromEntityOwner(const Handle(SelectMgr_EntityOwner) owner);

    // 静态函数：从选择器中获取拾取的交互对象和形状
    static int GetPickedFromSelector(const Handle(StdSelect_ViewerSelector3d)& selector,
                                     std::vector<Handle(AIS_InteractiveObject)>& interactives,
                                     std::vector<TopoDS_Shape>& shapes);

};

#endif  // OCCT_OCCTHELPER_AISHELPER_H_
