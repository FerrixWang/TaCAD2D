
#include "Core/Shapes/Sketch/Sketch.hxx"

#include <Message.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_AISPresentation.hxx>

#include "Core/Shapes/Sketch/Constraints/SketchConstraintParallel.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementPoint.hxx"
#include "Core/Shapes/Sketch/SketchConstraintSolver.hxx"

int Sketch::AddPoint(const gp_Pnt2d& point, int nextIndex)
{
    if(nextIndex == -1)
    {
        nextIndex = PointsLabel().NbChildren();  // tag form 0 to NbChildren - 1;
    }

    Handle(SketchElementPoint) element;
    auto newLabel = PointsLabel().FindChild(nextIndex, false);
	if(!newLabel.IsNull())
	{
		element = SketchFactory::Wrap<SketchElementPoint>(newLabel);    
	}
    else
    {
        newLabel = PointsLabel().FindChild(nextIndex, true);
        element = new SketchElementPoint(nextIndex, point, newLabel);
    }
    element->OnPointsChanged({{nextIndex, point}}, {});
    return nextIndex;
}

int Sketch::AddSegment(const Handle(SketchSegment)& segment, int index)
{
    if(index == -1)
    {
        index = SegmentsLabel().NbChildren();
    }

    TDF_Label segmentLabel = segment->InstanceLabel();

    auto newSegmentLabel = SegmentsLabel().FindChild(index);
    DocumentTool::CopyLabel(segmentLabel, newSegmentLabel);
    auto newSegment = SketchFactory::Wrap<SketchSegment>(newSegmentLabel);
    auto elementLabel = newSegment->ElementLabel();

    Handle(SketchElementSegment) element;
    if(elementLabel.HasAttribute())
    {
        element = SketchFactory::Wrap<SketchElementSegment>(elementLabel);
    }
    else
    {
        element = MakeHandle<SketchElementSegment>(newSegment, elementLabel);
    }
    element->OnPointsChanged(pointsMap, {});

    return index;
}

int Sketch::AddConstraint(const Handle(SketchConstraint)& constraint, int index)
{
    // Ìí¼ÓÏß¶Î
    if(index == -1)
    {
        index = SegmentsLabel().NbChildren();  // tag form 0 to NbChildren - 1;
    }

    TDF_Label segmentLabel = constraint->InstanceLabel();
    if(segmentLabel.IsNull())
    {
        return -1;
    }

    auto newSegmentLabel = ConstraintsLabel().FindChild(index);

    DocumentTool::CopyLabel(segmentLabel, newSegmentLabel);

    return index;
}

bool Sketch::SolveConstraints(bool canFail, const std::vector<int>& fixedPoints)
{
    if(ConstraintsLabel().NbChildren() == 0)
    {
        _ConstraintSolverFailed = false;
        return true;
    }

    bool success = SketchConstraintSolver::Solve(this, fixedPoints);
    if(!success)
    {
        // If constraints can not be solved with fixed points, try again without.
        // Solving the constraints has a higher prio than leave the points untouched.
        success = SketchConstraintSolver::Solve(this);
    }

    if(success)
    {
        Invalidate();
        Update();
        PointsChanged(PointsLabel());
        if(canFail)
        {
            _ConstraintSolverFailed = false;
        }
        return true;
    }
    if(canFail)
    {
        Message::SendFail("Sketch constraints failed to solve.");
        _ConstraintSolverFailed = true;
    }
    return false;
}

void Sketch::Update()
{
	auto points = Points();
    TDF_ChildIterator itp(PointsLabel());
    for(; itp.More(); itp.Next())
    {
        auto element = SketchFactory::Wrap<SketchElementPoint>(itp.Value());
        element->OnPointsChanged(points, {});
    }

    TDF_ChildIterator it(SegmentsLabel());
    for(; it.More(); it.Next())
    {
        auto _Segment = SketchFactory::Wrap<SketchSegment>(it.Value());
        auto element = SketchFactory::Wrap<SketchElementSegment>(_Segment->ElementLabel());
        element->OnPointsChanged(points, {});
    }
}
