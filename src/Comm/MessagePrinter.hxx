//! class MessagePrinter

#ifndef MESSAGEPRINTER_HXX
#define MESSAGEPRINTER_HXX

#include <Message_Printer.hxx>
#include <Message_Messenger.hxx>
#include <Message.hxx>
#include <TCollection_AsciiString.hxx>

class MessagePrinter : public Message_Printer
{
public:
    MessagePrinter()
    {
        Handle<Message_Messenger> messenger = Message::DefaultMessenger();
        messenger->AddPrinter(this);
    }

    Signal<void(const std::string&)> MessageSignal;

protected:
    virtual void send(const TCollection_AsciiString& theString, const Message_Gravity) const override
    {
        MessageSignal(theString.ToCString());
    }
};

DEFINE_STANDARD_HANDLE(MessagePrinter, Message_Printer)

#endif // MESSAGEPRINTER_HXX
