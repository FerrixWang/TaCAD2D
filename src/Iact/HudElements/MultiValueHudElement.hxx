// Copyright [2025] TaCAD

#ifndef IACT_HUD_ELEMENTS_MULTIVALUEHUDELEMENT_H_
#define IACT_HUD_ELEMENTS_MULTIVALUEHUDELEMENT_H_

// Qt includes
#include <QDoubleValidator>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

// Project includes
#include "Iact/HudElements/HudElement.hxx"

class MultiValueHudElement : public HudElement
{
    Q_OBJECT

public:
    explicit MultiValueHudElement(const QString& text1 = "", const QString& text2 = "", QWidget* parent = nullptr);

    void SetLabel(const QString& text1, const QString& text2);
    void SetLabel1(const QString& text);
    void SetLabel2(const QString& text);
    void SetValues(double value1, double value2);
    void SetValue1(double value);
    void SetValue2(double value);

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    void MultiValueEntered(double value1, double value2);

private:
    void _onEdit1Finished();
    void _onEdit2Finished();
    void simulateKeyPress(QLineEdit* edit, QKeyEvent* event);

private:
    QLabel* m_label1;
    QLabel* m_label2;
    QLineEdit* m_edit1;
    QLineEdit* m_edit2;
    bool m_isInKeyboardMode1;
    bool m_isInKeyboardMode2;
    bool m_firstFromKeyboard;
};

#endif  // IACT_HUD_ELEMENTS_MULTIVALUEHUDELEMENT_H_
