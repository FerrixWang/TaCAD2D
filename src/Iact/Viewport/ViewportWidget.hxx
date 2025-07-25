
#ifndef _OcctQtViewer_HeaderFile
#define _OcctQtViewer_HeaderFile

#include <Standard_WarningsDisable.hxx>
#include <QOpenGLWidget>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>

#include "Iact/ViewportController.hxx"

class AIS_ViewCube;

//! OCCT 3D View.
class ViewportWidget : public QOpenGLWidget
{
    Q_OBJECT
public:

    ViewportWidget(const Handle<ViewportController>& controller,
                   QWidget* parent = nullptr);

    //! Destructor.
    virtual ~ViewportWidget();

    //! Return Viewer.
    const Handle<V3d_Viewer>& Viewer() const { return myViewer; }

    //! Return View.
    const Handle<V3d_View>& View() const { return myView; }

    //! Return AIS context.
    const Handle<AIS_InteractiveContext>& Context() const { return myContext; }

    //! setter

    //! Return OpenGL info.
    const QString& getGlInfo() const { return myGlInfo; }

    //! Minial widget size.
    virtual QSize minimumSizeHint() const override { return QSize(200, 200); }

    //! Default widget size.
    virtual QSize sizeHint()        const override { return QSize(720, 480); }

protected: // OpenGL events

    virtual void initializeGL() override;
    virtual void paintGL() override;
    //virtual void resizeGL(int , int ) override;

protected: // user input events

    virtual void closeEvent(QCloseEvent* theEvent) override;
    virtual void keyPressEvent(QKeyEvent* theEvent) override;
    virtual void mousePressEvent(QMouseEvent* theEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent* theEvent) override;
    virtual void mouseMoveEvent(QMouseEvent* theEvent) override;
    virtual void wheelEvent(QWheelEvent* theEvent) override;

private:

    //! Dump OpenGL info.
    void dumpGlInfo(bool theIsBasic, bool theToPrint);

    //! Request widget paintGL() event.
    void updateView();

private:
    Handle<ViewportController>     m_viewportController;

    Handle<V3d_Viewer>             myViewer;
    Handle<V3d_View>               myView;
    Handle<AIS_InteractiveContext> myContext;
	Handle<AIS_ViewCube>          myViewCube;

    Handle<V3d_View>               myFocusView;
    QString myGlInfo;
    bool myIsCoreProfile;
};

#endif // _OcctQtViewer_HeaderFile
