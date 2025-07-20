//! class InteractiveEntity

#ifndef _InteractiveEntity_Header_File_
#define _InteractiveEntity_Header_File_

#include <Standard_Transient.hxx>

#include "Core/Topology/Entity.hxx"

class InteractiveEntity : public Entity
{
public:
	InteractiveEntity(const TDF_Label& label)
		: Entity(label)
	{};

	//! Destructor
	virtual ~InteractiveEntity() = default;

};

DEFINE_STANDARD_HANDLE(InteractiveEntity, Entity)

#endif  // _InteractiveEntity_Header_File_
