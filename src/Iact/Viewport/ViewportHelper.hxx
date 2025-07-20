
#ifndef _OcctGlTools_HeaderFile
#define _OcctGlTools_HeaderFile

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <Aspect_VKeyFlags.hxx>
#include <Aspect_VKey.hxx>
#include <V3d_View.hxx>

class OpenGl_Context;

class ViewportHelper
{
public:
	//! Return GL context.
	static Handle(OpenGl_Context) getGlContext(const Handle(V3d_View)& theView);

	//! Map Qt buttons bitmask to virtual keys.
	static Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons);

	//! Map Qt mouse modifiers bitmask to virtual keys.
	static Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers);

	//! Map Qt key to virtual key.
	static Aspect_VKey qtKey2VKey(int theKey);
};

#endif // _OcctGlTools_HeaderFile
