#ifndef MESSAGES_PANEL_HXX
#define MESSAGES_PANEL_HXX

#include <QPlainTextEdit>
#include <QScrollBar>

#include "Comm/MessagePrinter.hxx"

class MessagesPanel : public QPlainTextEdit
{
    Q_OBJECT
public:
    MessagesPanel(QWidget* parent = nullptr) : QPlainTextEdit(parent)
    {
        setReadOnly(true);
        
        myPrinter = new MessagePrinter();
        myConnection = myPrinter->MessageSignal.connect(
            [this](const std::string& msg) {
            QMetaObject::invokeMethod(this, "onMessageReceived", Qt::QueuedConnection,
                                      Q_ARG(std::string, msg));
        });
    }

    ~MessagesPanel()
    {
        myConnection.disconnect();
    }

private slots:
    void onMessageReceived(const std::string& msg)
    {
        appendPlainText(QString::fromStdString(msg));
        QScrollBar* bar = verticalScrollBar();
        bar->setValue(bar->maximum());
    }

private:
    Handle(MessagePrinter) myPrinter;
    boost::signals2::connection myConnection;
};

#endif // MESSAGES_PANEL_HXX