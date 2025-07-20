// class DocumentTool

#ifndef _DocumentTool_Header_File_
#define _DocumentTool_Header_File_

#include <TDataStd_GenericEmpty.hxx>
#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_CopyTool.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_AttributeIterator.hxx>

#include "Core/CoreContext.hxx"

class DocumentTool : public TDataStd_GenericEmpty
{
public:
	static const Standard_GUID GetID();

	static Handle(DocumentTool) Set(
		const TDF_Label& L,
		const Standard_Boolean IsAcces = true);

	bool IsCoreDocument(const Handle(TDocStd_Document)& D);

	static TDF_Label DocLabel(const TDF_Label& acces = CoreContext::Current()->MainLabel());

	static TDF_Label EntitysLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label ShapesLabel(const TDF_Label& acces = CoreContext::Current()->MainLabel());

	static TDF_Label Shape2dsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label SketchsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label BoxsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label InteractiveEntitysLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label BodysLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label WorkspacesLabel(const TDF_Label& acces = CoreContext::Current()->MainLabel());

	static TDF_Label PointsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label TrashLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

	static TDF_Label SketchSegmentsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchElementsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchElementSegmentsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchElementPointsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentLinesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentCirclesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentArcsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchSegmentCircleArcsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentBeziersLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentEllipsesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchSegmentEllipticalArcsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintVerticalsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintParallelsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintHorizontalsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintLengthsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintAnglesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintEqualsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintFixedsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintHorizontalDistancesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintPerpendicularsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintPointOnMidpointsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintPointOnSegmentsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintRadiusLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintSmoothCornersLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintTangentsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());
    
    static TDF_Label SketchConstraintVerticalDistancesLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    static TDF_Label SketchConstraintConcentricsLabel(const TDF_Label& access = CoreContext::Current()->MainLabel());

    // 移动标签及其子标签树到新父标签下
    static bool MoveLabel(const TDF_Label& sourceLabel, const TDF_Label& newParentLabel)
    {
        if(!CopyLabel(sourceLabel, newParentLabel))
        {
            return false;
        }

        DeleteLabel(sourceLabel);

        return true;
    }

    // 复制标签及其子标签树到新父标签下
    static bool CopyLabel(const TDF_Label& sourceLabel, const TDF_Label& newParentLabel)
    {
        // 检查标签有效性
        if(sourceLabel.IsNull() || newParentLabel.IsNull())
        {
            return false;
        }

        // 创建新子标签作为复制目标
        TDF_Label newLabel = newParentLabel;

        // 初始化 TDF_DataSet
        Handle(TDF_DataSet) dataSet = new TDF_DataSet();
        AddLabelWithChildren(sourceLabel, dataSet);
        dataSet->Roots().Append(sourceLabel); // 设置根标签

        // 初始化 TDF_RelocationTable
        Handle(TDF_RelocationTable) relocationTable = new TDF_RelocationTable();
        relocationTable->LabelTable().Bind(sourceLabel, newLabel); // 预绑定根标签

        // 执行复制
        TDF_CopyTool::Copy(dataSet, relocationTable);
        return true;
    }

	static void DeleteLabel(const TDF_Label& label)
	{
		if(label.IsNull())
		{
			return;
		}

		// 删除标签及其子标签树
		label.ForgetAllAttributes(true); // true 表示递归删除子标签
	}

    static std::string GetName(const TDF_Label& label)
    {
		if(label.IsNull())
		{
			return "";
		}

		// 获取标签的名称
		Handle(TDataStd_Name) nameAttr;
		if(label.FindAttribute(TDataStd_Name::GetID(), nameAttr))
		{
			return OcctHelper::ExtendedStringToStdString(nameAttr->Get());
		}

		return "";
    }

private:
    // 递归添加标签及其子标签到 TDF_DataSet
    static void AddLabelWithChildren(const TDF_Label& label, Handle(TDF_DataSet)& dataSet)
    {
        if(label.IsNull())
        {
            return;
        }

        // 添加当前标签
        dataSet->AddLabel(label);

		// 添加子标签下的所有Attribute

        // 添加当前标签的所有属性
        for(TDF_AttributeIterator attrIt(label); attrIt.More(); attrIt.Next())
        {
            dataSet->AddAttribute(attrIt.Value());
        }

        // 递归添加子标签
        for(TDF_ChildIterator it(label); it.More(); it.Next())
        {
            AddLabelWithChildren(it.Value(), dataSet);
        }
    }

public:
	DocumentTool();

	void Init() const;

	const Standard_GUID& ID() const override;

	bool AfterRetrieval(const bool forceIt = false)
		override;

	DEFINE_DERIVED_ATTRIBUTE(DocumentTool, TDataStd_GenericEmpty)
};

DEFINE_STANDARD_HANDLE(DocumentTool, TDF_Attribute)

#endif  // _DocumentTool_Header_File_
