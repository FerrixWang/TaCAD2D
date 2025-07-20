//! Color 
//! 

#include <Standard_Real.hxx>
#include <Quantity_Color.hxx>

namespace Colors
{
// 基础颜色
const Quantity_Color Default(0.75, 0.75, 0.75, Quantity_TOC_RGB); // #c0c0c0
const Quantity_Color Selection(0.98f, 0.922f, 0.843f, Quantity_TOC_RGB);
const Quantity_Color Highlight(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB);
const Quantity_Color FilteredSubshapes(0.0f, 0.698f, 0.933f, Quantity_TOC_RGB);
const Quantity_Color FilteredSubshapesHot(1.0f, 0.0f, 0.0f, Quantity_TOC_RGB);
const Quantity_Color Ghost(0.827f, 0.827f, 0.827f, Quantity_TOC_RGB);
const Quantity_Color Auxillary(0.251f, 0.251f, 0.251f, Quantity_TOC_RGB);
const Quantity_Color Marker(1.0f, 1.0f, 0.0f, Quantity_TOC_RGB);

// 属性标记
const Quantity_Color AttributeMarkerBackground(0.2f, 0.3f, 0.6f, Quantity_TOC_RGB);
const Quantity_Color AttributeMarkerSelection(0.7f, 0.3f, 0.3f, Quantity_TOC_RGB);

// 草图编辑器
const Quantity_Color SketchEditorSegments(1.0f, 1.0f, 1.0f, Quantity_TOC_RGB); // 白色
const Quantity_Color SketchEditorHighlight(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB);
const Quantity_Color SketchEditorSelection(1.0f, 0.0f, 0.0f, Quantity_TOC_RGB);
const Quantity_Color SketchEditorCreating(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB);
const Quantity_Color SketchEditorAuxillary(0.0f, 0.604f, 0.804f, Quantity_TOC_RGB);

// 操作颜色
const Quantity_Color ActionBlue(0.2f, 0.2f, 0.8f, Quantity_TOC_RGB);
const Quantity_Color ActionRed(0.8f, 0.2f, 0.2f, Quantity_TOC_RGB);
const Quantity_Color ActionGreen(0.2f, 0.8f, 0.2f, Quantity_TOC_RGB);
const Quantity_Color ActionWhite(0.8f, 0.8f, 0.8f, Quantity_TOC_RGB);
}
