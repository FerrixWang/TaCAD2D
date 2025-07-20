#ifndef SketchElementSegment_HXX
#define SketchElementSegment_HXX

#include <AIS_Shape.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Label.hxx>
#include <TNaming_NamedShape.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Core/Shapes/Sketch/SketchElement.hxx"
#include "Core/Shapes/Sketch/SketchFactory.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchElementSegment : public SketchElement
{
    DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
    DEFINE_ATTRIBUTE_ON_LABEL(TDataXtd_Shape, Shape)
    DEFINE_ATTRIBUTE_ON_LABEL(TPrsStd_AISPresentation, Present)
    DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_Name, Base)

public:
    SketchElementSegment(const Handle(SketchSegment)& segment,
                         const TDF_Label& label = DocumentTool::SketchElementSegmentsLabel().NewChild())
        : SketchElement(label.FindChild(0))
		, _Label(label)
    {
        assert(!label.HasAttribute());
        ShapeAttribute()->Set(ShapeLabel(), {});

        NameAttribute()->Set(NameLabel(), "SketchElementSegment");

        ReferenceAttribute()->Set(ReferenceLabel(), segment->InstanceLabel());

        PresentAttribute()->Set(PresentLabel(), TNaming_NamedShape::GetID());
        PresentAttribute()->SetColor(Quantity_NOC_WHITE);
    }

    //! Constructor
    SketchElementSegment(const TDF_Label& label)
        : SketchElement(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    virtual void OnPointsChanged(std::map<int, gp_Pnt2d> points, 
                                 std::map<int, Handle(SketchSegment)> segments) override
    {
        auto segment = SketchFactory::Wrap<SketchSegment>(ReferenceAttribute()->Get());
        ShapeAttribute()->Set(ShapeLabel(), segment->MakeEdge(points));
        PresentAttribute()->SetDisplayed(true);
        PresentAttribute()->Update();
        PresentAttribute()->Display(true);

        auto hintEdge = segment->CreateHintEdge(points);
        if(hintEdge.IsNull()) return;

		HintShapeAttribute()->Set(HintShapeLabel(), hintEdge);
		HintPresentAttribute()->Update();
        auto aisObject = HintPresentAttribute()->GetAIS();
        if(!aisObject.IsNull())
        {
            aisObject->Attributes()->SetWireAspect(HintAspect());
            HintPresentAttribute()->Display();
            aisObject->GetContext()->Deactivate(aisObject);
        }
    }

    TDF_Label Label() const { return _Label; }

private:
    TDF_Label _Label;
};

DEFINE_STANDARD_HANDLE(SketchElementSegment, SketchElement)

#endif // SketchElementSegment_HXX
