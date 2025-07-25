// Copyright [2025] TaCAD

#ifndef OCCT_OCCTEXTENSIONS_AIS_VIEWCUBEEX_H_
#define OCCT_OCCTEXTENSIONS_AIS_VIEWCUBEEX_H_

// Occt includes
#include <AIS_ViewCube.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Image_PixMap.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <V3d_TypeOfOrientation.hxx>

DEFINE_STANDARD_HANDLE(AIS_ViewCubeEx, AIS_ViewCube)

class AIS_ViewCubeEx : public AIS_ViewCube
{
	DEFINE_STANDARD_RTTIEXT(AIS_ViewCubeEx, AIS_ViewCube)

public:
	void Compute(const opencascade::handle<PrsMgr_PresentationManager>& thePrsMgr,
	             const opencascade::handle<Prs3d_Presentation>& thePrs,
	             const Standard_Integer theMode) override;

	void HilightOwnerWithColor(const Handle<PrsMgr_PresentationManager>& thePrsMgr,
	                           const Handle<Prs3d_Drawer>& theStyle,
	                           const Handle<SelectMgr_EntityOwner>& theOwner) override;
	
	void SetTexture(const opencascade::handle<Image_PixMap>& thePixMap);
	
protected:
	void createBoxSideTriangles(const Handle<Graphic3d_ArrayOfTriangles>& theTris,
	                            Standard_Integer& theNbNodes, Standard_Integer& theNbTris,
	                            V3d_TypeOfOrientation theDirection) const override;
};

#endif // OCCT_OCCTEXTENSIONS_AIS_VIEWCUBEEX_H_
