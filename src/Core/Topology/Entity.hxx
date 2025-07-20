//! class Entity

#ifndef _Entity_Header_File_
#define _Entity_Header_File_

#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>

class Entity : public Standard_Transient
{
public:
	Entity(const TDF_Label& label) 
		: myLabel(label)
	{};

	//! Destructor
	virtual ~Entity() = default;

	TDF_Label Label() const
	{
		return myLabel;
	}

private:
	TDF_Label myLabel; //!< Label of the entity
};

DEFINE_STANDARD_HANDLE(Entity, Standard_Transient)

#endif // !_Entity_Header_File_
