#include "Iact/Shapes/Primitives/CreateBoxTool.hxx"

#include "Iact/HudElements/Coord2DHudElement.hxx"
#include "Iact/ToolActions/PointAction.hxx"
#include "Xcaf/Attributes/DocumentTool.hxx"

bool CreateBoxTool::onStart()
{
    // Implementation for starting the box creation tool
    m_workspaceController->Selector()->ClearPicked();
    myCurrentPhase = PivotPoint;

    auto aPointAction = MakeHandle<PointAction>();

    if(!startAction(aPointAction))
    {
        return false;
    }

	aPointAction->Preview.connect([this](const auto& action) {
		_PivotAction_Preview(action);
	});
	aPointAction->Finished.connect([this](const auto& action) {
		_PivotAction_Finished(action);
	});

    SetHintMessage("Select corner point.");

	_Coord2DHudElement = new Coord2DHudElement();

	Add(_Coord2DHudElement);
    return true;
}

void CreateBoxTool::ensurePreviewShape() 
{
    // add a box
    {
        auto aDocument = InteractiveContext::Current()->GetDocument();
        auto aWorkspace = InteractiveContext::Current()->workspace();
        auto aViewport = InteractiveContext::Current()->viewport();

        aDocument->NewCommand();

        TDF_Label boxsLabel = DocumentTool::BoxsLabel(aDocument->Main());

        m_Box = new NewBox(boxsLabel.NewChild());
        m_Box->Make();

        // Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
        Handle(TPrsStd_AISPresentation) anAisPresentation = TPrsStd_AISPresentation::Set(m_Box->BRepLabel(), TNaming_NamedShape::GetID());
        // Display it
        anAisPresentation->Display(1);
        anAisPresentation->SetMode(1);
        anAisPresentation->SetSelectionMode(1);

        aDocument->CommitCommand();
        aWorkspace->UpdateCurrentViewer();
        aViewport->OnViewMoved();
    }
}

void CreateBoxTool::_PivotAction_Preview(const Handle(PointAction)& action)
{
	_Coord2DHudElement->setValues(action->PointOnPlane().X(), action->PointOnPlane().Y());
}

void CreateBoxTool::_PivotAction_Finished(const Handle(PointAction)& action)
{

    _Plane = m_workspaceController->workspace()->WorkingPlane();
    _PointPlane1 = action->PointOnPlane();

    stopAction(action);

    auto aPointAction = MakeHandle<PointAction>();

    aPointAction->Preview.connect([this](const auto& action) {
        _BaseRectAction_Preview(action);
    });
    aPointAction->Finished.connect([this](const auto& action) {
        _BaseRectAction_Finished(action);
    });

    if(!startAction(aPointAction))
    {
        return ;
    }

    myCurrentPhase = BaseRect;

    if(_MultiValueHudElement == nullptr)
    {
		_MultiValueHudElement = new MultiValueHudElement("Length:", "Width:");
		Add(_MultiValueHudElement);
    }
}

void CreateBoxTool::_BaseRectAction_Preview(const Handle(PointAction)& action)
{
	_PointPlane2 = action->PointOnPlane();

    // 计算尺寸
    double dimX = std::abs(_PointPlane1.X() - _PointPlane2.X());
    double dimY = std::abs(_PointPlane1.Y() - _PointPlane2.Y());
    dimX = std::round(dimX * 1000.0) / 1000.0; // 四舍五入到三位小数
    dimY = std::round(dimY * 1000.0) / 1000.0;

    // 按 Ctrl 键对齐网格
    if((action->GetKeyFlags() & Aspect_VKeyFlags_CTRL) != 0)
    {
        double gridStep = m_workspaceController->workspace()->GetGridStep();
        dimX = std::round(dimX / gridStep) * gridStep;
        dimY = std::round(dimY / gridStep) * gridStep;
    }
    // 确保最小尺寸
    if(std::abs(dimX) <= 0.0)
    {
        dimX = 0.001;
    }
    if(std::abs(dimY) <= 0.0)
    {
        dimY = 0.001;
    }

    // 计算位置
    double posX, posY;
    if(_PointPlane1.X() < _PointPlane2.X())
    {
        posX = _PointPlane1.X();
        _PointPlane2.SetX(_PointPlane1.X() + dimX);
    }
    else
    {
        posX = _PointPlane1.X() - dimX;
        _PointPlane2.SetX(posX);
    }

    if(_PointPlane1.Y() < _PointPlane2.Y())
    {
        posY = _PointPlane1.Y();
        _PointPlane2.SetY(_PointPlane1.Y() + dimY);
    }
    else
    {
        posY = _PointPlane1.Y() - dimY;
        _PointPlane2.SetY(posY);
    }

    // 更新预览几何
    ensurePreviewShape();
    gp_Pnt position = ElSLib::Value(posX, posY, _Plane); // 转换为 3D 点

    // 更新标记位置
    if(!action.IsNull())
    {
        //gp_Pnt markerPos = ElSLib::Value(_PointPlane2.X(), _PointPlane2.Y(), _Plane->Pln()).Rounded(3);
        //action->SetMarkerPosition(markerPos);
    }

    // 更新 HUD
    if(_Coord2DHudElement)
    {
        _Coord2DHudElement->setValues(_PointPlane2.X(), _PointPlane2.Y());
    }
    if(_MultiValueHudElement)
    {
        _MultiValueHudElement->SetValues(dimX, dimY);
    }
}

void CreateBoxTool::_BaseRectAction_Finished(const Handle(PointAction) & action)
{
}

void CreateBoxTool::_HeightAction_Preview(const Handle(PointAction) & action)
{}

void CreateBoxTool::_HeightAction_Finished(const Handle(PointAction) & action)
{}
