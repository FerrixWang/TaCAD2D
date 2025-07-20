// Copyright [2025] TaCAD

// Own include
#include "Core/CoreContext.hxx"

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include "Comm/ParameterSets.hxx"
#include "Core/Shapes/Sketch/SketchFactory.hxx"

CoreContext::CoreContext()
	: m_document(nullptr)
	, m_workspace(nullptr)
	, m_viewport(nullptr)
{
    s_current = this;
	_ParameterSets = new ParameterSets("Parameters", qApp);

	SketchFactory::Init();
}

// load local setting with name

std::shared_ptr<QSettings> CoreContext::GetLocalSettings(const std::string& name)
{
	try
	{
		QString configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
		if(configPath.isEmpty())
		{
			qWarning("AppContext::loadLocalSettings - Empty config path");
			return nullptr;
		}

		if(!QDir().mkpath(configPath))
		{
			qWarning("AppContext::loadLocalSettings - Failed to create config directory");
			return nullptr;
		}

		QString settingsFile = configPath + "/" + QString::fromStdString(name) + ".ini";
		return std::make_shared<QSettings>(settingsFile, QSettings::IniFormat);
	}
	catch(const std::exception& ex)
	{
		qWarning("AppContext::loadLocalSettings - Exception: %s", ex.what());
		return nullptr;
	}
}

CoreContext* CoreContext::s_current = nullptr;
