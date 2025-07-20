#ifndef SketchElementConstraint_HXX
#define SketchElementConstraint_HXX

#include <algorithm>
#include <map>
#include <vector>

#include <TPrsStd_AISPresentation.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt2d.hxx>
#include <TDataStd_Name.hxx>
#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/SketchElement.hxx"


class SketchElementConstraint : public SketchElement
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
    DEFINE_ATTRIBUTE_ON_LABEL(TPrsStd_AISPresentation, Present)
    DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_Name, Base)

public:
    SketchElementConstraint(int index, const TDF_Label& label)
        : SketchElement(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());
    }

    //! Constructor
    SketchElementConstraint(const TDF_Label& label)
        : SketchElement(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return _Label; }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchElementConstraint, SketchElement)

#endif // SketchElementConstraint_HXX
