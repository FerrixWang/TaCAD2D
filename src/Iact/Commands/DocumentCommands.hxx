// Copyright [2025] TaCAD

#ifndef _DocumentCommands_Header_File_
#define _DocumentCommands_Header_File_

#include <memory>
#include <string>
#include <unordered_map>

#include "Iact/Commands/CommandHelper.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Pres/Commands/ActionCommand.hxx"

class DocumentCommands
{
public:
	static ActionCommand* CreateNewModel()
	{
		static ActionCommand command(
			[](const auto&) { InteractiveContext::Current()->GetApplication()->NewModel(Document::Format::Xml); },
			[](const auto&) { return CommandHelper::ApplicationExists(); }
		);

		return &command;
	}

	static ActionCommand* SaveModel()
	{
		static ActionCommand command(
			[](const auto&) { InteractiveContext::Current()->GetApplication()->SaveModel(); },
			[](const auto&) { return CommandHelper::ApplicationExists(); }
		);
		if(command.text().isEmpty())
		{
			command.setText(QObject::tr("Save Model"));
			command.setIcon(ResourceUtils::icon("App/App-SaveModel"));
			command.setToolTip(QObject::tr("Saves the current model to file."));
			command.connect(&command, &QAction::triggered, []() {command.execute(); });
		}
		return &command;
	}
};

#endif  // _DocumentCommands_Header_File_
