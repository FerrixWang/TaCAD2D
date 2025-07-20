// Copyright [2025] TaCAD

// Own include
#include "Core/Document.hxx"

// Occt includes
#include <Standard_Type.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDocStd_Document.hxx>

#include "Core/Workspace.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

Document::Document(const std::string& format)
	: TDocStd_Document(format.c_str())
{
	// Initialize the document

	DocumentTool::Set(Main());
	m_workspaces.push_back(new Workspace(this));
}

Document::Document(Format format)
	: TDocStd_Document(FormatToString(format))
{
}

void Document::Init()
{
	DocumentTool::Set(Main());

	InitWorkspaces();
}

void Document::InitWorkspaces()
{
	TDF_Label aLabel = DocumentTool::WorkspacesLabel(Main());
	if(!aLabel.HasChild())
	{
		aLabel.NewChild();
	}
	for(TDF_ChildIterator it(aLabel); it.More(); it.Next())
	{
		auto aWorkspace = new Workspace(this, it.Value());
		m_workspaces.push_back(aWorkspace);
	}
}
