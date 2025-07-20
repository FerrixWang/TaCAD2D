#include "Core/Project/WorkingContext.hxx"

#include <TDataXtd_Geometry.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDF_Label.hxx>

#include "Core/GridTypes.hxx"

WorkingContext::WorkingContext(const TDF_Label& thLabel)
	: myLabel(thLabel)
{}

gp_Pln WorkingContext::WorkingPlane() const
{
	auto aAttr = WorkingPlaneAttribute();
	if(aAttr.IsNull())
	{
		InitWorkingPlane();
		TDataXtd_Plane::Set(WorkingPlaneLabel(), gp_Pln(gp::XOY()));
	}
	// Get from _WorkingPlane
	gp_Pln pln;
	TDataXtd_Geometry::Plane(WorkingPlaneLabel(), pln);
	return pln;
}

void WorkingContext::SetWorkingPlane(const gp_Pln& plane)
{
	WorkingPlaneAttribute()->Set(WorkingPlaneLabel(), plane);
}

GridTypes WorkingContext::GridType() const
{
	auto aAttr = GridTypeAttribute();
	if(aAttr.IsNull())
	{
		InitGridType();
	}
	// Get from GetGridType()
	int gridType = GridTypeAttribute()->Get();
	GridTypes myGridType = static_cast<GridTypes>(gridType);
	// Check if the value is valid
	if(myGridType < GridTypes::Rectangular || myGridType > GridTypes::Circular)
	{
		myGridType = GridTypes::Rectangular; // Default to Rectangular if invalid
	}

	// Set the value to GetGridType()
	TDataStd_Integer::Set(GridTypeLabel(), static_cast<int>(myGridType));

	// Return the value
	return myGridType;
}

void WorkingContext::SetGridType(GridTypes type)
{

	// Set the value to GetGridType()
	TDataStd_Integer::Set(GridTypeLabel(), static_cast<int>(type));
}

double WorkingContext::GridStep() const
{
	auto aAttr = GridStepAttribute();
	if(aAttr.IsNull())
	{
		InitGridStep();
	}
	return GridStepAttribute()->Get();
}

void WorkingContext::SetGridStep(double step)
{
	// Set the value to GetGridStep()
	TDataStd_Real::Set(GridStepLabel(), step);
}

double WorkingContext::GridRotation() const
{
	auto aAttr = GridRotationAttribute();
	if(aAttr.IsNull())
	{
		InitGridRotation();
	}
	return GridRotationAttribute()->Get();
}

void WorkingContext::SetGridRotation(double rotation)
{
	// Set the value to GetGridRotation()
	TDataStd_Real::Set(GridRotationLabel(), rotation);
}

int WorkingContext::GridDivisions() const
{
	auto aAttr = GridDivisionsAttribute();
	if(aAttr.IsNull())
	{
		InitGridDivisions();
	}
	return GridDivisionsAttribute()->Get();
}

void WorkingContext::SetGridDivisions(int divisions)
{
	// Set the value to GetGridDivisions()
	TDataStd_Integer::Set(GridDivisionsLabel(), divisions);
}
