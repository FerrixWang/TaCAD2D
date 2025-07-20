#include "App/Commands/AppCommands.hxx"

#include <string>
#include <unordered_map>

#include "App/MainWindow.hxx"
#include "Iact/Commands/DocumentCommands.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Pres/Commands/RelayCommand.hxx"

RelayCommand* AppCommands::InitApplication()
{
	static RelayCommand command(
		[](const auto&) { DocumentCommands::CreateNewModel()->execute(); },
		[](const auto&) { return !InteractiveContext::Current()->GetApplication().IsNull(); }
	);

	return &command;
}

ActionCommand* AppCommands::ShowShapeInspector()
{
	static ActionCommand command(
		[](const auto&) {
		auto inspector = MainWindow::Current()->DockManager()->findDockWidget("Inspector");
		// switch toggle view
		if(inspector)
		{
			inspector->toggleView(inspector->isClosed());
		}
	},
		[](const auto&) { return !InteractiveContext::Current()->GetApplication().IsNull(); }
	);

	if(command.text().isEmpty())
	{
		command.setText(QObject::tr("Shape Inspector"));
		command.setIcon(ResourceUtils::icon("Tools/Tool-ShapeInspect"));
		command.connect(&command, &QAction::triggered, []() {command.execute(); });
	}
	return &command;
}
