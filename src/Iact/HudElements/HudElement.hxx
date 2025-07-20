// Copyright [2025] TaCAD

#ifndef IACT_HUD_ELEMENTS_HUDELEMENT_H_
#define IACT_HUD_ELEMENTS_HUDELEMENT_H_

#include <QWidget>

class HudElement : public QWidget
{
	Q_OBJECT

public:
	explicit HudElement(QWidget* parent = nullptr)
		: QWidget(parent)
	{}

public:
	virtual void initialize() {};
};

#endif  // IACT_HUD_ELEMENTS_HUDELEMENT_H_
