// Copyright [2025] TaCAD

// Own include
#include "Core/Application.hxx"

// occt includes
#include <BinDrivers.hxx>
#include <CDF_Application.hxx>
#include <CDM_Document.hxx>
#include <Standard_Type.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XmlDrivers.hxx>

// Project includes
#include "App/AppContext.hxx"
#include "Core/Document.hxx"

Application::Application() 
{
}

void Application::InitDriver(Document::Format format)
{
	// Initialize the drivers with the format
	if(format == Document::Format::Binary)
	{
		BinDrivers::DefineFormat(this);
	}
	else if(format == Document::Format::Xml)
	{
		XmlDrivers::DefineFormat(this);
	}
}

Handle(Document) Application::NewModel(Document::Format theFormat)
{
	InitDriver(theFormat);

	Handle(Document) aNewModel = new Document(theFormat);
	CDF_Application::Open(aNewModel);

	aNewModel->SetUndoLimit(30);
	aNewModel->Init();

	CoreContext::Current()->SetDocument(aNewModel);

	return aNewModel;
}

Handle(Document) Application::OpenModel(const std::string& thePath, Document::Format theFormat)
{
	InitDriver(theFormat);
	try
	{
		Handle(Document) aDoc = new Document(theFormat);
		if(this->Open(thePath.c_str(), aDoc) == PCDM_RS_OK)
		{
			AppContext::Current()->SetDocument(aDoc);
			return aDoc;
		}
	}
	catch(Standard_Failure& e)
	{
		std::cerr << "Error opening model: " << e.GetMessageString() << std::endl;
		return nullptr;
	}
}

bool Application::SaveModel()
{
	auto doc = AppContext::Current()->GetDocument();
	TDocStd_Application::Save(AppContext::Current()->GetDocument());
	if(doc->FilePath().empty())
	{
		// If the file path is empty, call SaveModelAs
		return SaveModelAs();
	}
	else
	{
		// Save the model to the existing file path
		TDocStd_Application::Save(doc);
		return true;
	}
}

bool Application::SaveModelAs()
{
	// open qt dialog to save file
	auto dlg = new QDialog;
	dlg->show();
	return true;
}
