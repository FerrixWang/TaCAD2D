// Copyright [2025] TaCAD

// Qt includes
#include <QCoreApplication>
#include <QMessageBox>
#include <QSystemSemaphore>

// Project includes
#include "App/GuiApplication.hxx"
#include "App/MainWindow.hxx"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    GuiApplication app(argc, argv);

    QSystemSemaphore sema(app.applicationName(), 1, QSystemSemaphore::Open);
    if(!sema.acquire())
    {
        QMessageBox::warning(nullptr, QObject::tr("Error"),
                             QObject::tr("An instance of the application is already running."));
        return 1;
    }

    MainWindow mainWindow;
	mainWindow.resize(1280, 800);
    mainWindow.show();

    app.exec();

    sema.release();
    return 0;
}
