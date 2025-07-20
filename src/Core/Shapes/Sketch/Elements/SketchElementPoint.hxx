#ifndef SketchElementPoint_HXX
#define SketchElementPoint_HXX

#include <AIS_Point.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Core/Shapes/Sketch/SketchElement.hxx"

class SketchElementPoint : public SketchElement
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Integer, Index)
    DEFINE_ATTRIBUTE_ON_LABEL(TDataXtd_Point, Point)
    DEFINE_ATTRIBUTE_ON_LABEL(TPrsStd_AISPresentation, Present)
    DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_Name, Base)

public:
    SketchElementPoint(int index, const gp_Pnt2d& point, 
                       const TDF_Label& label = DocumentTool::SketchElementPointsLabel().NewChild())
        : SketchElement(label.FindChild(0))
        , _Label(label)
    {
        assert(!label.HasAttribute());

        NameAttribute()->Set(NameLabel(), "SketchElementPoint");
        IndexAttribute()->Set(IndexLabel(), index);
        PointAttribute()->Set(PointLabel(), gp_Pnt(point.X(), point.Y(), 0));
        auto present = PresentAttribute()->Set(PresentLabel(), TDataXtd_Point::GetID());
        present->SetColor(Quantity_NOC_YELLOW);
        present->SetWidth(1.5);
    }

    //! Constructor
    SketchElementPoint(const TDF_Label& label)
        : SketchElement(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    virtual void OnPointsChanged(std::map<int, gp_Pnt2d> points,
                                 std::map<int, Handle(SketchSegment)> segments) override
    {
        auto point = points.at(IndexAttribute()->Get());
        PointAttribute()->Set(PointLabel(), gp_Pnt(point.X(), point.Y(), 0));
        PresentAttribute()->Set(PresentLabel(), TDataXtd_Point::GetID());
        auto present = PresentAttribute();
        Handle(AIS_Point)::DownCast(present->GetAIS())->SetMarker(Aspect_TOM_BALL);
        present->Display(true);
    }

    TDF_Label Label() const { return _Label; }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchElementPoint, SketchElement)

#endif // SketchElementPoint_HXX
