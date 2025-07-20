// Copyright [2025] TaCAD

// Own include
#include "App/MainWindow.hxx"

// Qt includes
#include <QAbstractButton>
#include <QAction>
#include <QLabel>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QStatusBar>

// SARibbonBar includes
#include "SARibbon.h"

// ads includes
#include "AutoHideDockContainer.h"
#include "DockAreaTitleBar.h"
#include "DockAreaWidget.h"

#include "App/AppContext.hxx"
#include "App/Commands/AppCommands.hxx"
#include "App/MessagesPanel.hxx"
#include "App/ShapeInspector.hxx"
#include "App/ShortcutDefinition.hxx"
#include "App/ViewportView.hxx"
#include "App/WelcomeDialog.hxx"
#include "Iact/Commands/DocumentCommands.hxx"
#include "Iact/Commands/ModelCommands.hxx"
#include "Iact/Commands/SketchCommands.hxx"
#include "Iact/Commands/WorkspaceCommands.hxx"

MainWindow::MainWindow(QWidget* parent)
    : SARibbonMainWindow(parent)
    , myRibbonBar(nullptr)
    , myDockManager(nullptr)
{
    setupUi();

    setupAppButton();
    setupCategories();

    setupDockWidgets();

    onMainWindowLoaded();
}

MainWindow::~MainWindow()
{}

void MainWindow::onMainWindowLoaded()
{
    AppCommands::InitApplication()->execute();
    emit ApplicationLoaded();

    ModelCommands::CreateSketch()->execute();

    AppContext::Current()->SetMainWidow(this);

    ShortcutDefinition::RegisterShortcuts(this);
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("TaCAD"));
    setWindowIcon(ResourceUtils::icon("App/App-MainLogo"));
    setStatusBar(new QStatusBar());

    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::AlwaysShowTabs, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::ActiveTabHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::XmlCompressionEnabled, false);

    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton, false);
    ads::CDockManager::setAutoHideConfigFlags(ads::CDockManager::DefaultAutoHideConfig);
    ads::CDockManager::setAutoHideConfigFlag(ads::CDockManager::DockAreaHasAutoHideButton, false);

    myDockManager = new ads::CDockManager(this);

    // set ribbonbar
    myRibbonBar = ribbonBar();
    myRibbonBar->setContentsMargins(5, 0, 5, 0);
    myRibbonBar->setPannelLayoutMode(SARibbonPannel::PannelLayoutMode::ThreeRowMode);
    myRibbonBar->setEnableWordWrap(true);
}

void MainWindow::setupAppButton()
{
    if(!myRibbonBar)
    {
        return;
    }

    auto aAppBtn = new SARibbonApplicationButton(this);
    aAppBtn->setText(tr("  &File  "));
    auto aMenu = new QMenu(this);
    {
		aMenu->addAction(DocumentCommands::CreateNewModel());
        aMenu->addAction(DocumentCommands::SaveModel());

        aMenu->addAction(AppCommands::TestCommand());
    }

    aAppBtn->setMenu(aMenu);
    myRibbonBar->setApplicationButton(aAppBtn);
}

void MainWindow::setupCategories()
{
    if(SARibbonCategory* aCategory = myRibbonBar->addCategoryPage(tr("Sketch")))
    {
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Create Segment")))
        {
            aPannel->addLargeAction(SketchCommands::CreateSegmentLine());
            aPannel->addLargeAction(SketchCommands::CreatePolyLine());
            aPannel->addLargeAction(SketchCommands::CreateBezier2());
            aPannel->addLargeAction(SketchCommands::CreateBezier3());
            aPannel->addLargeAction(SketchCommands::CreateRectangle());
            aPannel->addLargeAction(SketchCommands::CreateCircle());
            aPannel->addLargeAction(SketchCommands::CreateArcCenter());
            aPannel->addLargeAction(SketchCommands::CreateArcRim());
            aPannel->addLargeAction(SketchCommands::CreateEllipse());
            aPannel->addLargeAction(SketchCommands::CreateEllipticalArc());
        }
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Create Constraint")))
        {
            aPannel->addSmallAction(SketchCommands::CreateConstraintParallel());
            aPannel->addSmallAction(SketchCommands::CreateConstraintVertical());
            aPannel->addSmallAction(SketchCommands::CreateConstraintHorizontal());

            aPannel->addSmallAction(SketchCommands::CreateConstraintLength());
            aPannel->addSmallAction(SketchCommands::CreateConstraintAngle());
            aPannel->addSmallAction(SketchCommands::CreateConstraintEqual());

            aPannel->addSmallAction(SketchCommands::CreateConstraintPerpendicular());
            aPannel->addSmallAction(SketchCommands::CreateConstraintTangent());
            aPannel->addSmallAction(SketchCommands::CreateConstraintSmoothCorner());

            aPannel->addSmallAction(SketchCommands::CreateConstraintRadius());
            aPannel->addSmallAction(SketchCommands::CreateConstraintHorizontalDistance());
            aPannel->addSmallAction(SketchCommands::CreateConstraintVerticalDistance());

            aPannel->addSmallAction(SketchCommands::CreateConstraintPointOnSegment());
            aPannel->addSmallAction(SketchCommands::CreateConstraintPointOnMidpoint());
            aPannel->addSmallAction(SketchCommands::CreateConstraintFixed());
            aPannel->addSmallAction(SketchCommands::CreateConstraintConcentric());
        }

        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Tools")))
        {
            aPannel->addMediumAction(SketchCommands::StartSketchToolMirror());

            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-SplitTool"), "Split"), SARibbonPannelItem::Medium);
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-ScaleTool"), "Scale"), SARibbonPannelItem::Medium);
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-OffsetTool"), "Offset"), SARibbonPannelItem::Medium);
        }

        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Editor")))
        {
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-RotateLeft"), "Rotate\nLeft"), SARibbonPannelItem::Large);
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-RotateRight"), "Rotate\nRight"), SARibbonPannelItem::Large);
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-ClipPlane"), "Clip\nPlane"), SARibbonPannelItem::Large);
            aPannel->addAction(new QAction(ResourceUtils::icon("Sketch/Sketch-RecenterGrid"), "Recenter\nGrid"), SARibbonPannelItem::Large);
        }
    }

    if(SARibbonCategory* aCategory = myRibbonBar->addCategoryPage(tr("Model")))
    {
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Create")))
        {
            aPannel->addAction(ModelCommands::CreateBox(), SARibbonPannelItem::Large);
            aPannel->addAction(ModelCommands::CreateCylinder(), SARibbonPannelItem::Large);
            aPannel->addAction(ModelCommands::CreateSphere(), SARibbonPannelItem::Large);

            auto aRenderAction = ModelCommands::CreateSketch();
            auto aMenu = new QMenu(this);
            aRenderAction->setMenu(aMenu);
            aMenu->setTitle(tr("Sketch"));
            auto aChild1 = aMenu->addAction(ResourceUtils::icon("Edit/WorkingPlane-SetXY"), "WorkingPlane XY");
            connect(aChild1, &QAction::triggered, [=]() {
                ModelCommands::CreateSketchAligned()->execute();
            });

            auto aChild2 = aMenu->addAction(ResourceUtils::icon("Edit/WorkingPlane-SetXZ"), "WorkingPlane XZ");
            connect(aChild2, &QAction::triggered, [=]() {
                ModelCommands::CreateSketchAligned()->execute();
            });

            auto aChild3 = aMenu->addAction(ResourceUtils::icon("Edit/WorkingPlane-SetZY"), "WorkingPlane ZY");
            connect(aChild3, &QAction::triggered, [=]() {
                ModelCommands::CreateSketchAligned()->execute();
            });

            aPannel->addMenu(aMenu, SARibbonPannelItem::Large, QToolButton::MenuButtonPopup);
        }
    }

    if(SARibbonCategory* aCategory = myRibbonBar->addCategoryPage(tr("Edit")))
    {
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Undo")))
        {
            aPannel->addAction(WorkspaceCommands::DoUndo(), SARibbonPannelItem::Large);
            aPannel->addAction(WorkspaceCommands::DoRedo(), SARibbonPannelItem::Large);
        }
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Grid")))
        {
            auto aToggleAction = WorkspaceCommands::ToggleGrid();

            auto aGridMenu = new QMenu(this);
            aGridMenu->setTitle(tr("Show Grid"));
            aToggleAction->setMenu(aGridMenu);

            // Rectangular Grid
            auto aRectAction = aGridMenu->addAction(ResourceUtils::icon("View/Grid-Rect"), tr("Rectangular Grid"));
            connect(aRectAction, &QAction::triggered, [=]() {
                WorkspaceCommands::SetGridType()->execute(GridTypes::Rectangular);
                aToggleAction->setIcon(aRectAction->icon());
                aGridMenu->setDefaultAction(aRectAction);
            });

            // Circular Grid
            auto aCircAction = aGridMenu->addAction(ResourceUtils::icon("View/Grid-Circ"), tr("Circular Grid"));
            connect(aCircAction, &QAction::triggered, [=]() {
                WorkspaceCommands::SetGridType()->execute(GridTypes::Circular);
                aToggleAction->setIcon(aCircAction->icon());
                aGridMenu->setDefaultAction(aCircAction);
            });

            // Ìí¼Óµ½Ãæ°å
            aPannel->addMenu(aGridMenu, SARibbonPannelItem::Large, QToolButton::MenuButtonPopup);
            connect(this, &MainWindow::ApplicationLoaded, aRectAction, &QAction::trigger);
        }
    }

    if(SARibbonCategory* aCategory = myRibbonBar->addCategoryPage(tr("View")))
    {
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("View")))
        {
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Top));
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Bottom));
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Left));
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Right));
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Front));
            aPannel->addAction(WorkspaceCommands::setPredefinedView(ViewportController::Back));
        }
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Zoom")))
        {
            aPannel->addAction(WorkspaceCommands::ZoomFitAll());
            aPannel->addAction(WorkspaceCommands::ZoomIn());
            aPannel->addAction(WorkspaceCommands::ZoomOut());
        }
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Display")))
        {
            auto aRenderAction = WorkspaceCommands::SetRenderMode();
            auto aMenu = new QMenu(this);           
            aRenderAction->setMenu(aMenu);
            aMenu->setTitle(tr("Render Mode"));
            auto aChild1 = aMenu->addAction(ResourceUtils::icon("View/View-RenderSolidShaded"), "Solid Shaded");
            connect(aChild1, &QAction::triggered, [=]() {
                aRenderAction->execute(Viewport::RenderModes::SolidShaded);
                aRenderAction->setIcon(aChild1->icon());
                aRenderAction->menu()->setDefaultAction(aChild1);
            });

            auto child2 = aMenu->addAction(ResourceUtils::icon("View/View-RenderHLR"), "HLR");
            connect(child2, &QAction::triggered, [=]() {
                aRenderAction->execute(Viewport::RenderModes::HLR);
                aRenderAction->setIcon(child2->icon());
                aRenderAction->menu()->setDefaultAction(child2);
            });

            auto child3 = aMenu->addAction(ResourceUtils::icon("View/View-RenderRaytraced"), "Raytraced");
            connect(child3, &QAction::triggered, [=]() {
                aRenderAction->execute(Viewport::RenderModes::Raytraced);
                aRenderAction->setIcon(child3->icon());
                aRenderAction->menu()->setDefaultAction(child3);
            });

            aPannel->addMenu(aMenu, SARibbonPannelItem::Large, QToolButton::InstantPopup);
            connect(this, &MainWindow::ApplicationLoaded, aChild1, &QAction::trigger);
        }
    }

    if(SARibbonCategory* aCategory = myRibbonBar->addCategoryPage(tr("Tools")))
    {
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Convert")))
        {
        }
        if(SARibbonPannel* aPannel = aCategory->addPannel(tr("Wrokflow")))
        {
            aPannel->addAction(AppCommands::ShowShapeInspector());
        }
    }
}

void MainWindow::setupDockWidgets()
{
    // Set up a central dock widget 
    ads::CDockWidget* CentralDockWidget = new ads::CDockWidget("Workspace");

    CentralDockWidget->setWidget(new ViewportView());
    auto* CentralDockArea = myDockManager->setCentralWidget(CentralDockWidget);

    // Set up additional dock widgets for various panels
    ads::CDockWidget* documentDock = new ads::CDockWidget("Document");
    documentDock->setWidget(new WelcomeDialog());

    ads::CDockWidget* layersDock = new ads::CDockWidget("Layers");
    layersDock->setWidget(new WelcomeDialog());

    ads::CDockWidget* propertiesDock = new ads::CDockWidget("Properties");
    propertiesDock->setWidget(new WelcomeDialog());

    ads::CDockWidget* messageDock = new ads::CDockWidget("Message");
    messageDock->setWidget(new MessagesPanel());

    ads::CDockWidget* shapeInspector = new ads::CDockWidget("Inspector");
    shapeInspector->setWidget(new ShapeInspector());

    // add dock widgets to specific dock areas
    myDockManager->addAutoHideDockWidget(ads::SideBarLocation::SideBarRight, propertiesDock)->setSize(240);
    myDockManager->addAutoHideDockWidget(ads::SideBarLocation::SideBarLeft, documentDock)->setSize(240);
    myDockManager->addAutoHideDockWidget(ads::SideBarLocation::SideBarLeft, layersDock)->setSize(240);
    myDockManager->addAutoHideDockWidget(ads::SideBarLocation::SideBarBottom, messageDock)->setSize(240);
    myDockManager->addAutoHideDockWidget(ads::SideBarLocation::SideBarRight, shapeInspector)->setSize(240);
}

QAction* MainWindow::createAction(const QString& text, const QString& iconurl)
{
    QAction* action = new QAction(this);
    action->setText(text);
    action->setIcon(QIcon(iconurl));
    action->setObjectName(text);
    return action;
}
