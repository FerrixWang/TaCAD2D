// Copyright [2025] TaCAD

// Own includes
#include "Pres/Commands/RelayCommand.hxx"

RelayCommand::RelayCommand(std::function<void(const std::any&)> execute)
    : m_execute(std::move(execute))
    , m_canExecute(nullptr)
{}

RelayCommand::RelayCommand(std::function<void(const std::any&)> execute, std::function<bool(const std::any&)> canExecute)
    : m_execute(std::move(execute))
    , m_canExecute(std::move(canExecute))
{}

void RelayCommand::execute(const std::any& value)
{
    if (m_execute && canExecute(value)) {
        m_execute(value);
    }
}

bool RelayCommand::canExecute(const std::any& value)const
{
    return m_canExecute ? m_canExecute(value) : true;
}
