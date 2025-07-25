//! class CreateBoxTool

#ifndef _CreateBoxTool_Header_File_
#define _CreateBoxTool_Header_File_

#include "Core/Shapes/Primitives/Box.hxx"
#include "Iact/Framework/Tool.hxx"
#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/HudElements/MultiValueHudElement.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/ToolActions/PointAction.hxx"
#include "Xcaf/Attributes/BoxAttribute.hxx"
#include "Xcaf/Attributes/BoxTool.hxx"

class CreateBoxTool : public Tool
{
public:
    enum Phase
    {
        PivotPoint,
        BaseRect,
        Height
    };

    CreateBoxTool() = default;

    void _PivotAction_Preview(const Handle<PointAction>& action);
    void _PivotAction_Finished(const Handle<PointAction>& action);
    void _BaseRectAction_Preview(const Handle<PointAction>& action);
    void _BaseRectAction_Finished(const Handle<PointAction>& action);
    void _HeightAction_Preview(const Handle<PointAction>& action);
    void _HeightAction_Finished(const Handle<PointAction>& action);

protected:
    bool onStart() override;
    void ensurePreviewShape();
private:
    Phase myCurrentPhase;

    Handle<NewBox> m_Box;
    gp_Pnt _PointPlane1;
    gp_Pnt _PointPlane2;
	gp_Pln _Plane;
	Coord2DHudElement* _Coord2DHudElement = nullptr;
	MultiValueHudElement* _MultiValueHudElement = nullptr;
};

#endif  // _CreateBoxTool_Header_File_
