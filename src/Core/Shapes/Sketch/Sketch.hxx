//! class Sketch

#ifndef Sketch_Header_File_
#define Sketch_Header_File_

#include <map>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Reference.hxx>
#include <AIS_Shape.hxx>

#include "Core/Shapes/Shape2D.hxx"
#include "Core/Shapes/Sketch/Elements/SketchElementSegment.hxx"
#include "Core/Shapes/Sketch/Segments/SketchSegmentLine.hxx"
#include "Core/Shapes/Sketch/SketchConstraint.hxx"
#include "Core/Shapes/Sketch/SketchFactory.hxx"
#include "Core/Shapes/Sketch/SketchSegment.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

class SketchConstraint;

class Sketch : public Shape2D
{
	DEFINE_ATTRIBUTE_ON_LABEL(TDataStd_Name, Name)
    DEFINE_ATTRIBUTE_ON_CHILD(0, TDataStd_Name, Shape2D)
    DEFINE_ATTRIBUTE_ON_CHILD(1, TDataStd_Name, Points)
    DEFINE_ATTRIBUTE_ON_CHILD(2, TDataStd_Name, Segments)
    DEFINE_ATTRIBUTE_ON_CHILD(3, TDataStd_Name, Constraints)
public:
    //! Constructor
    Sketch(const TDF_Label& label = DocumentTool::SketchsLabel().NewChild())
        : Shape2D(label.FindChild(0))
        , _Label(label)
    {
        if(NameAttribute().IsNull())
            NameAttribute()->Set(_Label, "Sketch");

		if(PointsAttribute().IsNull())
            PointsAttribute()->Set(PointsLabel(), "Points");

		if(SegmentsAttribute().IsNull())
			SegmentsAttribute()->Set(SegmentsLabel(), "Segments");

		if(ConstraintsAttribute().IsNull())
			ConstraintsAttribute()->Set(ConstraintsLabel(), "Constraints");
    };

    //! Destructor
    virtual ~Sketch() = default;

    TDF_Label Label() const { return _Label; }

    gp_Pln Plane() const { return {}; }

    int AddPoint(const gp_Pnt2d& point, int nextIndex = -1);

    int AddSegment(const Handle<SketchSegment>& segment, int index = -1);

    int AddConstraint(const Handle<SketchConstraint>& constraint, int index = -1);

	void DeleteSegment(const Handle<SketchSegment>& segment)
	{
		// 删除线段
		auto segmentsLabel = segment->InstanceLabel();

		if(!segmentsLabel.IsNull())
		{
            segmentsLabel.ForgetAllAttributes(true);
		}
	}

    const std::map<int, gp_Pnt2d>& Points()
    {
        pointsMap.clear();

        // 检查标签有效性
        auto pointsLabel = PointsLabel();

        if(pointsLabel.IsNull())
        {
            return pointsMap;
        }

        TDF_ChildIterator it(pointsLabel);

        for(; it.More(); it.Next())
        {
            TDF_Label child = it.Value();

            gp_Pnt point;

            if(!child.IsNull())
            {
                TDataXtd_Geometry::Point(child, point);
            }

            // 获取 gp_Pnt 并转换为 gp_Pnt2d
            pointsMap[child.Tag()] = gp_Pnt2d(point.X(), point.Y());
        }
        return pointsMap;
    }

    const std::map<int, Handle<SketchSegment>>& Segments()
    {
        segmentsMap.clear();

        // 检查标签有效性
        if(SegmentsLabel().IsNull())
        {
            return segmentsMap;
        }

        TDF_ChildIterator it(SegmentsLabel());

        for(; it.More(); it.Next())
        {
            TDF_Label segmentLabel = it.Value();

            auto o = SketchFactory::Wrap<SketchSegment>(segmentLabel);

			segmentsMap.emplace(segmentLabel.Tag(), o);
        }

        return segmentsMap;
    }

    const std::vector<Handle<SketchConstraint>>& Constraints()
    {
        constraintsArray.clear();

        // 检查标签有效性
        if(ConstraintsLabel().IsNull())
        {
            return constraintsArray;
        }

        TDF_ChildIterator it(ConstraintsLabel());

        for(; it.More(); it.Next())
        {
            TDF_Label constraintLabel = it.Value();

            auto o = SketchFactory::Wrap<SketchConstraint>(constraintLabel);

            if (!o.IsNull()) 
                constraintsArray.push_back(o);
        }

        return constraintsArray;
    }

    gp_Pnt2d Point(int idx) const
    {
        auto child = PointsLabel().FindChild(idx, false);

        if(child.IsNull())
        {
            return {};
        }      
        gp_Pnt point;
        TDataXtd_Geometry::Point(child, point);

        return {point.X(), point.Y()};
    }

    // void SetPoints()

    void AddPoints(const std::map<int, gp_Pnt2d>& points)
    {
        for(const auto& [key, point] : points)
        {
            AddPoint(point, key);
        }
    }

    auto AddElements(const std::map<int, gp_Pnt2d>& points,
                     const std::vector<int>& mergePointIndices,
                     const std::map<int, Handle<SketchSegment>>& segments, 
					 const std::vector<Handle<SketchConstraint>> constraints = {})
    {
        std::map<int, int> pointMap;
        std::map<int, int> segmentMap;

        int nextIndex = PointsLabel().NbChildren();  // tag form 0 to NbChildren - 1;

        // 添加点
        for(const auto& [key, point] : points)
        {
            int newPointIndex = -1;
            if(!mergePointIndices.empty() && mergePointIndices[key] >= 0)
            {
                newPointIndex = mergePointIndices[key];
            }
            else
            {
                newPointIndex = nextIndex++;
                pointsMap[newPointIndex] = point;
                AddPoint(point, newPointIndex);
            }
            pointMap[key] = newPointIndex;
        }

        // 添加线段
        nextIndex = SegmentsLabel().NbChildren();  // tag form 0 to NbChildren - 1;
        for(const auto& [key, segment] : segments)
        {
            for(int i = 0; i < segment->PointsAttribute()->Length(); i++)
            {
                segment->PointsAttribute()->SetValue(i, pointMap[segment->PointsAttribute()->Value(i)]);
            }

            AddSegment(segment, nextIndex);

			segmentMap[key] = nextIndex;
            nextIndex++;
        }

        std::vector<int> newConstraints;

        if(!constraints.empty()) 
        {
            int nextIndex = ConstraintsCount();

			for(const auto& constraint : constraints)
			{
                if((!pointMap.empty()) && (constraint->PointsCount() > 0))
                {
                    for(const auto& a : constraint->Points())
                    {
                        constraint->SetPoint(a, pointMap[a]);
                    }            
                    
                    for(int i = 0; i < constraint->PointsCount(); i++)
                    {
                        constraint->SetPoint(i, pointMap[constraint->Point(i)]);
                    }

                }
                if((!segmentMap.empty()) && (constraint->SegmentsCount() > 0))
                {
                    for(int i = 0; i < constraint->SegmentsCount(); i++)
                    {
                        constraint->SetSegment(i, pointMap[constraint->Segment(i)]);
                    }
                }

                AddConstraint(constraint);
				newConstraints.push_back(nextIndex);
                nextIndex++;
			}
        }

        Invalidate();

        return std::make_tuple(pointMap, segmentMap, newConstraints);
    }

    void Clear()
    {
    }

	int ConstraintsCount() const { return ConstraintsLabel().NbChildren(); }

    bool SolveConstraints(bool canFail, const std::vector<int>& fixedPoints = {});

    void Update();

protected: 
    bool MakeInternal() override
    {
        return false;
    }

private:
    Signal<void(const TDF_Label&)> PointsChanged;

private:
    TDF_Label _Label;
    std::map<int, gp_Pnt2d> pointsMap;
    std::map<int, Handle<SketchSegment>> segmentsMap;
    std::vector<Handle<SketchConstraint>> constraintsArray;

    Handle<SketchElementSegment> element;

    bool _ConstraintSolverFailed;
};

DEFINE_STANDARD_HANDLE(Sketch, Shape2D)

#endif  // _Sketch_Header_File_
