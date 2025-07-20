// Copyright [2025] TaCAD

#ifndef APP_APPCONTEXT_H_
#define APP_APPCONTEXT_H_

// Project includes
#include "App/CommandLine.hxx"
#include "App/ShortcutDefinition.hxx"
#include "Core/Document.hxx"
#include "Iact/InteractiveContext.hxx"

class MainWindow;

class AppContext : public InteractiveContext
{
public:
    static AppContext* Current()
    {
        return static_cast<AppContext*>(InteractiveContext::Current());
    }

    static AppContext* initialize(const std::shared_ptr<CommandLine>& cmdLine)
    {
		if(!cmdLine)
		{
			throw std::runtime_error("Command line is null");
		}

		return new AppContext();
    }

	MainWindow* GetMainWindow() const
	{
		return m_mainWindow;
	}

	void SetMainWidow(MainWindow* mainWindow)
	{
		m_mainWindow = mainWindow;
	}

private:
	AppContext() : InteractiveContext() {}
	~AppContext() = default;

private:
	std::shared_ptr<CommandLine> m_cmdLine;

	MainWindow* m_mainWindow = nullptr;
};

#endif  // APP_APPCONTEXT_H_
