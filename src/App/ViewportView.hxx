// Copyright [2025] TaCAD

#ifndef APP_VIEWPORTVIEW_H_
#define APP_VIEWPORTVIEW_H_

// Qt includes
#include <QLabel>
#include <QScrollArea>

#include "Iact/Viewport/ViewportPanel.hxx"

/// @brief The ViewportView class
/// The ViewportView class is a QWidget that contains the ViewportPanel and a message bar.
class ViewportView : public QScrollArea
{
    Q_OBJECT

public:
    explicit ViewportView(QWidget* parent = nullptr);

    // Property for updateMessage
    const QString& updateMessage() const;

    void setUpdateMessage(const QString& value);

    void setupConnection();

    void onWorkspaceControllerChanged(const Handle<WorkspaceController>& WCer);

private:
    // Update Info handling
    void versionCheck_UpdateAvailable(const QString& updateUrl, const QString& updateVersion);

    // Executes when update is available
    void updateExecute();

    // Dismiss update message
    void dismissUpdateExecute();

signals:
    void updateAvailable();
    void propertyChanged(const QString& propertyName);

private:
    QLabel* m_messageBar;
    ViewportPanel* m_viewportPanel;

    QString m_updateMessage;
}; // class ViewportView

#endif  // APP_VIEWPORTVIEW_H_
