// Copyright [2025] TaCAD

#ifndef IACT_HUD_ELEMENTS_VALUEHUDELEMENT_H_
#define IACT_HUD_ELEMENTS_VALUEHUDELEMENT_H_

// Qt includes
#include <QDoubleValidator>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

// Project includes
#include "Iact/HudElements/HudElement.hxx"

class ValueHudElement : public HudElement 
{
    Q_OBJECT

public:
    explicit ValueHudElement(const QString& label = "", QWidget* parent = nullptr);

    void SetLabel(const std::string& text);
    void SetValue(double value);
    double Value() const;

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void valueEntered(double value);

private:
    void onEditFinished();
    void simulateKeyPress(QLineEdit* edit, QKeyEvent* event);

private:
    QLabel* m_label;
    QLineEdit* m_edit;
    bool m_isInKeyboardMode;
};

#endif  // IACT_HUD_ELEMENTS_VALUEHUDELEMENT_H_
