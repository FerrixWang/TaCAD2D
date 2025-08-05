//! class ModelCommands

#ifndef _ModelCommands_Header_File_
#define _ModelCommands_Header_File_

#include <string>

#include <QObject>

#include "Iact/InteractiveContext.hxx"
#include "Iact/Shapes/Primitives/CreateBoxTool.hxx"
#include "Iact/Shapes/Sketch/CreateSketchTool.hxx""
#include "Pres/Commands/ActionCommand.hxx"

class ModelCommands
{
public:
    static ActionCommand* CreateBox()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetWorkspaceController()->StartTool(new CreateBoxTool); },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Box"));
            command.setIcon(ResourceUtils::icon("Model/Prim-Box"));
            command.setToolTip(QObject::tr("Creates a new body with a box shape."));
            command.setCheckable(true);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }
        return &command;
    }

    static ActionCommand* CreateCylinder()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetWorkspaceController()->StartTool(new CreateBoxTool); },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Cylinder"));
            command.setIcon(ResourceUtils::icon("Model/Prim-Cylinder"));
            command.setToolTip(QObject::tr("Creates a new body with a cylinder shape."));
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }
        return &command;
    }

    static ActionCommand* CreateSphere()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetWorkspaceController()->StartTool(new CreateBoxTool); },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Sphere"));
            command.setIcon(ResourceUtils::icon("Model/Prim-Sphere"));
            command.setToolTip(QObject::tr("Creates a new body with a sphere shape."));
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }
        return &command;
    }

    static ActionCommand* CreateSketch()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetWorkspaceController()->StartTool(new CreateSketchTool); },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Sketch"));
            command.setIcon(ResourceUtils::icon("Model/Prim-Sketch"));
            command.setToolTip(QObject::tr("Creates a new body with a sketch shape."));
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }
        return &command;
    }

    static RelayCommand* CreateSketchAligned()
    {
        static RelayCommand command(
            [](const auto&) { ; },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );
        return &command;
    }
};

#endif  // _ModelCommands_Header_File_
