// Copyright [2025] TaCAD

// Own include
#include "App/GuiApplication.hxx"

// stl includes
#include <iostream>

// Qt includes
#include <QLocale>
#include <QStringList>
#include <QTranslator>

// Project includes
#include "App/AppContext.hxx"
#include "App/CommandLine.hxx"
#include "App/WelcomeDialog.hxx"

GuiApplication::GuiApplication(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_welcomeDialog(nullptr)
{
    initTranslation();

    auto cmdLine = std::make_shared<CommandLine>(argc, argv);

    // Show Welcome Dialog if not skipped
    bool bSkipWelcome = cmdLine->isWelcomeDialogDisabled() || cmdLine->hasPathToOpen() || cmdLine->hasScriptToRun();
    if (!bSkipWelcome && false) {
        m_welcomeDialog = new WelcomeDialog; // Create the WelcomeDialog
        m_welcomeDialog->setWindowFlags(m_welcomeDialog->windowFlags() | Qt::WindowStaysOnTopHint);
        m_welcomeDialog->show(); // Show the WelcomeDialog
    }

    // Init context
    AppContext::initialize(cmdLine);
}

GuiApplication::~GuiApplication()
{}

// Initialize synchronization mechanisms
void GuiApplication::initTranslation()
{
    // Set up translator for localization
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString& locale : uiLanguages) {
        const QString baseName = "FaCAD_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            installTranslator(&translator); // Install the translator
            break; // Exit loop after loading the first valid translation
        }
    }
}
