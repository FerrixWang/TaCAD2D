
#ifndef _ViewportFrameBuffer_Header_File_
#define _ViewportFrameBuffer_Header_File_

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Context.hxx>

//! OpenGL FBO subclass for wrapping FBO created by Qt using GL_RGBA8 texture format instead of GL_SRGB8_ALPHA8.
//! This FBO is set to OpenGl_Context::SetDefaultFrameBuffer() as a final target.
//! Subclass calls OpenGl_Context::SetFrameBufferSRGB() with sRGB=false flag,
//! which asks OCCT to disable GL_FRAMEBUFFER_SRGB and apply sRGB gamma correction manually.
class ViewportFrameBuffer : public OpenGl_FrameBuffer
{
    DEFINE_STANDARD_RTTI_INLINE(ViewportFrameBuffer, OpenGl_FrameBuffer)
public:
    //! Empty constructor.
    ViewportFrameBuffer() {}

    //! Make this FBO active in context.
    virtual void BindBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindBuffer(theGlCtx);
        theGlCtx->SetFrameBufferSRGB(true, false);
    }

    //! Make this FBO as drawing target in context.
    virtual void BindDrawBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindDrawBuffer(theGlCtx);
        theGlCtx->SetFrameBufferSRGB(true, false);
    }

    //! Make this FBO as reading source in context.
    virtual void BindReadBuffer(const Handle(OpenGl_Context)& theGlCtx) override
    {
        OpenGl_FrameBuffer::BindReadBuffer(theGlCtx);
    }
};

#endif // _ViewportFrameBuffer_Header_File_
