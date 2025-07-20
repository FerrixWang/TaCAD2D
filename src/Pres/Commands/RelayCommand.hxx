// Copyright [2025] TaCAD

#ifndef PRES_COMMANDS_RELAYCOMMAND_H_
#define PRES_COMMANDS_RELAYCOMMAND_H_

// std includes
#include <functional>
#include <utility>

class RelayCommand
{
public:
    // Constructor
	RelayCommand() = default;
    RelayCommand(const RelayCommand& command) {
		m_execute = command.m_execute;
		m_canExecute = command.m_canExecute;
    };

    RelayCommand(std::function<void(const std::any&)> execute);

    // Constructor
    RelayCommand(std::function<void(const std::any&)> execute, std::function<bool(const std::any&)> canExecute);

    // Method to execute the command
    void execute(const std::any& = nullptr);

    // Method to check if the command can be executed
    bool canExecute(const std::any&) const;

protected:
    std::function<void(const std::any&)> m_execute;      // Function to execute the command
    std::function<bool(const std::any&)> m_canExecute;   // Function to check if the command can be executed
};

#endif  // PRES_COMMANDS_RELAYCOMMAND_H_
