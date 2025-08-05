//! class Body

#ifndef _Body_Header_File_
#define _Body_Header_File_

#include <Standard_Transient.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataStd_RealArray.hxx>

#include "Core/Topology/InteractiveEntity.hxx"

class Body : public InteractiveEntity
{
public:
	DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Name, InteractiveEntity)	
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataXtd_Point, Position)
	DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_RealArray, Rotation)

public:
	Body(const TDF_Label& label)
		: InteractiveEntity(label.FindChild(1))
	{};

	//! Destructor
	virtual ~Body() = default;

	TDF_Label Label() const
	{
		return myLabel;
    }

private:
	TDF_Label myLabel; //!< Label of the Body
};

DEFINE_STANDARD_HANDLE(Body, Standard_Transient)

#endif // !_Body_Header_File_
