//! class MessagePrinter

#ifndef _MessagePrinter_Header_File_
#define _MessagePrinter_Header_File_

#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_Printer.hxx>
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

#endif // _MessagePrinter_Header_File_
