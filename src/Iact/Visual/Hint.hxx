#ifndef _Hint_HeaderFile
#define _Hint_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include "Iact/Visual/VisualObject.hxx"

class WorkspaceController;

// HintStyle enum
enum class HintStyle
{
    Styles = 0xf,
    ThinDashed = 1,
    WorkingAxis = 2,
    Dashed = 3,
    Solid = 4,

    Boundary = 1 << 14,
    Translucent = 1 << 15,
    Topmost = 1 << 16,
    NoResize = 1 << 18
};
DEFINE_ENUM_FLAGS(HintStyles, HintStyle)

// Hint class
class Hint : public VisualObject
{
protected:
    Hint() : VisualObject() {}

    // Set aspects for AIS_InteractiveObject based on HintStyle
    void SetAspects(const Handle(AIS_InteractiveObject)& obj, const HintStyles& style)
    {
        Standard_ASSERT(!obj.IsNull(), "AIS_InteractiveObject must not be null", );

        Handle(Prs3d_Drawer) drawer = obj->Attributes();

        auto styles = style & HintStyle::Styles;

        if(styles & HintStyle::ThinDashed)
        {
            drawer->SetLineAspect(new Prs3d_LineAspect(Quantity_Color(0.753f, 0.753f, 0.753f, Quantity_TOC_RGB), Aspect_TOL_DASH, 0.5));
            obj->SetTransparency(0.5);
        }
        else if(styles & HintStyle::WorkingAxis)
        {
            drawer->SetLineAspect(new Prs3d_LineAspect(Quantity_Color(1.0f, 1.0f, 0.0f, Quantity_TOC_RGB), Aspect_TOL_DASH, 2.0));
        }
        else if(styles & HintStyle::Dashed)
        {
            drawer->SetLineAspect(new Prs3d_LineAspect(Quantity_Color(0.753f, 0.753f, 0.753f, Quantity_TOC_RGB), Aspect_TOL_DASH, 2.0));
            obj->SetTransparency(0.5);
        }
        else if(styles & HintStyle::Solid)
        {
            drawer->SetLineAspect(new Prs3d_LineAspect(Quantity_Color(0.753f, 0.753f, 0.753f, Quantity_TOC_RGB), Aspect_TOL_SOLID, 2.0));
            obj->SetTransparency(0.5);
        }

        drawer->SetFaceBoundaryDraw((style & HintStyle::Boundary) == HintStyle::Boundary);

        if(style & HintStyle::Translucent)
        {
            obj->SetTransparency(0.75);
        }

        if(style & HintStyle::Topmost)
        {
            obj->SetZLayer(-3); // TOPMOST
        }
    }
};

DEFINE_STANDARD_HANDLE(Hint, VisualObject)

#endif // _Hint_HeaderFile
