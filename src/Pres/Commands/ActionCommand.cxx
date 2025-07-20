// Copyright [2025] TaCAD

// Own include
#include "Pres/Commands/ActionCommand.hxx"

// Constructor
ActionCommand::ActionCommand(std::function<void(const std::any&)> execute, std::function<bool(const std::any&)> canExecute)
    : QAction()
    , RelayCommand(execute, canExecute)
{
}
