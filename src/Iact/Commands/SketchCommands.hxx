#ifndef _SketchCommands_Header_File_
#define _SketchCommands_Header_File_

#include <string>

#include <QObject>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintAngle.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintConcentric.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintEqual.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHorizontal.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHorizontalDistance.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintLength.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintParallel.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPerpendicular.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPointOnMidpoint.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPointOnSegment.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintRadius.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintSmoothCorner.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintTangent.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintVertical.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintVerticalDistance.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Iact/Commands/CommandHelper.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Sketch/CreateSketchTool.hxx"
#include "Iact/Shapes/Sketch/SketchEditorTool.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentArcCenterCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentArcRimCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentBezier2Creator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentBezier3Creator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentCircleCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentEllipseCenterCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentEllipticalArcCenterCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentLineCreator.hxx"
#include "Iact/Shapes/Sketch/SketchSegmentRectangleCreator.hxx"
#include "Iact/Shapes/Sketch/SketchToolMirror.hxx"
#include "Pres/Commands/ActionCommand.hxx"

class SketchCommands
{
public:
    static ActionCommand* CreateSegmentLine()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentLineCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Line"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentLine"));
            command.setToolTip(QObject::tr("Creates a new Line."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentLineCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreatePolyLine()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentLineCreator>(true); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("PolyLine"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentPolyLine"));
            command.setToolTip(QObject::tr("Creates multiple continuous segments."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentLineCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateCircle()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentCircleCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Circle"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentCircle"));
            command.setToolTip(QObject::tr("Creates a new Circle."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentCircleCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateRectangle()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentRectangleCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Rectangle"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentRectangle"));
            command.setToolTip(QObject::tr("Creates a new Rectangle."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentRectangleCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateBezier2()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentBezier2Creator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Quadratic\nBezier"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentBezier2"));
            command.setToolTip(QObject::tr("Creates a new quadratic bezier curve."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentBezier2Creator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateBezier3()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentBezier3Creator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Cubic\nBezier"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentBezier3"));
            command.setToolTip(QObject::tr("Creates a new cubic bezier curve."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentBezier3Creator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateArcCenter()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentArcCenterCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Arc using \nCenter"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentArcCenter"));
            command.setToolTip(QObject::tr("Creates an arc from its center point."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentArcCenterCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateArcRim()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentArcRimCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Arc using \nRim"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentArcRim"));
            command.setToolTip(QObject::tr("Create an arc by selecting a rim point."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentArcRimCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateEllipse()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentEllipseCenterCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Ellipse"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentEllipseCenter"));
            command.setToolTip(QObject::tr("Create an ellipse."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentEllipseCenterCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateEllipticalArc()
    {
        static ActionCommand command(
            [](const auto&) { _StartOrStopSegmentCreator<SketchSegmentEllipticalArcCenterCreator>(false); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Elliptical \nArc"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-SegmentEllipticalArcCenter"));
            command.setToolTip(QObject::tr("Create an ellipse arc."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchSegmentEllipticalArcCenterCreator::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintParallel()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintParallel>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintParallel"));
            command.setToolTip(QObject::tr("Creates a new parallel constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintVertical()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintVertical>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintVertical"));
            command.setToolTip(QObject::tr("Creates a new vertical constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintHorizontal()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintHorizontal>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintHorizontal"));
            command.setToolTip(QObject::tr("Creates a new horizontal constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintLength()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintLength>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintLineLength"));
            command.setToolTip(QObject::tr("Creates a new constraint with line length."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintAngle()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintAngle>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintAngle"));
            command.setToolTip(QObject::tr("Creates a new constraint with angle."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintEqual()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintEqual>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintEqual"));
            command.setToolTip(QObject::tr("Creates a new constraint with equal length."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintFixed()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintFixed>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintFixed"));
            command.setToolTip(QObject::tr("Creates a new fixed constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintHorizontalDistance()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintHorizontalDistance>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintHorizontalDistance"));
            command.setToolTip(QObject::tr("Creates a new horizontal distance constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintPerpendicular()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintPerpendicular>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintPerpendicular"));
            command.setToolTip(QObject::tr("Creates a new perpendicular constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintPointOnMidpoint()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintPointOnMidpoint>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintPointOnMidpoint"));
            command.setToolTip(QObject::tr("Creates a new constraint to place a point on a segment's midpoint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintPointOnSegment()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintPointOnSegment>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintPointOnSegment"));
            command.setToolTip(QObject::tr("Creates a new constraint to place a point on a segment."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintRadius()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintRadius>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintCircleRadius"));
            command.setToolTip(QObject::tr("Creates a new radius constraint for circles or arcs."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintSmoothCorner()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintSmoothCorner>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintSmoothCorner"));
            command.setToolTip(QObject::tr("Creates a new constraint for a smooth corner between segments."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintTangent()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintTangent>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintTangent"));
            command.setToolTip(QObject::tr("Creates a new tangent constraint between segments."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintConcentric()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintConcentric>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintConcentric"));
            command.setToolTip(QObject::tr("Creates a new concentric constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* CreateConstraintVerticalDistance()
    {
        static ActionCommand command(
            [](const auto&) { _SketchEditorTool()->CreateConstraint<SketchConstraintVerticalDistance>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.toolTip().isEmpty())
        {
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-ConstraintVerticalDistance"));
            command.setToolTip(QObject::tr("Creates a new vertical distance constraint."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchEditorTool::ConstraintCreated.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

    static ActionCommand* StartSketchToolMirror()
    {
        static ActionCommand command(
            [](const auto&) { _StartTool<SketchToolMirror>(); },
            [](const auto&) { return CommandHelper::CurrentToolIs<SketchEditorTool>(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Mirror"));
            command.setIcon(ResourceUtils::icon("Sketch/Sketch-MirrorTool"));
            command.setToolTip(QObject::tr("Mirrors selected points or segments by defining the mirror axis."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, [](bool checked) {command.execute(); });
            SketchToolMirror::Stoped.connect([]() { command.setChecked(false); });
        }
        return &command;
    }

private:
    template<typename T>
    static Handle(SketchEditorTool) _StartOrStopTool()
    {
        auto sketchEditTool = _SketchEditorTool();
        if(sketchEditTool.IsNull())
            return nullptr;

        auto currentTool = Handle(T)::DownCast(sketchEditTool->CurrentTool());
        if(!currentTool.IsNull())
        {
            sketchEditTool->StopTool();
            return nullptr;
        }
        return sketchEditTool;
    }

    template<typename T>
    static bool _StartTool()
    {
        auto sketchEditTool = _SketchEditorTool();
        if(sketchEditTool.IsNull()) return false;
        sketchEditTool->StartTool(new T);
        return true;
    }

    template<typename T>
    static bool _StartOrStopSegmentCreator(bool continuesModeEnabled)
    {
        auto sketchEditTool = _SketchEditorTool();
        if(sketchEditTool.IsNull()) return false;
        return sketchEditTool->StartSegmentCreation<T>(continuesModeEnabled);
    }

    static Handle(SketchEditorTool) _SketchEditorTool()
    {
        return Handle(SketchEditorTool)::DownCast(
            InteractiveContext::Current()->GetWorkspaceController()->CurrentTool()
        );
    }
};

#endif  // _SketchCommands_Header_File_
