// Copyright [2025] TaCAD

#ifndef PRES_COMMANDS_ACTIONCOMMAND_H_
#define PRES_COMMANDS_ACTIONCOMMAND_H_

#include <functional>

#include <QAction>

#include "Pres/Commands/RelayCommand.hxx"

class ActionCommand final : public QAction, public RelayCommand
{
    Q_OBJECT

public:
    // Constructor
    ActionCommand(std::function<void(const std::any&)> execute = nullptr, std::function<bool(const std::any&)> canExecute = nullptr);
};

#endif  // PRES_COMMANDS_ACTIONCOMMAND_H_
