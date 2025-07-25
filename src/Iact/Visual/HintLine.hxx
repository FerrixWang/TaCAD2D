// class HintLine

#ifndef _Iact_Visual_HintLine_cxx_
#define _Iact_Visual_HintLine_cxx_

#include <AIS_Line.hxx>
#include <ElSLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_Point.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include "Iact/Visual/Hint.hxx"

class HintLine : public Hint
{
public:
	HintLine(const HintStyles& styles) 
		: Hint()
		, _AisLine(nullptr)
		, _P1(nullptr)
		, _P2(nullptr)
		, _GeomLine(nullptr)
		, _Styles(styles)
	{}

public:
	void Update() override
	{
		bool isValid = true;
		if(_GeomLine.IsNull())
		{
			isValid = _P1->Distance(_P2) > Precision::Confusion();
		}

		if(!isValid)
		{
			Remove();
			return;
		}

		if(_AisLine.IsNull())
		{
			if(!_EnsureAisObject())
				return;
		}
		else
		{
			if(!_GeomLine.IsNull())
				_AisLine->SetLine(_GeomLine);
			else
				_AisLine->SetPoints(_P1, _P2);
			AisContext()->RecomputePrsOnly(_AisLine, false);
		}
		_UpdatePresentation();
	}

	void Remove() override
	{
		if(!_AisLine.IsNull())
		{
			AisContext()->Remove(_AisLine, false);
			_AisLine.Nullify();
	}
	}

	virtual Handle<AIS_InteractiveObject> AisObject() const override
	{
		return _AisLine;
	}

	void Set(const gp_Pnt& p1, const gp_Pnt& p2)
	{
		_P1 = new Geom_CartesianPoint(p1);
		_P2 = new Geom_CartesianPoint(p2);
		_GeomLine.Nullify();

		Update();
	}

	void Set(const gp_Pnt2d& p1, const gp_Pnt2d& p2, const gp_Pln& plane)
	{
		gp_Pnt pnt1, pnt2;
		ElSLib::D0(p1.X(), p1.Y(), plane, pnt1);
		ElSLib::D0(p2.X(), p2.Y(), plane, pnt2);

		Set(pnt1, pnt2);
	}

	void Set(const gp_Ax1& axis)
	{
		_GeomLine = new Geom_Line(axis);
		_P1.Nullify(); 
		_P2.Nullify();

		Update();
	}

	void Set(const gp_Ax2d& axis, const gp_Pln& plane)
	{
		Set(axis.Location(), axis.Location().Translated(axis.Direction()), plane);
	}

	// Get Color and set Color
	void SetColor(const Quantity_Color& color)
	{
		_Color = color;
		if(!_AisLine.IsNull())
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
		if(!_AisLine.IsNull())
		{
			return true;
		}

		if((_P1.IsNull() || _P2.IsNull())
		   && _GeomLine.IsNull())
			return false;

		_AisLine = (_GeomLine.IsNull())
			? new AIS_Line(_P1, _P2)
			: new AIS_Line(_GeomLine);

		SetAspects(_AisLine, _Styles);

		AisContext()->Display(_AisLine, false);
		AisContext()->Deactivate(_AisLine);
		return true;
	}

	//--------------------------------------------------------------------------------------------------

	void _UpdatePresentation()
	{
		_AisLine->SetColor(_Color);
	}

private:
	Handle<AIS_Line> _AisLine;
	Handle<Geom_Point> _P1;
	Handle<Geom_Point> _P2;
	Handle<Geom_Line> _GeomLine;
	HintStyles _Styles;
	Quantity_Color _Color;
};

DEFINE_STANDARD_HANDLE(HintLine, Hint)

#endif // _Iact_Visual_HintLine_cxx_
