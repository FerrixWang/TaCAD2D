// Copyright [2025] TaCAD

#ifndef _Color_Header_File_
#define _Color_Header_File_

#include <Standard_Real.hxx>
#include <Quantity_Color.hxx>

namespace Color
{
    // Define basic color constants
    const Quantity_Color Default(0.75, 0.75, 0.75, Quantity_TOC_RGB);    // Default color #c0c0c0
    const Quantity_Color Selection(0.98f, 0.922f, 0.843f, Quantity_TOC_RGB); // Selection color
    const Quantity_Color Highlight(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB); // Highlight color
    const Quantity_Color FilteredSubshapes(0.0f, 0.698f, 0.933f, Quantity_TOC_RGB); // Filtered subshapes color
    const Quantity_Color FilteredSubshapesHot(1.0f, 0.0f, 0.0f, Quantity_TOC_RGB); // Filtered subshapes hot color
    const Quantity_Color Ghost(0.827f, 0.827f, 0.827f, Quantity_TOC_RGB); // Ghost color
    const Quantity_Color Auxillary(0.251f, 0.251f, 0.251f, Quantity_TOC_RGB); // Auxiliary color
    const Quantity_Color Marker(1.0f, 1.0f, 0.0f, Quantity_TOC_RGB); // Marker color

    // Define attribute marker colors
    const Quantity_Color AttributeMarkerBackground(0.2f, 0.3f, 0.6f, Quantity_TOC_RGB); // Attribute marker background color
    const Quantity_Color AttributeMarkerSelection(0.7f, 0.3f, 0.3f, Quantity_TOC_RGB); // Attribute marker selection color

    // Define sketch editor colors
    const Quantity_Color SketchEditorSegments(1.0f, 1.0f, 1.0f, Quantity_TOC_RGB); // Sketch editor segments color (white)
    const Quantity_Color SketchEditorHighlight(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB); // Sketch editor highlight color
    const Quantity_Color SketchEditorSelection(1.0f, 0.0f, 0.0f, Quantity_TOC_RGB); // Sketch editor selection color
    const Quantity_Color SketchEditorCreating(0.933f, 0.706f, 0.133f, Quantity_TOC_RGB); // Sketch editor creating color
    const Quantity_Color SketchEditorAuxillary(0.0f, 0.604f, 0.804f, Quantity_TOC_RGB); // Sketch editor auxiliary color

    // Define action colors
    const Quantity_Color ActionBlue(0.2f, 0.2f, 0.8f, Quantity_TOC_RGB); // Blue action color
    const Quantity_Color ActionRed(0.8f, 0.2f, 0.2f, Quantity_TOC_RGB); // Red action color
    const Quantity_Color ActionGreen(0.2f, 0.8f, 0.2f, Quantity_TOC_RGB); // Green action color
    const Quantity_Color ActionWhite(0.8f, 0.8f, 0.8f, Quantity_TOC_RGB); // White action color
}

#endif // _Color_Header_File_
