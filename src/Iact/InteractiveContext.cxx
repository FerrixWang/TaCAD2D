// Copyright [2025] TaCAD

// Own include
#include "Iact/InteractiveContext.hxx"

#include <QStandardPaths>

#include "Iact/Workspace/EditorState.hxx"

InteractiveContext::InteractiveContext()
	: CoreContext()
	, m_application(new Application())
	, m_workspaceController(nullptr)
	, m_viewportController(nullptr)
	, m_editorState(new EditorState())
{
    LoadEditorState();
}

void InteractiveContext::LoadEditorState()
{
    // load local settings
    auto settings = GetLocalSettings("EditorState");
	m_editorState->LoadFromQSettings(settings);
}

void InteractiveContext::SaveEditorState()
{
	auto settings = GetLocalSettings("EditorState");
	if(!settings)
	{
		settings = std::make_shared<QSettings>(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/EditorState.ini", QSettings::IniFormat);
	}
	m_editorState->SaveToQSettings(settings);
}

