// Copyright [2025] TaCAD

#ifndef IACT_HUD_ELEMENTS_COORD2DHUDELEMENT_H_
#define IACT_HUD_ELEMENTS_COORD2DHUDELEMENT_H_

#include <QLabel>
#include <QWidget>    

#include "Iact/HudElements/HudElement.hxx"

class Coord2DHudElement : public HudElement 
{
    Q_OBJECT
    Q_PROPERTY(double coordinateX READ CoordinateX)
    Q_PROPERTY(double coordinateY READ CoordinateY)

public:
    explicit Coord2DHudElement(QWidget* parent = nullptr);

    double CoordinateX() const { return _CoordinateX; }
    double CoordinateY() const { return _CoordinateY; }

    void setValues(double coordX, double coordY);

private:
    QLabel* _LabelX;
    QLabel* _LabelY;
    double _CoordinateX;
    double _CoordinateY;
};

#endif  // IACT_HUD_ELEMENTS_COORD2DHUDELEMENT_H_
