// Copyright [2025] TaCAD

#ifndef APP_MAINWINDOW_H_
#define APP_MAINWINDOW_H_

// Qt includes
#include <QWidget>
#include <QApplication>

// SARibbonBar includes
#include "SARibbon.h"

// Dock includes
#include "DockManager.h"

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

    static MainWindow* Current()
    {
        auto app = QApplication::instance();
        if(!app)
        {
            return nullptr;
        }
        for(auto widget : QApplication::topLevelWidgets())
        {
            if(auto window = qobject_cast<MainWindow*>(widget))
            {
                return window;
            }
        }
        return nullptr;
    }

    // Get dock manager
	ads::CDockManager* DockManager() const
	{
		return myDockManager;
	}

private:
    void setupUi();
    void setupDockWidgets();
    void setupAppButton();
    void setupCategories();

    QAction* createAction(const QString& text, const QString& iconurl);

    void onMainWindowLoaded();

signals:
	void ApplicationLoaded();

private:
    SARibbonBar* myRibbonBar;
    ads::CDockManager* myDockManager;
};

#endif  // APP_MAINWINDOW_H_
