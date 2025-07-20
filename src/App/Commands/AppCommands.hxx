// Copyright [2025] TaCAD

#ifndef _AppCommands_Header_File_
#define _AppCommands_Header_File_

#include "Pres/Commands/ActionCommand.hxx"
#include "Pres/Commands/RelayCommand.hxx"

class AppCommands
{
public:
	static RelayCommand* InitApplication();

	static ActionCommand* ShowShapeInspector();

	static ActionCommand* TestCommand()
	{
		static ActionCommand command(
			[](const auto&) { QMessageBox::information(nullptr, "TestCommand", "");
		},
			[](const auto&) { return true; }
		);

		if(command.text().isEmpty())
		{
			command.setText(QObject::tr("Test Command"));
			command.connect(&command, &QAction::triggered, []() {command.execute(); });
		}
		return &command;
	}
};

#endif  // _AppCommands_Header_File_
