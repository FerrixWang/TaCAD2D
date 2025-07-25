// class HintCircle

#ifndef _Iact_Visual_HintCircle_cxx_
#define _Iact_Visual_HintCircle_cxx_

#include <AIS_Circle.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom_Circle.hxx>
#include <GeomAPI.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pln.hxx>

#include "Iact/Visual/Hint.hxx"

class HintCircle : public Hint
{
public:
    HintCircle(const HintStyles& styles)
        : Hint()
        , _AisCircle(nullptr)
        , _Circle(nullptr)
        , _StartParam(0.0)
        , _EndParam(2.0 * M_PI)
        , _Styles(styles)
    {}

public:
    void Update() override
    {
        bool isValid = (!_Circle.IsNull()) && (_Circle->Radius() > Precision::Confusion());
        if(!isValid)
        {
            Remove();
            return;
        }

        if(_AisCircle.IsNull())
        {
            if(!_EnsureAisObject())
                return;
        }
        else
        {
            _AisCircle->SetCircle(_Circle);
            _AisCircle->SetFirstParam(_StartParam);
            _AisCircle->SetLastParam(_EndParam);
            AisContext()->RecomputePrsOnly(_AisCircle, false);
        }
        _UpdatePresentation();
    }

    void Remove() override
    {
        if(!_AisCircle.IsNull())
        {
            AisContext()->Remove(_AisCircle, false);
            _AisCircle.Nullify();
        }
    }

    Handle<AIS_InteractiveObject> AisObject() const override
    {
        return _AisCircle;
    }

    void Set(const gp_Circ& circ)
    {
        _Circle = new Geom_Circle(circ);
        Update();
    }

    void SetRange(const double startAngleDeg, const double endAngleDeg)
    {
        _StartParam = startAngleDeg * (M_PI / 180.0);
        _EndParam = endAngleDeg * (M_PI / 180.0);
        Update();
    }

    void Set(const gp_Circ2d& circ2d, const gp_Pln& plane)
    {
        Handle<Geom2d_Circle> geom2dCircle = new Geom2d_Circle(circ2d);
        _Circle = Handle<Geom_Circle>::DownCast(GeomAPI::To3d(geom2dCircle, plane));
        Update();
    }

    void SetColor(const Quantity_Color& color)
    {
        _Color = color;
        if(!_AisCircle.IsNull())
        {
            Update();
        }
    }

    Quantity_Color Color() const
    {
        return _Color;
    }

private:
    bool _EnsureAisObject()
    {
        if(!_AisCircle.IsNull())
        {
            return true;
        }

        if(_Circle.IsNull())
            return false;

        if(_Circle->Radius() <= Precision::Confusion())
            return false;

        _AisCircle = new AIS_Circle(_Circle);
        SetAspects(_AisCircle, _Styles);

        AisContext()->Display(_AisCircle, false);
        AisContext()->Deactivate(_AisCircle);
        return true;
    }

    void _UpdatePresentation()
    {
        _AisCircle->SetColor(_Color);
    }

private:
    Handle<AIS_Circle> _AisCircle;
    Handle<Geom_Circle> _Circle;
    double _StartParam;
    double _EndParam;
    HintStyles _Styles;
    Quantity_Color _Color;
};

DEFINE_STANDARD_HANDLE(HintCircle, Hint)

#endif // _Iact_Visual_HintCircle_cxx_
