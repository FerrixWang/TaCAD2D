// Copyright [2025] TaCAD

#ifndef OCCT_AISEXTENSIONS_AISX_GRID_H_
#define OCCT_AISEXTENSIONS_AISX_GRID_H_

#include "gp_XY.hxx"
#include "gp_Ax3.hxx"
#include "AIS_InteractiveObject.hxx"
#include "Graphic3d_AspectLine3d.hxx"
#include "Graphic3d_Group.hxx"

class AISX_Grid : public AIS_InteractiveObject
{
public:
    AISX_Grid();

    //--------------------------------------------------------------------------------------------------

    void Compute(const Handle(PrsMgr_PresentationManager)& thePrsMgr, const Handle(Prs3d_Presentation)& thePrs, const Standard_Integer theMode) override;
    void ComputeSelection(const Handle(SelectMgr_Selection)& theSelection, const Standard_Integer theMode) override;

    //--------------------------------------------------------------------------------------------------

    void SetExtents(double x, double y);
    void SetPosition(const gp_Ax3& position);
    void SetDivisions(double linearStep, int angularCount);

    //--------------------------------------------------------------------------------------------------

    DEFINE_STANDARD_RTTIEXT(AISX_Grid, AIS_InteractiveObject)

        //--------------------------------------------------------------------------------------------------

private:
    void _ComputeRectangular(const Handle(Graphic3d_Group)& theGroup);
    void _ComputeCircular(const Handle(Graphic3d_Group)& theGroup);
    void _ComputeAxes(const Handle(Graphic3d_Group)& theGroup);
    void _InitAttributes();

private:
    gp_Ax3 _Position;
    gp_XY _Extents;
    double _LinearStep;
    int _AngularCount;

    Handle(Graphic3d_AspectLine3d) _MinorAspect;
    Handle(Graphic3d_AspectLine3d) _MajorAspect;
    Handle(Graphic3d_AspectLine3d) _XAxisAspect;
    Handle(Graphic3d_AspectLine3d) _YAxisAspect;
};

#endif // !OCCT_AISEXTENSIONS_AISX_GRID_H_
