// Copyright [2025] TaCAD

#ifndef _EnablePropertyChanged_Header_File_
#define _EnablePropertyChanged_Header_File_

#include <any>
#include <string>

#include <boost/signals2.hpp>

class EnablePropertyChanged
{
public:
	EnablePropertyChanged() = default;

	// connect a slot to the propertyChanged signal
	template<typename Slot>
	void connectPropertyChanged(Slot&& slot)
	{
		propertyChanged.connect(std::forward<Slot>(slot));
	}

protected:
	virtual void raisePropertyChanged(const std::string& propertyName, const std::any& sender = nullptr)
	{
		if(!m_suppressPropertyChanged)
		{
			propertyChanged(propertyName, sender);
		}
	}

	virtual void suppressPropertyChanged(bool suppress)
	{
		m_suppressPropertyChanged = suppress;
	}

private:
	bool m_suppressPropertyChanged = false;

	Signal<void(const std::string&, const std::any)> propertyChanged;
};

#endif  // _EnablePropertyChanged_Header_File_
