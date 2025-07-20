// class SketchConstraintSmoothCorner
//
// Implements a smooth corner constraint, ensuring smooth transition between two segments at a shared point.

#ifndef SketchConstraintSmoothCorner_HXX
#define SketchConstraintSmoothCorner_HXX

#include <map>
#include <memory>
#include <vector>

#include <Standard_Handle.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintFixed.hxx"
#include "Core/Shapes/Sketch/Constraints/SketchConstraintHelper.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentArc.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentBezier.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/Sketch.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Core/Shapes/Sketch/SketchUtils.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchConstraintSmoothCorner : public SketchConstraint
{
public:
    SketchConstraintSmoothCorner(int point, bool symmetric, const TDF_Label& label = DocumentTool::SketchConstraintSmoothCornersLabel().NewChild())
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
        , _Symmetric(symmetric)
    {
        assert(!label.HasAttribute());
        TDataStd_Name::Set(label, "SketchConstraintSmoothCorner");
        TDataStd_Integer::Set(label, symmetric ? 1 : 0); // Store as 1 (true) or 0 (false)
        SetPoints({point});
    }

    SketchConstraintSmoothCorner(const TDF_Label& label)
        : SketchConstraint(label.FindChild(0))
        , _Label(label)
    {
        assert(label.HasAttribute());
    }

    TDF_Label Label() const { return _Label; }

    bool Symmetric() const { return _Symmetric; }
    void SetSymmetric(bool symmetric) { _Symmetric = symmetric; }

    virtual bool MakeConstraint(const std::map<int, gp_Pnt2d>& points,
                                const std::map<int, Handle(SketchSegment)>& segments,
                                const Handle(SketchConstraintSolver)& solver) override
    {
        auto adjacentSegments = SketchUtils::GetSegmentsUsingPoint(segments, Points()[0]);
        if(adjacentSegments.size() != 2)
            return false;

        // Prepare values
        int tan1p1 = -1, tan1p2 = -1;
        Handle(SketchSegmentBezier) bezier1 = Handle(SketchSegmentBezier)::DownCast(segments.at(adjacentSegments[0]));
        if(!bezier1.IsNull())
        {
            if(bezier1->Point(0) == Point(0))
            {
                tan1p1 = bezier1->Point(0);
                tan1p2 = bezier1->Point(1);
            }
            else
            {
                tan1p1 = bezier1->Point(bezier1->Degree());
                tan1p2 = bezier1->Point(bezier1->Degree() - 1);
            }
        }

        int tan2p1 = -1, tan2p2 = -1;
        Handle(SketchSegmentBezier) bezier2 = Handle(SketchSegmentBezier)::DownCast(segments.at(adjacentSegments[1]));
        if(!bezier2.IsNull())
        {
            if(bezier2->Point(0) == Point(0))
            {
                tan2p1 = bezier2->Point(0);
                tan2p2 = bezier2->Point(1);
            }
            else
            {
                tan2p1 = bezier2->Point(bezier2->Degree());
                tan2p2 = bezier2->Point(bezier2->Degree() - 1);
            }
        }

        if(!bezier1.IsNull() && !bezier2.IsNull())
        {
            // *** Bezier-Bezier ***
            Constraint con;
            con.type = ConstraintType::Colinear;
            if(!solver->SetLine(con.line1, tan1p1, tan1p2, true, false) ||
               !solver->SetLine(con.line2, tan2p1, tan2p2, true, false))
            {
                return false;
            }
            solver->AddConstraint(con);

            if(_Symmetric)
            {
                con.type = ConstraintType::EqualLength;
                solver->AddConstraint(con);
            }
            return true;
        }

        auto line = Handle(SketchSegmentLine)::DownCast(segments.at(adjacentSegments[0]));
        if(line.IsNull())
            line = Handle(SketchSegmentLine)::DownCast(segments.at(adjacentSegments[1]));

        if(!line.IsNull() && (!bezier1.IsNull() || !bezier2.IsNull()))
        {
            // *** Bezier-Line ***
            Constraint con;
            con.type = ConstraintType::Colinear;
            if(!solver->SetLine(con.line1, bezier1.IsNull() ? tan2p1 : tan1p1, bezier1.IsNull() ? tan2p2 : tan1p2, true, false) ||
               !solver->SetLine(con.line2, line->StartPoint(), line->EndPoint(), true, true))
            {
                return false;
            }
            solver->AddConstraint(con);
            return true;
        }

        auto arc = Handle(SketchSegmentArc)::DownCast(segments.at(adjacentSegments[0]));
        if(arc.IsNull())
            arc = Handle(SketchSegmentArc)::DownCast(segments.at(adjacentSegments[1]));

        if(!arc.IsNull() && (!bezier1.IsNull() || !bezier2.IsNull()))
        {
            // *** Bezier-Arc ***
            std::vector<Constraint> constraints;
            Constraint con;
            con.type = ConstraintType::TangentToCircle;
            if(!solver->SetLine(con.line1, bezier1.IsNull() ? tan2p1 : tan1p1, bezier1.IsNull() ? tan2p2 : tan1p2, true, false) ||
               !solver->SetCircle(con.circle1, arc, constraints, points, true, true))
            {
                return false;
            }
            for(const auto& c : constraints)
                solver->AddConstraint(c);
            solver->AddConstraint(con);
            return true;
        }

        return false;
    }

    virtual Handle(SketchConstraint) Clone() const override
    {
        return new SketchConstraintSmoothCorner(Points()[0], _Symmetric);
    }

    static bool CanCreate(const Handle(Sketch)& sketch, const std::vector<int>& points, const std::vector<int>& segments)
    {
        if(points.empty() || segments.size() != 0 ||
           SketchConstraintHelper::AnySegmentWithConstrainedPoint<SketchConstraintFixed>(sketch, points) ||
           SketchConstraintHelper::AnyConstrainedPoint<SketchConstraintSmoothCorner>(sketch, points))
        {
            return false;
        }

		auto segmentsMap = sketch->Segments();
        for(int pointIndex : points)
        {
            auto adjacentSegments = SketchUtils::GetSegmentsUsingPoint(segmentsMap, pointIndex);
            if(adjacentSegments.size() != 2)
                continue;

            auto segment1 = segmentsMap.at(adjacentSegments[0]);
            auto segment2 = segmentsMap.at(adjacentSegments[1]);
            if(Handle(SketchSegmentBezier)::DownCast(segment1))
            {
                return Handle(SketchSegmentBezier)::DownCast(segment2) ||
                    Handle(SketchSegmentLine)::DownCast(segment2) ||
                    Handle(SketchSegmentArc)::DownCast(segment2);
            }
            if(Handle(SketchSegmentBezier)::DownCast(segment2))
            {
                return Handle(SketchSegmentBezier)::DownCast(segment1) ||
                    Handle(SketchSegmentLine)::DownCast(segment1) ||
                    Handle(SketchSegmentArc)::DownCast(segment1);
            }
        }
        return false;
    }

    static std::vector<Handle(SketchConstraint)> Create(const Handle(Sketch)& sketch, const std::vector<int>& points, const std::vector<int>& segments)
    {
        std::vector<Handle(SketchConstraint)> list;

        if(!CanCreate(sketch, points, segments))
            return list;

        for(int pointIndex : points)
        {
            auto adjacentSegments = SketchUtils::GetSegmentsUsingPoint(sketch->Segments(), pointIndex);
            if(adjacentSegments.size() != 2)
                continue;

            list.push_back(new SketchConstraintSmoothCorner(pointIndex, true));
        }
        return list;
    }

private:
    TDF_Label _Label;
    bool _Symmetric;
};

DEFINE_STANDARD_HANDLE(SketchConstraintSmoothCorner, SketchConstraint)

#endif // SketchConstraintSmoothCorner_HXX
