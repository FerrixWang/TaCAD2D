// Copyright [2025] TaCAD

#ifndef _Application_H_
#define _Application_H_

#include <QDialog>

#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>

// Project includes
#include "Core/Document.hxx"

class Application : public TDocStd_Application, public EnablePropertyChanged
{
public:
    explicit Application();

    Handle<Document> NewModel(Document::Format theFormat);

    Handle<Document> OpenModel(const std::string& thePath, Document::Format theFormat);

    bool SaveModel();

    bool SaveModelAs();

	DEFINE_STANDARD_RTTI_INLINE(Application, TDocStd_Application)

private:
    void InitDriver(Document::Format format);
};

DEFINE_STANDARD_HANDLE(Application, TDocStd_Application)

#endif // _Application_H_
