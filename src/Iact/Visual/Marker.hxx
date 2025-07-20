//! class Marker
//! 
#ifndef _Marker_Header_File_
#define _Marker_Header_File_

#include <AIS_Point.hxx> 
#include <ElSLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Point.hxx>
#include <Image_PixMap.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Quantity_Color.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_Label.hxx>
#include <XCAFDoc_Color.hxx>

#include "Iact/Visual/VisualObject.hxx"
#include "Occt/OcctExtensions/AIS_PointEx.hxx"
#include "Occt/OcctHelper/AisHelper.hxx"
#include "Occt/OcctHelper/PixMapHelper.hxx"

enum class MarkerStyle
{
    Bitmap = 1,
    Image = 2,
    ModeMask = 0x000f,

    NoClipPlane = 1 << 15,
    Topmost = 1 << 16,
    Background = 1 << 18,
};
DEFINE_ENUM_FLAGS(MarkerStyles, MarkerStyle)

class Marker : public VisualObject
{
public:
    struct MarkerImage
    {
        Handle(Image_PixMap) PixMap;
        int Width;
        int Height;
        Handle(TColStd_HArray1OfByte) Bytes;
    };

public:
    Marker(const MarkerStyles& styles, const MarkerImage& image)
        : VisualObject()
        , m_styles(styles)
        , m_image(image)
        , m_color(Quantity_NOC_YELLOW)
        , m_colorBg(Quantity_NOC_LIGHTGRAY)
        , m_isSelectable(false)
        , m_p(nullptr)
    {}

    Marker(const MarkerStyles& styles, const std::string& imageName, int size)
        : VisualObject()
        , m_styles(styles)
        , m_image(_GetMarkerImage(imageName, size))
        , m_color(Quantity_NOC_YELLOW)
        , m_colorBg(Quantity_NOC_LIGHTGRAY)
        , m_isSelectable(false)
        , m_p(nullptr)
    {}

    virtual void Update() override;
	virtual void Remove() override;

    void Set(const gp_Pnt& p);

    void Set(const Handle(Geom_CartesianPoint)& p);

    void Set(const gp_Pnt2d& p, const gp_Pln& plane);

    void SetImage(MarkerImage image);

    void SetColor(const Quantity_Color& color)
    {
        m_color = color;
        Update();
    }

    // Set isSelectable

    virtual bool IsSelectable()const { return m_isSelectable; }
    virtual void SetSelectable(bool value) { m_isSelectable = value; }

    static Handle(Prs3d_PointAspect) CreateBitmapPointAspect(
        const MarkerImage& image, const Quantity_Color& color);

    static Handle(Prs3d_PointAspect) CreateImagePointAspect(
        const MarkerImage& image);

public:
    static MarkerImage& BallImage()
    {
        static MarkerImage a = _GetMarkerImage("Ball", 8);
        return a;
    }

    static MarkerImage& RectImage()
    {
        static MarkerImage a = _GetMarkerImage("Rect", 8);
        return a;
    }

    static MarkerImage& RingImage()
    {
        static MarkerImage a = _GetMarkerImage("Ring", 16);
        return a;
    }

    static MarkerImage& PlusImage()
    {
        static MarkerImage a = _GetMarkerImage("Plus", 16);
        return a;
    }

    static MarkerImage& XImage()
    {
        static MarkerImage a = _GetMarkerImage("X", 16);
        return a;
    }

    static MarkerImage& ErrorImage()
    {
        static MarkerImage a = _GetMarkerImage("Error", 32);
        return a;
    }

private:
    // 确保 _AisPoint 被正确初始化
    bool ensureAisObject();

    void _UpdatePresentation();

    // Static method to load marker images
    static MarkerImage _GetMarkerImage(const std::string& name, int size);

    static Handle(Image_PixMap) _TryGetMarkerAsImage(const std::string& name, int size);

private:
    MarkerStyles m_styles;
    MarkerImage m_image;
    Handle(AIS_PointEx) _AisPoint = nullptr; // OCCT 点对象
    Handle(Geom_CartesianPoint) m_p; // OCCT 坐标点
    Handle(Prs3d_PointAspect) _PointAspect = nullptr;
    Quantity_Color m_color; // 标记颜色
    Quantity_Color m_colorBg; // 背景颜色
    bool m_isSelectable;
};

DEFINE_STANDARD_HANDLE(Marker, VisualObject)

#endif  // _Marker_Header_File_
