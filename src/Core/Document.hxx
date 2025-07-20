// Copyright [2025] TaCAD

#ifndef CORE_TOPOLOGY_MODEL_H_
#define CORE_TOPOLOGY_MODEL_H_

// stl includes
#include <string>

// Occt includes
#include <TDocStd_Document.hxx>
#include <TDataStd_Name.hxx>

class Workspace;

class Document : public TDocStd_Document, public EnablePropertyChanged
{
public:
	enum class Format { Binary, Xml };

	Document(const std::string& format);
	Document(Format format);

	void Init();

	void InitWorkspaces();

	static const char* FormatToString(Format format)
	{
		switch(format)
		{
		case Format::Binary: return "BinOcaf";
		case Format::Xml: return "XmlOcaf";
		default: return nullptr;
		}
	}

	std::unordered_map<TDF_Label, Handle(Standard_Transient)>& RegisteredTable()
	{
		return myRegistered;
	}

	template<typename T>
	static Handle(T) Get(const TDF_Label& theLabel)
	{

		return nullptr;
	}

	// Get file path
	std::string FilePath() const
	{
		return myFilePath;
	}

public:
	// Get the workspaces
	std::vector<Handle(Workspace)>& workspaces()
	{
		return m_workspaces;
	}

	DEFINE_STANDARD_RTTI_INLINE(Document, TDocStd_Document)
public:

private:
	std::vector<Handle(Workspace)> m_workspaces;

	std::unordered_map<TDF_Label, Handle(Standard_Transient)> myRegistered;

	std::string  myFilePath;
};

DEFINE_STANDARD_HANDLE(Document, TDocStd_Document)

#endif  // CORE_TOPOLOGY_MODEL_H_
