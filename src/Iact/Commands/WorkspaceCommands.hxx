// Copyright [2025] TaCAD

#ifndef IACT_COMMANDS_WORKSPACECOMMANDS_H_
#define IACT_COMMANDS_WORKSPACECOMMANDS_H_

// Project includes
#include "Core/GridTypes.hxx"
#include "Core/Workspace.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/WorkspaceController.hxx"
#include "Pres/Commands/ActionCommand.hxx"

class WorkspaceCommands
{
public:
    static ActionCommand* Escape()
    {
        static ActionCommand command(
            [](const auto&) {
            InteractiveContext::Current()->GetWorkspaceController()->CancelCurrentTool();
        },
            [](const auto&) { return true; }
        );

        // Initialize command properties if not already set
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Stop Current Tool"));
            command.setToolTip(QObject::tr("Stops or cancels the current tool."));
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    // Initialize the static command outside the class
    static ActionCommand* DoUndo()
    {
        static ActionCommand command(
            [](const auto&) {
            auto aDocument = InteractiveContext::Current()->GetDocument();
            auto aWorkspace = InteractiveContext::Current()->workspace();
            auto aViewport = InteractiveContext::Current()->viewport();

            if(aDocument->Undo())
            {
                aDocument->CommitCommand();
                aWorkspace->UpdateCurrentViewer();
                aViewport->OnViewMoved();

                qDebug() << "Undo was done successfully";
            }},
            [](const auto&) { return true; }
        );

        // Initialize command properties if not already set
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Undo"));
            command.setIcon(ResourceUtils::icon("Edit/Edit-Undo"));
            command.setToolTip(QObject::tr("Revert the last operation."));
            command.setCheckable(false);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    // Initialize the static command outside the class
    static ActionCommand* DoRedo()
    {
        static ActionCommand command(
            [](const auto&) {
            auto aDocument = InteractiveContext::Current()->GetDocument();
            auto aWorkspace = InteractiveContext::Current()->workspace();
            auto aViewport = InteractiveContext::Current()->viewport();

            if(aDocument->Redo())
            {
                aDocument->CommitCommand();
                aWorkspace->UpdateCurrentViewer();
                aViewport->OnViewMoved();
            }},
            [](const auto&) { return true; }
        );

        // Initialize command properties if not already set
        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Redo"));
            command.setIcon(ResourceUtils::icon("Edit/Edit-Redo"));
            command.setToolTip(QObject::tr("Redo the last operation."));
            command.setCheckable(false);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    static ActionCommand* setPredefinedView(ViewportController::PredefinedViews value)
    {
        static std::vector<std::shared_ptr<ActionCommand>> commands;

        if(commands.empty())
        {
            for(int i = 0; i < static_cast<int>(ViewportController::PredefinedViews::Count); ++i)
            {
                commands.emplace_back(std::make_shared<ActionCommand>(
                    [i](const auto& param) {
                    auto VC = InteractiveContext::Current()->GetViewportController();
                    VC->SetPredefinedView(static_cast<ViewportController::PredefinedViews>(std::any_cast<int>(param)));
                },
                    [](const auto&) { return !InteractiveContext::Current()->GetViewportController().IsNull(); }
                ));
                commands[i]->setText(QObject::tr(ViewportController::convertToString(static_cast<ViewportController::PredefinedViews>(i)).c_str()));
                commands[i]->setIcon(ResourceUtils::icon(QString("View/View-") + ViewportController::convertToString(static_cast<ViewportController::PredefinedViews>(i)).c_str()));
                commands[i]->setCheckable(false);
                commands[i]->connect(commands[i].get(), &QAction::triggered, [i]() { commands[i]->execute(i); });
            }
        }
        return commands[static_cast<int>(value)].get();
    }

    static ActionCommand* ZoomFitAll()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetViewportController()->ZoomFitAll();
        },
            [](const auto&) { return !InteractiveContext::Current()->GetViewportController().IsNull(); }
        );

        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Zoom All"));
            command.setIcon(ResourceUtils::icon("View/Zoom-All"));
            command.setToolTip(QObject::tr("Adjusts the position and scale of the viewport so all visible entities are in view."));
            command.setCheckable(false);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    static ActionCommand* ZoomIn()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetViewportController()->Zoom(0.5);
        },
            [](const auto&) { return !InteractiveContext::Current()->GetViewportController().IsNull(); }
        );

        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Zoom In"));
            command.setIcon(ResourceUtils::icon("View/Zoom-In"));
            command.setCheckable(false);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    static ActionCommand* ZoomOut()
    {
        static ActionCommand command(
            [](const auto&) { InteractiveContext::Current()->GetViewportController()->Zoom(-0.5);
        },
            [](const auto&) { return !InteractiveContext::Current()->GetViewportController().IsNull(); }
        );

        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Zoom Out"));
            command.setIcon(ResourceUtils::icon("View/Zoom-Out"));
            command.setCheckable(false);
            command.connect(&command, &QAction::triggered, []() {command.execute(); });
        }

        return &command;
    }

    static ActionCommand* ToggleGrid()
    {
        static ActionCommand command(
            [](const auto&) {
            auto ws = InteractiveContext::Current()->GetWorkspaceController()->workspace();
            ws->SetGridEnabled(!ws->IsGridEnabled());
            ws->UpdateCurrentViewer();
        },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );

        if(command.text().isEmpty())
        {
            command.setText(QObject::tr("Show Grid"));
            command.setIcon(ResourceUtils::icon("View/Grid-Rect"));
            command.setToolTip(QObject::tr("Toggles the grid visibility"));
            command.connect(&command, &QAction::triggered, []() {command.execute();});
        }

        return &command;
    }

    static RelayCommand* SetGridType()
    {
        static RelayCommand command(
            [](const auto& param) {
            auto ws = InteractiveContext::Current()->GetWorkspaceController()->workspace();
            ws->SetGridType(std::any_cast<GridTypes>(param));
            ws->UpdateCurrentViewer();
        },
            [](const auto&) { return !InteractiveContext::Current()->GetWorkspaceController().IsNull(); }
        );

        return &command;
    }


    static ActionCommand* SetRenderMode()
    {
        static ActionCommand command(
            [](const auto& mode) {
            auto viewport = InteractiveContext::Current()->GetViewportController()->GetViewport();
            if(!viewport.IsNull())
            {
                viewport->SetRenderMode(std::any_cast<Viewport::RenderModes>(mode));
                viewport->GetWorkspace()->UpdateCurrentViewer();
            }
        },
            [](const auto&) { return true; }
        );

        return &command;
    }
};

#endif  // IACT_COMMANDS_WORKSPACECOMMANDS_H_
