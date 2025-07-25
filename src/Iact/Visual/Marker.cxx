#include "Iact/Visual/Marker.hxx"

#include <AIS_Point.hxx> 
#include <ElSLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Point.hxx>
#include <Image_PixMap.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Quantity_Color.hxx>

#include "Iact/Visual/VisualObject.hxx"
#include "Occt/OcctExtensions/AIS_PointEx.hxx"
#include "Occt/OcctHelper/AisHelper.hxx"
#include "Occt/OcctHelper/PixMapHelper.hxx"

void Marker::Update()
{
    // 确保 AIS_Point 对象存在
    if(_AisPoint.IsNull())
    {
        ensureAisObject();
    }
    else
    {
        _UpdatePresentation();
        // 在 AisContext 上进行重新显示
        AisContext()->Redisplay(_AisPoint, true);
    }

    if(m_isSelectable)
    {
        AisContext()->Activate(_AisPoint);
    }
    else
    {
        AisContext()->Deactivate(_AisPoint);
    }
}

void Marker::Remove()
{
    if(_AisPoint.IsNull())
        return;

    AisContext()->Erase(_AisPoint, true); // Erase the point from the context
    _AisPoint.Nullify();
}

void Marker::Set(const gp_Pnt& p)
{
    Set(new Geom_CartesianPoint(p));
}

void Marker::Set(const Handle(Geom_CartesianPoint)& p)
{
    m_p = p;

    if(!ensureAisObject())
        return;

    _AisPoint->SetComponent(m_p);
    Update();
}

void Marker::Set(const gp_Pnt2d& p, const gp_Pln& plane)
{
    gp_Pnt pnt;
    ElSLib::D0(p.X(), p.Y(), plane, pnt);
    Set(pnt);
}

void Marker::SetImage(MarkerImage image)
{
    if(m_image.PixMap == image.PixMap)
        return;

    m_image = image;

    Remove();
    ensureAisObject();
}

Handle(Prs3d_PointAspect) Marker::CreateBitmapPointAspect(const MarkerImage& image, const Quantity_Color& color)
{
    if(image.Bytes->IsEmpty())
    {
        return new Prs3d_PointAspect(Aspect_TypeOfMarker::Aspect_TOM_BALL, color, 1.0);
    }

    return new Prs3d_PointAspect(color, image.Width, image.Height, image.Bytes);
}

Handle(Prs3d_PointAspect) Marker::CreateImagePointAspect(const MarkerImage& image)
{
    if(image.PixMap->IsEmpty())
    {
        return new Prs3d_PointAspect(Aspect_TypeOfMarker::Aspect_TOM_BALL, Quantity_Color(Quantity_NOC_YELLOW), 1.0);
    }

    auto aspectMarker = MakeHandle<Graphic3d_AspectMarker3d>(image.PixMap);
    auto aspectPoint = MakeHandle<Prs3d_PointAspect>(aspectMarker);

    return aspectPoint;
}

bool Marker::ensureAisObject()
{

    if(!_AisPoint.IsNull())
        return true;

    if(m_p.IsNull())
        return false;

    _AisPoint = new AIS_PointEx(m_p);

    if(m_styles == MarkerStyle::NoClipPlane)
        AisHelper::DisableGlobalClipPlanes(_AisPoint);

    _UpdatePresentation();
    if(m_styles == MarkerStyle::Topmost)
    {
        _AisPoint->SetZLayer(-4); // TopOSD
        _AisPoint->HilightAttributes()->SetZLayer(-4);
        _AisPoint->DynamicHilightAttributes()->SetZLayer(-4);
    }

    AisContext()->Display(_AisPoint, 0, 0, false);
    AisContext()->SetSelectionSensitivity(_AisPoint, 0, std::min(m_image.Height, m_image.Width));

    return true;
}

void Marker::_UpdatePresentation()
{
    if(_AisPoint.IsNull()) return;

    _AisPoint->SetMarker(Aspect_TOM_USERDEFINED);

    if(_PointAspect.IsNull())
    {
        if(m_styles & MarkerStyle::Bitmap) {
            _PointAspect = CreateBitmapPointAspect(m_image, m_color);
		}
		else if(m_styles & MarkerStyle::Image)
		{
			_PointAspect = CreateImagePointAspect(m_image);
        }
    }

    if(!_PointAspect.IsNull())
    {
        _AisPoint->Attributes()->SetPointAspect(_PointAspect);
        _AisPoint->HilightAttributes()->SetPointAspect(_PointAspect);
        _AisPoint->HilightAttributes()->SetColor(Color::Highlight);
        _AisPoint->DynamicHilightAttributes()->SetPointAspect(_PointAspect);
        _AisPoint->DynamicHilightAttributes()->SetColor(Color::Highlight);
    }

    if(m_styles & MarkerStyle::Background)
    {
        _AisPoint->EnableBackground(0.75);
        _AisPoint->SetColor(m_colorBg);
    }
}

Marker::MarkerImage Marker::_GetMarkerImage(const std::string& name, int size)
{
    auto pixmap = _TryGetMarkerAsImage(name, size);

    MarkerImage image;
    image.PixMap = pixmap;
    image.Width = pixmap->Width();
    image.Height = (int)pixmap->Height();
    image.Bytes = (new Graphic3d_MarkerImage(pixmap))->GetBitMapArray();

    return image;
}

Handle(Image_PixMap) Marker::_TryGetMarkerAsImage(const std::string& name, int size)
{
    int dpiScale = 1;
    int finalSize = static_cast<int>(size * dpiScale);

    QString imagePath = ResourceUtils::markerPath(QString::fromStdString(name));

    QImage image(imagePath);
    if(!image.load(imagePath, "svg"))
    {
        qDebug() << "Error: Failed to load image:" << imagePath;
        return nullptr;
    }

    Handle(Image_PixMap) pixmap = PixMapHelper::convertFromBitmap(image.scaled(finalSize, finalSize));
    if(pixmap.IsNull())
    {
        qDebug() << "Could not load marker image " << name << " into pixmap.";
        return nullptr;
    }
    return pixmap;
}