//! class SketchEditorPointElement
//! 

#ifndef SKETCHEDITORPOINTELEMENT_HXX
#define SKETCHEDITORPOINTELEMENT_HXX

#include <Geom_CartesianPoint.hxx>
#include <map>
#include <Standard.hxx>
#include <Standard_Transient.hxx>

#include "Comm/Handle.hxx"
#include "Iact/Shapes/Sketch/SketchEditorElement.hxx"
#include "Iact/Visual/Marker.hxx"

class SketchEditorPointElement : public SketchEditorElement
{
public:
    // Constructor
    SketchEditorPointElement(const Handle(SketchEditorTool)& sketchEditorTool,
                             int pointIndex,
                             const gp_Pnt2d& point,
                             const gp_Trsf& transform,
                             const gp_Pln& plane);

    // Virtual methods from SketchEditorElement
    void UpdateVisual() override;
    void OnPointsChanged(const std::map<int, gp_Pnt2d>& points,
                         const std::map<int, Handle(SketchSegment)>& segments,
                         std::map<int, int>& markerCounts) override;
    void Activate(bool selectable) override;
    void Remove() override;
    bool IsOwnerOf(const Handle(AIS_InteractiveObject)& aisObject) const override;

    // Getter
    int PointIndex() const { return _PointIndex; }

private:
    int _PointIndex;
    Handle(Marker) _Marker;
};

DEFINE_STANDARD_HANDLE(SketchEditorPointElement, SketchEditorElement)

#endif // SKETCHEDITORPOINTELEMENT_HXX

