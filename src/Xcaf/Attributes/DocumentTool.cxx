#include "Xcaf/Attributes/DocumentTool.hxx"

#include <TDataStd_Name.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_DerivedAttribute.hxx>

#include "Xcaf/Attributes/ShapeTool.hxx"
#include "Xcaf/Attributes/WorkspaceAttribute.hxx"


//Root
//©À©¤©¤[TreeNode:DocumentToolRefID]
//©¦   ©¸©¤©¤ DocLabel(Root / 1)
//©¦       ©À©¤©¤[Child 1] Workspace          ¡û WorkspacesLabel()
//©¦       ©À©¤©¤[Child 3] Trash              ¡û TrashLabel()
//©¦       ©¸©¤©¤[Child 2] Entitys            ¡û EntitysLabel()
//©¦           ©À©¤©¤[Child 2] Shapes         ¡û ShapesLabel()
//©¦           ©¦   ©À©¤©¤[Child 1] 2DShapes   ¡û Shape2dsLabel()
//©¦           ©¦   ©¦   ©¸©¤©¤[Child 1] Sketches ¡û SketchsLabel()
//©¦           ©¦   ©¸©¤©¤[Child 2] Boxes      ¡û BoxsLabel()
//©¦           ©¸©¤©¤[Child 1] InteractiveEntities ¡û InteractiveEntitysLabel()
//©¦               ©¸©¤©¤[Child 1] Bodies     ¡û BodysLabel()
//©¦        -©¤©¤[Child 4] Points            ¡û PointsLabel()


IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(DocumentTool,
                                      TDataStd_GenericEmpty,
                                      "xcaf",
                                      "DocumentTool")

namespace
{
//=======================================================================
// function : GetRefID
// purpose  : Returns a reference id to find a tree node attribute at the root
//           label
//=======================================================================

static const Standard_GUID& GetDocumentToolRefID()
{
    static Standard_GUID DocumentToolRefID("69d5f65b-3d63-1277-6f0e-a6fabed127f1");
    return DocumentToolRefID;
}
} // namespace

const Standard_GUID DocumentTool::GetID()
{
    static Standard_GUID guid("69d5f65a-3d63-1277-6f0e-a6fabed127f1");
    return guid;
}

Handle(DocumentTool) DocumentTool::Set(const TDF_Label& L, 
                                       const Standard_Boolean IsAcces)
{
    Handle(DocumentTool) A;
    TDF_Label                    aL = DocLabel(L);
    if(!aL.FindAttribute(DocumentTool::GetID(), A))
    {
        if(!IsAcces)
            aL = L;

        A = new DocumentTool;
        aL.AddAttribute(A);
        A->Init();
    }

    return A;
}

bool DocumentTool::IsCoreDocument(const Handle(TDocStd_Document)& D)
{
    TDF_Label                 RootL = D->Main().Root();
    const Standard_GUID& aRefGuid = GetDocumentToolRefID();
    Handle(TDataStd_TreeNode) aRootNode;
    return RootL.FindAttribute(aRefGuid, aRootNode);
}

TDF_Label DocumentTool::DocLabel(const TDF_Label& acces)
{
    TDF_Label                 DocL, RootL = acces.Root();
    const Standard_GUID& aRefGuid = GetDocumentToolRefID();
    Handle(TDataStd_TreeNode) aRootNode, aLabNode;

    if(RootL.FindAttribute(aRefGuid, aRootNode))
    {
        aLabNode = aRootNode->First();
        DocL = aLabNode->Label();
        return DocL;
    }

    DocL = RootL.FindChild(1);
    return DocL;
}

TDF_Label DocumentTool::TrashLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "Trash");
    return L;
}

TDF_Label DocumentTool::WorkspacesLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "Workspaces");
    return L;
}

TDF_Label DocumentTool::EntitysLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(2, Standard_True);
    TDataStd_Name::Set(L, "Entitys");
    return L;
}

TDF_Label DocumentTool::PointsLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(3, Standard_True);
    TDataStd_Name::Set(L, "Trash");
    return L;
}

TDF_Label DocumentTool::SketchSegmentsLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(4, Standard_True);
    TDataStd_Name::Set(L, "SketchSegments");
    return L;
}

TDF_Label DocumentTool::SketchConstraintsLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(5, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraints");
    return L;
}

TDF_Label DocumentTool::SketchElementsLabel(const TDF_Label& access)
{
    TDF_Label L = DocLabel(access).FindChild(6, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraints");
    return L;
}

TDF_Label DocumentTool::SketchElementSegmentsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchElementsLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "SketchElementSegments");
    return L;
}

TDF_Label DocumentTool::SketchElementPointsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchElementsLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentPoints");
    return L;
}

TDF_Label DocumentTool::SketchSegmentLinesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentLines");
    return L;
}

TDF_Label DocumentTool::SketchSegmentCirclesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentCircles");
    return L;
}

TDF_Label DocumentTool::SketchSegmentBeziersLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(2, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentCircles");
    return L;
}

TDF_Label DocumentTool::SketchSegmentArcsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(3, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentArcs");
    return L;
}

TDF_Label DocumentTool::SketchSegmentCircleArcsLabel(const TDF_Label& access) 
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(4, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentCircleArcs");
    return L;
}

TDF_Label DocumentTool::SketchSegmentEllipsesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(5, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentEllipses");
    return L;
}

TDF_Label DocumentTool::SketchSegmentEllipticalArcsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchSegmentsLabel(access).FindChild(6, Standard_True);
    TDataStd_Name::Set(L, "SketchSegmentEllipticalArcs");
    return L;
}

TDF_Label DocumentTool::SketchConstraintParallelsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintParallels");
    return L;
}

TDF_Label DocumentTool::SketchConstraintVerticalsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintVerticals");
    return L;
}

TDF_Label DocumentTool::SketchConstraintHorizontalsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(2, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintVerticals");
    return L;
}

TDF_Label DocumentTool::SketchConstraintLengthsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(3, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintVerticals");
    return L;
}

TDF_Label DocumentTool::SketchConstraintAnglesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(4, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintAngles");
    return L;
}

TDF_Label DocumentTool::SketchConstraintEqualsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(5, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintEquals");
    return L;
}

TDF_Label DocumentTool::SketchConstraintFixedsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(6, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintFixeds");
    return L;
}

TDF_Label DocumentTool::SketchConstraintHorizontalDistancesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(7, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintHorizontalDistances");
    return L;
}

TDF_Label DocumentTool::SketchConstraintPerpendicularsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(8, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintPerpendiculars");
    return L;
}

TDF_Label DocumentTool::SketchConstraintPointOnMidpointsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(9, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintPointOnMidpoints");
    return L;
}

TDF_Label DocumentTool::SketchConstraintPointOnSegmentsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(10, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintPointOnSegments");
    return L;
}

TDF_Label DocumentTool::SketchConstraintRadiusLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(11, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintRadius");
    return L;
}

TDF_Label DocumentTool::SketchConstraintSmoothCornersLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(12, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintSmoothCorners");
    return L;
}

TDF_Label DocumentTool::SketchConstraintTangentsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(13, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintTangents");
    return L;
}

TDF_Label DocumentTool::SketchConstraintVerticalDistancesLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(14, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintTangents");
    return L;
}

TDF_Label DocumentTool::SketchConstraintConcentricsLabel(const TDF_Label& access)
{
    TDF_Label L = SketchConstraintsLabel(access).FindChild(15, Standard_True);
    TDataStd_Name::Set(L, "SketchConstraintConcentrics");
    return L;
}

TDF_Label DocumentTool::ShapesLabel(const TDF_Label& access)
{
    TDF_Label L = EntitysLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "Shapes");
    return L;
}

TDF_Label DocumentTool::InteractiveEntitysLabel(const TDF_Label& access)
{
    TDF_Label L = EntitysLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "InteractiveEntitys");
    return L;
}

TDF_Label DocumentTool::Shape2dsLabel(const TDF_Label& access)
{
    TDF_Label L = ShapesLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "Shape2ds");
    return L;
}

TDF_Label DocumentTool::BoxsLabel(const TDF_Label& access)
{
    TDF_Label L = ShapesLabel(access).FindChild(1, Standard_True);
    TDataStd_Name::Set(L, "Boxs");
    return L;
}

TDF_Label DocumentTool::SketchsLabel(const TDF_Label& access)
{
    TDF_Label L = Shape2dsLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "Sketchs");
    return L;
}

TDF_Label DocumentTool::BodysLabel(const TDF_Label& access)
{
    TDF_Label L = InteractiveEntitysLabel(access).FindChild(0, Standard_True);
    TDataStd_Name::Set(L, "Bodys");
    return L;
}

DocumentTool::DocumentTool() {}

const Standard_GUID& DocumentTool::ID() const
{
    return GetID();
}

void DocumentTool::Init() const
{
    TDF_Label                 DocL = Label(), RootL = DocL.Root();
    const Standard_GUID& aRefGuid = GetDocumentToolRefID();
    Handle(TDataStd_TreeNode) aRootNode, aLabNode;

    if(!RootL.FindAttribute(aRefGuid, aRootNode))
    {
        Handle(TDataStd_TreeNode) aRootNodeNew = TDataStd_TreeNode::Set(RootL, aRefGuid);
        Handle(TDataStd_TreeNode) aLNode = TDataStd_TreeNode::Set(DocL, aRefGuid);
        aLNode->SetFather(aRootNodeNew);
        aRootNodeNew->SetFirst(aLNode);
    }
}

bool DocumentTool::AfterRetrieval(const bool forceIt)
{
    Init();
    return true;
}
