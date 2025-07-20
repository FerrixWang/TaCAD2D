// SketchConstraintPointOnSegment.hxx
// Created on: 2025-05-15
// Created by: xAI User
//
// Implements a geometric constraint that enforces a point to lie on a segment (line or circle)
// in a 2D sketch. This class ensures a specified point remains on a given line or circle segment
// during constraint solving. It integrates with the sketch constraint solver to manage relationships
// and supports validation and cloning.

#ifndef SketchConstraintPointOnSegment_HXX
#define SketchConstraintPointOnSegment_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintPointOnMidpoint.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentCircle.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"

//! \class SketchConstraintPointOnSegment
//! \brief Implements a geometric constraint that enforces a point to lie on a segment (line or circle) in a 2D sketch.
//! This class ensures a specified point remains on a given line or circle segment during constraint solving.
//! It integrates with the sketch constraint solver to manage relationships and supports validation and cloning.
class SketchConstraintPointOnSegment : public SketchConstraint
{
public:
    //! \brief Constructor for creating a new point-on-segment constraint.
    //! \param[in] point  Index of the point to be constrained.
    //! \param[in] segment  Index of the segment (line or circle) the point should lie on.
    //! \param[in] label  TDF_Label for storing the constraint in the document (defaults to a new child label).
    SketchConstraintPointOnSegment(int point, int segment,
                                   const TDF_Label& label = DocumentTool::SketchConstraintPointOnSegmentsLabel().NewChild())
        : SketchConstraint(label.FindChild(0)) //!< Initialize base class with first child label
        , _Label(label)                        //!< Store the provided label
    {
        assert(!label.HasAttribute());                             //!< Ensure the label is clean
        TDataStd_Name::Set(label, "SketchConstraintPointOnSegment"); //!< Set name attribute
        SetSegments({segment});                                    //!< Initialize segment index
        SetPoints({point});                                        //!< Initialize point index
    }

    //! \brief Constructor for loading an existing constraint from a TDF_Label.
    //! \param[in] label  TDF_Label containing the constraint data.
    SketchConstraintPointOnSegment(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0)) //!< Initialize base class with first child label
        , _Label(label)                        //!< Store the provided label
    {
        assert(label.HasAttribute()); //!< Ensure the label has attributes
    }

    //! \brief Return the TDF_Label associated with this constraint.
    //! \return The TDF_Label storing the constraint data.
    TDF_Label Label() const { return _Label; }

    //! \brief Apply the constraint to the solver.
    //! \param[in] points  Map of point indices to their 2D coordinates.
    //! \param[in] segments  Map of segment indices to their SketchSegment handles.
    //! \param[in] solver  Handle to the constraint solver.
    //! \return True if the constraint was successfully applied, false otherwise.
    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        // Check if the segment exists using std::map::find
        auto it = segments.find(Segment(0));
        if(it == segments.end())
        {
            return false; //!< Segment not found
        }

        bool valid = false;
        Handle(SketchSegment) segment = it->second; //!< Get segment handle

        // Handle line segment
        if(Handle(SketchSegmentLine) line = Handle(SketchSegmentLine)::DownCast(segment))
        {
            Constraint con;
            con.type = ConstraintType::PointOnLine; //!< Set constraint type
            valid = solver->SetLine(con.line1, line->PointsAttribute()->Value(0),
                                    line->PointsAttribute()->Value(1), true, true); //!< Configure line
            valid &= solver->SetPoint(con.point1, Point(0), false);                  //!< Set point
            if(valid)
            {
                solver->AddConstraint(con); //!< Add constraint to solver
            }
        }
        // Handle circle segment
        else if(Handle(SketchSegmentCircle) circle = Handle(SketchSegmentCircle)::DownCast(segment))
        {
            std::vector<Constraint> constraints;
            Constraint con;
            con.type = ConstraintType::PointOnCircle;                            //!< Set constraint type
            valid = solver->SetCircle(con.circle1, circle, constraints, points, true, false); //!< Configure circle
            valid &= solver->SetPoint(con.point1, Point(0), false);              //!< Set point
            constraints.push_back(con);
            if(valid)
            {
                for(const auto& c : constraints)
                {
                    solver->AddConstraint(c); //!< Add each constraint to solver
                }
            }
        }

        return valid; //!< Return success status
    }

    //! \brief Create a clone of this constraint.
    //! \return A new instance of SketchConstraintPointOnSegment with the same point and segment indices.
    virtual Handle(SketchConstraint) Clone() const override
    {
        return new SketchConstraintPointOnSegment(Point(0), Segment(0));
    }

    //! \brief Check if a point-on-segment constraint can be created.
    //! \param[in] sketch  Handle to the sketch containing points and segments.
    //! \param[in] points  Vector of point indices.
    //! \param[in] segments  Vector of segment indices.
    //! \return True if the constraint is valid and can be created, false otherwise.
    static bool CanCreate(const Handle(Sketch)& sketch,
                          const std::vector<int>& points,
                          const std::vector<int>& segments)
    {
        return points.size() > 0                                                         //!< At least one point
            && segments.size() == 1                                                   //!< Exactly one segment
            && (SketchConstraintHelper::AllSegmentsOfType<SketchSegmentLine>(sketch, segments) ||
                SketchConstraintHelper::AllSegmentsOfType<SketchSegmentCircle>(sketch, segments)) //!< Line or circle
            && !SketchConstraintHelper::AnySegmentAndPointConstraint<SketchConstraintPointOnSegment>(
            sketch, points, segments) //!< No existing point-on-segment
            && !SketchConstraintHelper::AnySegmentAndPointConstraint<SketchConstraintPointOnMidpoint>(
            sketch, points, segments) //!< No midpoint constraint
            && !SketchConstraintHelper::AnyConstrainedPoint<SketchConstraintFixed>(
            sketch, points); //!< No fixed point
    }

    //! \brief Create point-on-segment constraints for the given points and segment.
    //! \param[in] sketch  Handle to the sketch containing points and segments.
    //! \param[in] points  Vector of point indices.
    //! \param[in] segments  Vector of segment indices (must contain exactly one segment).
    //! \return A vector of created constraint handles.
    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch,
                                                        const std::vector<int>& points,
                                                        const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;

        // Find segment in sketch
        auto sketchSegments = sketch->Segments();
        auto it = sketchSegments.find(segments[0]);
        if(it == sketchSegments.end()) return list; //!< Segment not found

        Handle(SketchSegment) segment = it->second; //!< Get segment handle

        // Create constraints for valid points
        for(const auto& pointIndex : points)
        {
            bool contains = false;
            for(const auto& segPoint : *segment->PointsAttribute()->Array())
            {
                if(segPoint == pointIndex)
                {
                    contains = true;
                    break;
                }
            }
            if(!contains)
            {
                list.push_back(new SketchConstraintPointOnSegment(pointIndex, segments[0])); //!< Create constraint
            }
        }

        return list; //!< Return created constraints
    }

private:
    TDF_Label _Label; //!< TDF_Label for storing the constraint in the document
};

DEFINE_STANDARD_HANDLE(SketchConstraintPointOnSegment, SketchConstraint)

#endif // SketchConstraintPointOnSegment_HXX