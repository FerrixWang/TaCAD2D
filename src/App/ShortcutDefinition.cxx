#include "App/ShortcutDefinition.hxx"

#include <QKeySequence>
#include <QShortcut>

#include "App/Commands/AppCommands.hxx"
#include "App/MainWindow.hxx"
#include "Iact/Commands/WorkspaceCommands.hxx"

void ShortcutDefinition::RegisterShortcuts(QObject* object)
{
	//WorkspaceCommands::DoUndo()->setShortcut(QKeySequence::Undo);
	QShortcut* undoShortcut = new QShortcut(QKeySequence::Undo, object, []() {
		WorkspaceCommands::DoUndo()->execute();
	});

	QShortcut* redoShortcut = new QShortcut(QKeySequence::Redo, object, []() {
		WorkspaceCommands::DoRedo()->execute();
	});

	QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), object, []() {
		WorkspaceCommands::Escape()->execute();
	});

}
