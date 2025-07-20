// Copyright [2025] TaCAD

#ifndef _GuiApplication_Header_File_
#define _GuiApplication_Header_File_

// Qt includes
#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QTranslator>

// Project includes
#include "App/WelcomeDialog.hxx"

class GuiApplication : public QApplication
{
    Q_OBJECT
public:
    GuiApplication(int& argc, char** argv);
    ~GuiApplication();

public:

private:
    void initTranslation();

private:
    WelcomeDialog* m_welcomeDialog;
};

#endif  // _GuiApplication_Header_File_
