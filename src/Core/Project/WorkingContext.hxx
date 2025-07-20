#ifndef WORKING_CONTEXT_HXX
#define WORKING_CONTEXT_HXX

#include <gp.hxx>
#include <gp_Pln.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_GenericEmpty.hxx>

#include "Core/GridTypes.hxx"

class WorkingContextAttribute;

class WorkingContext : public Standard_Transient
{
public:
    DEFINE_ATTRIBUTE_ON_CHILD(1, TDataXtd_Plane, WorkingPlane)
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Real, GridStep)
    DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Integer, GridType)
    DEFINE_ATTRIBUTE_ON_CHILD(4, TDataStd_Real, GridRotation)
    DEFINE_ATTRIBUTE_ON_CHILD(5, TDataStd_Integer, GridDivisions)

	// set WorkingContextAttribute for type  using
	using Attribute = WorkingContextAttribute;

    WorkingContext(const TDF_Label& thLabel = {});

    gp_Pln WorkingPlane() const;
    void SetWorkingPlane(const gp_Pln& plane);

    GridTypes GridType() const;
    void SetGridType(GridTypes type);

    double GridStep() const;
    void SetGridStep(double step);

    double GridRotation() const;
    void SetGridRotation(double rotation);

    int GridDivisions() const;
    void SetGridDivisions(int divisions);

    friend WorkingContextAttribute;

	TDF_Label Label() const
	{
		return myLabel;
	}

    void SetLabel(const TDF_Label& label)
    {
        myLabel = label;
    }

    Handle(WorkingContext) Clone() const
    {
        Handle(WorkingContext) wc = new WorkingContext();
        wc->CopyFrom(this);
        return wc;
    }

    void CopyFrom(const Handle(WorkingContext)& other)
    {
		auto pln = other->WorkingPlane();     
        SetWorkingPlane(pln);
		auto step = other->GridStep();
        step = step < 0.0 ? 8.0 : step;
		SetGridStep(step);
		auto type = other->GridType();
		type = type < GridTypes::Rectangular ? GridTypes::Rectangular : type;
		SetGridType(type);
		auto rotation = other->GridRotation();
		rotation = rotation < 0.0 ? 0.0 : rotation;
		SetGridRotation(rotation);
		auto divisions = other->GridDivisions();
		divisions = divisions < 0 ? 9 : divisions;
		SetGridDivisions(divisions);
    }

private:
	TDF_Label myLabel;
};

DEFINE_STANDARD_HANDLE(WorkingContext, Standard_Transient)

#endif // WORKING_CONTEXT_HXX
