// Copyright [2025] TaCAD

#ifndef _CoreContext_Header_File_
#define _CoreContext_Header_File_

#include <string>

#include <QDir>
#include <QSettings>
#include <QStandardPaths>

#include <TDF_Label.hxx>

#include "Comm/ParameterSets.hxx"
#include "Core/Document.hxx"
#include "Core/Viewport.hxx"
#include "Core/Workspace.hxx"

class CoreContext
{
public:
	static CoreContext* Current()
	{
		return s_current;
	}

	Handle<Document> GetDocument() const { return m_document; }
	Handle<Workspace> workspace() const { return m_workspace; }
	Handle<Viewport> viewport() const { return m_viewport; }

	TDF_Label MainLabel() const
	{
		if(!m_document.IsNull())
		{
			return m_document->Main();
		}
		return TDF_Label();
	}

	ParameterSets* Parameters() const 
	{
		return _ParameterSets;
	}

public:
	virtual void SetDocument(const Handle<Document>& document)
	{
		if(!document.IsNull() && m_document != document)
		{
			m_document = document;
			documentChanged(document);

			auto workspaces = document->workspaces();
			
			auto it = std::find(workspaces.begin(), workspaces.end(), m_workspace);
			if(it == workspaces.end())
			{
				SetWorkspace(workspaces.front());
			}
		}
	}

protected:
	virtual void SetWorkspace(const Handle<Workspace>& workspace)
	{
		if (!workspace.IsNull() && m_workspace != workspace) {
			m_workspace = workspace;
			workspaceChanged(workspace);

			auto viewports = workspace->Viewports();
			SetViewport(viewports.front());
		}
 	}

	virtual void SetViewport(const Handle<Viewport>& viewport)
	{
		if(!viewport.IsNull() && m_viewport != viewport)
		{
			m_viewport = viewport;
			viewportChanged(viewport);
		}
	}

	// load local setting with name
	std::shared_ptr<QSettings> GetLocalSettings(const std::string& name);

public:
	Signal<void(const Handle<Document>&)> documentChanged;
	Signal<void(const Handle<Workspace>&)> workspaceChanged;
	Signal<void(const Handle<Viewport>&)> viewportChanged;

protected:
	Handle<Document> m_document;
	Handle<Workspace> m_workspace;
	Handle<Viewport> m_viewport;

protected:
	CoreContext();
	~CoreContext() = default;

	CoreContext(const CoreContext&) = delete;
	CoreContext& operator=(const CoreContext&) = delete;

	static CoreContext* s_current;

private:
	ParameterSets* _ParameterSets;
};

#endif  // _CoreContext_Header_File_

