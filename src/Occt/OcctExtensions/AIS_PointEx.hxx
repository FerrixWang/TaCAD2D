// Copyright [2025] TaCAD

#ifndef OCCT_OCCTEXTENSIONS_AIS_POINTEX_H_
#define OCCT_OCCTEXTENSIONS_AIS_POINTEX_H_

// Occt includes
#include <AIS_Point.hxx>
#include <Geom_Point.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Quantity_Color.hxx>

class AIS_PointEx : public AIS_Point
{
	DEFINE_STANDARD_RTTIEXT(AIS_PointEx, AIS_Point)

public:
	AIS_PointEx(const Handle(Geom_Point)& aComponent);

    void EnableBackground(double theTransparency);
	void SetBackgroundColor(const Quantity_Color& theColor);
    void Compute(const opencascade::handle<PrsMgr_PresentationManager>&,
                 const opencascade::handle<Prs3d_Presentation>& aPresentation, Standard_Integer aMode) override;

private:
    Handle(Prs3d_Drawer) myBackgroundDrawer;
};

DEFINE_STANDARD_HANDLE(AIS_PointEx, AIS_Point)

#endif  // OCCT_OCCTEXTENSIONS_AIS_POINTEX_H_
