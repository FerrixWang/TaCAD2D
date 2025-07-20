// Copyright [2025] TaCAD

// Own includes
#include "App/ViewportView.hxx"

// Qt includes
#include <QFont>
#include <QLabel>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "Iact/HudElements/HudManager.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/WorkspaceController.hxx"

/// @brief Constructor
/// @param parent, the parent widget
/// @note The constructor initializes the ViewportView with a ViewportPanel and a message bar.
/// The message bar is used to display tool and error messages.
/// The ViewportPanel is a QWidget that contains the 3D view of the model.
ViewportView::ViewportView(QWidget* parent)
    : QScrollArea(parent)
    , m_messageBar(nullptr)
{
    // Set layout for the main panel
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setFamily("Arial");
    font.setPointSize(10);
    font.setBold(true);

    // Tool and error message area
    m_messageBar = new QLabel("Tool and error message area", this);
    m_messageBar->setContentsMargins(20, 2, 20, 2);
    m_messageBar->setFont(font);
    m_messageBar->setStyleSheet("background-color: lightyellow;");
    mainLayout->addWidget(m_messageBar);

    // add spacer between the labels
    mainLayout->addStretch(1);

    // Grid information display
    QLabel* gridInfo = new QLabel("Grid information display area");
    gridInfo->setContentsMargins(m_messageBar->contentsMargins());
    gridInfo->setFont(m_messageBar->font());
    gridInfo->setStyleSheet(m_messageBar->styleSheet());
    mainLayout->addWidget(gridInfo);

    m_viewportPanel = new ViewportPanel(this);
    setWidget(m_viewportPanel);
    setWidgetResizable(true);

	setupConnection();
}

// Property for updateMessage

const QString& ViewportView::updateMessage() const
{
    return m_updateMessage;
}

void ViewportView::setUpdateMessage(const QString& value)
{
    if(m_updateMessage != value)
    {
        m_updateMessage = value;
        emit propertyChanged("UpdateMessage");
    }
}

void ViewportView::setupConnection()
{
    InteractiveContext::Current()->workspaceControllerChanged.connect(
        std::bind(&ViewportView::onWorkspaceControllerChanged, this, std::placeholders::_1));
}

void ViewportView::onWorkspaceControllerChanged(const Handle(WorkspaceController)& WCer)
{
    if(m_messageBar != nullptr)
    {
        auto aHudManager = WCer->GetHudManager();
        aHudManager->SetHintMessager(m_messageBar);
    }
}

// Update Info handling

void ViewportView::versionCheck_UpdateAvailable(const QString& updateUrl, const QString& updateVersion)
{
    m_updateMessage = "A new version is available for download: " + updateVersion;
    emit propertyChanged("UpdateMessage");

    emit updateAvailable();
}

// Executes when update is available

void ViewportView::updateExecute()
{
    dismissUpdateExecute();

    if(!m_updateMessage.isEmpty())
    {
        // Open the update URL (simulated here with a print statement)
        std::cout << "Navigating to: " << "https://example.com/update" << std::endl;
    }
}

// Dismiss update message

void ViewportView::dismissUpdateExecute()
{
    setUpdateMessage("");
}
