#include "Iact/Viewport/ViewportWidget.hxx"

#ifdef _WIN32
#include <windows.h>
#endif
#include <OpenGl_Context.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QMessageBox>
#include <QMouseEvent>
#include <Standard_WarningsRestore.hxx>

#include <AIS_Shape.hxx>
#include <AIS_ViewCube.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_NeutralWindow.hxx>
#include <Message.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_FrameBuffer.hxx>

#include "Iact/Viewport/ViewportFrameBuffer.hxx"
#include "Iact/Viewport/ViewportHelper.hxx"

ViewportWidget::ViewportWidget(const Handle(ViewportController)& controller, QWidget* parent)
    : QOpenGLWidget(parent)
    , m_viewportController(controller)
    , myContext(controller->GetViewport()->GetWorkspace()->AisContext())
    , myView(controller->GetViewport()->V3dView())
    , myViewer(controller->GetViewport()->GetWorkspace()->V3dViewer())
    , myFocusView(controller->FocusView())
    , myIsCoreProfile(true)
{
    m_viewportController->SetOpenglWidget(this);

    myViewCube = m_viewportController->ViewCube();

    // Qt widget setup
    setMouseTracking(true);
    setBackgroundRole(QPalette::NoRole); // or NoBackground
    setFocusPolicy(Qt::StrongFocus); // set focus policy to threat QContextMenuEvent from keyboard
    setUpdatesEnabled(true);
    setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);

    auto myViewer = myView->Viewer();
    Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast(myViewer->Driver());

    // OpenGL setup managed by Qt
    QSurfaceFormat aGlFormat;
    aGlFormat.setDepthBufferSize(24);
    aGlFormat.setStencilBufferSize(8);
    //aGlFormat.setOption (QSurfaceFormat::DebugContext, true);
    aDriver->ChangeOptions().contextDebug = aGlFormat.testOption(QSurfaceFormat::DebugContext);
    //aGlFormat.setOption (QSurfaceFormat::DeprecatedFunctions, true);
    if(myIsCoreProfile)
    {
        aGlFormat.setVersion(4, 5);
    }
    aGlFormat.setProfile(myIsCoreProfile ? QSurfaceFormat::CoreProfile : QSurfaceFormat::CompatibilityProfile);

    // request sRGBColorSpace colorspace to meet OCCT expectations or use ViewportFrameBuffer fallback.
  /*#if (QT_VERSION_MAJOR > 5) || (QT_VERSION_MAJOR == 5 && QT_VERSION_MINOR >= 10)
    aGlFormat.setColorSpace (QSurfaceFormat::sRGBColorSpace);
    setTextureFormat (GL_SRGB8_ALPHA8);
  #else
    Message::SendWarning ("Warning! Qt 5.10+ is required for sRGB setup.\n"
                          "Colors in 3D viewer might look incorrect (Qt " QT_VERSION_STR " is used).\n");
    aDriver->ChangeOptions().sRGBDisable = true;
  #endif*/

    setFormat(aGlFormat);

#if defined(_WIN32)
    // never use ANGLE on Windows, since OCCT 3D viewer does not expect this
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    //QCoreApplication::setAttribute (Qt::AA_UseOpenGLES);
#endif
}

// ================================================================
// Function : ~ViewportWidget
// Purpose  :
// ================================================================
ViewportWidget::~ViewportWidget()
{}

// ================================================================
// Function : dumpGlInfo
// Purpose  :
// ================================================================
void ViewportWidget::dumpGlInfo(bool theIsBasic, bool theToPrint)
{
    TColStd_IndexedDataMapOfStringString aGlCapsDict;
    myView->DiagnosticInformation(aGlCapsDict, theIsBasic ? Graphic3d_DiagnosticInfo_Basic : Graphic3d_DiagnosticInfo_Complete);
    TCollection_AsciiString anInfo;
    for(TColStd_IndexedDataMapOfStringString::Iterator aValueIter(aGlCapsDict); aValueIter.More(); aValueIter.Next())
    {
        if(!aValueIter.Value().IsEmpty())
        {
            if(!anInfo.IsEmpty())
            {
                anInfo += "\n";
            }
            anInfo += aValueIter.Key() + ": " + aValueIter.Value();
        }
    }

    if(theToPrint)
    {
        Message::SendInfo(anInfo);
    }
    myGlInfo = QString::fromUtf8(anInfo.ToCString());
}

// ================================================================
// Function : initializeGL
// Purpose  :
// ================================================================
void ViewportWidget::initializeGL()
{
    const QRect aRect = rect();
    const Graphic3d_Vec2i aViewSize(aRect.right() - aRect.left(), aRect.bottom() - aRect.top());

    Handle(OpenGl_Context) aGlCtx = new OpenGl_Context();
    if(!aGlCtx->Init(myIsCoreProfile))
    {
        Message::SendFail() << "Error: OpenGl_Context is unable to wrap OpenGL context";
        QMessageBox::critical(0, "Failure", "OpenGl_Context is unable to wrap OpenGL context");
        QApplication::exit(1);
        return;
    }

    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast(myView->Window());
    if(!aWindow.IsNull())
    {
        aWindow->SetSize(aViewSize.x(), aViewSize.y());
        myView->SetWindow(aWindow, aGlCtx->RenderingContext());
        dumpGlInfo(true, true);
    }
    else
    {
        aWindow = new Aspect_NeutralWindow();
        aWindow->SetVirtual(true);

        Aspect_Drawable aNativeWin = (Aspect_Drawable)winId();
#ifdef _WIN32
        //HGLRC aWglCtx    = wglGetCurrentContext();
        HDC   aWglDevCtx = wglGetCurrentDC();
        HWND  aWglWin = WindowFromDC(aWglDevCtx);
        aNativeWin = (Aspect_Drawable)aWglWin;
#endif
        aWindow->SetNativeHandle(aNativeWin);
        aWindow->SetSize(aViewSize.x(), aViewSize.y());
        myView->SetWindow(aWindow, aGlCtx->RenderingContext());
        dumpGlInfo(true, true);

        myContext->Display(myViewCube, 0, 0, false);
    }
}

// ================================================================
// Function : closeEvent
// Purpose  :
// ================================================================
void ViewportWidget::closeEvent(QCloseEvent* theEvent)
{
    theEvent->accept();
}

// ================================================================
// Function : keyPressEvent
// Purpose  :
// ================================================================
void ViewportWidget::keyPressEvent(QKeyEvent* theEvent)
{
    Aspect_VKey aKey = ViewportHelper::qtKey2VKey(theEvent->key());
    switch(aKey)
    {
    case Aspect_VKey_Escape:
    {
        QApplication::exit();
        return;
    }
    case Aspect_VKey_F:
    {
        myView->FitAll(0.01, false);
        update();
        return;
    }
    }
    QOpenGLWidget::keyPressEvent(theEvent);
}

// ================================================================
// Function : mousePressEvent
// Purpose  :
// ================================================================
void ViewportWidget::mousePressEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mousePressEvent(theEvent);
    const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
    if(!myView.IsNull()
       && m_viewportController->onMousePress(aPnt,
       ViewportHelper::qtMouseButtons2VKeys(theEvent->buttons()),
       ViewportHelper::qtMouseModifiers2VKeys(theEvent->modifiers())
       ))
    {
        updateView();
    }
}

// ================================================================
// Function : mouseReleaseEvent
// Purpose  :
// ================================================================
void ViewportWidget::mouseReleaseEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mouseReleaseEvent(theEvent);
    const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
    if(!myView.IsNull()
       && m_viewportController->onMouseRelease(aPnt,
       ViewportHelper::qtMouseButtons2VKeys(theEvent->buttons()),
       ViewportHelper::qtMouseModifiers2VKeys(theEvent->modifiers())
       ))
    {
        updateView();
    }
}

// ================================================================
// Function : mouseMoveEvent
// Purpose  :
// ================================================================
void ViewportWidget::mouseMoveEvent(QMouseEvent* theEvent)
{
    QOpenGLWidget::mouseMoveEvent(theEvent);
    const Graphic3d_Vec2i aNewPos(theEvent->pos().x(), theEvent->pos().y());
    if(!myView.IsNull()
       && m_viewportController->onMouseMove(aNewPos,
       ViewportHelper::qtMouseButtons2VKeys(theEvent->buttons()),
       ViewportHelper::qtMouseModifiers2VKeys(theEvent->modifiers())
       ))
    {
        updateView();
    }
}

// ==============================================================================
// function : wheelEvent
// purpose  :
// ==============================================================================
void ViewportWidget::wheelEvent(QWheelEvent* theEvent)
{
    QOpenGLWidget::wheelEvent(theEvent);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    const Graphic3d_Vec2i aPos(Graphic3d_Vec2d(theEvent->position().x(), theEvent->position().y()));
#else
    const Graphic3d_Vec2i aPos(theEvent->pos().x(), theEvent->pos().y());
#endif
    if(myView.IsNull())
    {
        return;
    }

    if(!myView->Subviews().IsEmpty())
    {
        Handle(V3d_View) aPickedView = myView->PickSubview(aPos);
        if(!aPickedView.IsNull()
           && aPickedView != myFocusView)
        {
            // switch input focus to another subview
            m_viewportController->OnSubviewChanged(myContext, myFocusView, aPickedView);
            updateView();
            return;
        }
    }

    if(m_viewportController->UpdateZoom(Aspect_ScrollDelta(aPos, double(theEvent->angleDelta().y()) / 8.0)))
    {
        updateView();
    }
}

// =======================================================================
// function : updateView
// purpose  :
// =======================================================================
void ViewportWidget::updateView()
{
    update();
    //if (window() != NULL) { window()->update(); }
}

// ================================================================
// Function : paintGL
// Purpose  :
// ================================================================
void ViewportWidget::paintGL()
{
    if(myView->Window().IsNull())
    {
        return;
    }

    // wrap FBO created by QOpenGLWidget
    // get context from this (composer) view rather than from arbitrary one
    //Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (myContext->CurrentViewer()->Driver());
    //Handle(OpenGl_Context) aGlCtx = aDriver->GetSharedContext();
    Handle(OpenGl_Context) aGlCtx = ViewportHelper::getGlContext(myView);
    Handle(OpenGl_FrameBuffer) aDefaultFbo = aGlCtx->DefaultFrameBuffer();
    if(aDefaultFbo.IsNull())
    {
        aDefaultFbo = new ViewportFrameBuffer();
        aGlCtx->SetDefaultFrameBuffer(aDefaultFbo);
    }
    if(!aDefaultFbo->InitWrapper(aGlCtx))
    {
        aDefaultFbo.Nullify();
        Message::SendFail("Default FBO wrapper creation failed");
        QMessageBox::critical(0, "Failure", "Default FBO wrapper creation failed");
        QApplication::exit(1);
        return;
    }

    Graphic3d_Vec2i aViewSizeOld;
    const QRect aRect = rect(); Graphic3d_Vec2i aViewSizeNew(aRect.right() - aRect.left(), aRect.bottom() - aRect.top());
    // Graphic3d_Vec2i aViewSizeNew = aDefaultFbo->GetVPSize();
    Handle(Aspect_NeutralWindow) aWindow = Handle(Aspect_NeutralWindow)::DownCast(myView->Window());
    aWindow->Size(aViewSizeOld.x(), aViewSizeOld.y());
    if(aViewSizeNew != aViewSizeOld)
    {
        aWindow->SetSize(aViewSizeNew.x(), aViewSizeNew.y());
        myView->MustBeResized();
        myView->Invalidate();
        dumpGlInfo(true, false);

        for(const Handle(V3d_View)& aSubviewIter : myView->Subviews())
        {
            aSubviewIter->MustBeResized();
            aSubviewIter->Invalidate();
            aDefaultFbo->SetupViewport(aGlCtx);
        }
    }

    // flush pending input events and redraw the viewer
    Handle(V3d_View) aView = !myFocusView.IsNull() ? myFocusView : myView;
    aView->InvalidateImmediate();
    m_viewportController->FlushViewEvents(myContext, aView, true);
}
