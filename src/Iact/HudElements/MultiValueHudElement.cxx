// Copyright [2025] TaCAD

// Own include
#include "Iact/HudElements/MultiValueHudElement.hxx"

// Qt includes
#include <QDoubleValidator>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

// Project includes
#include "Iact/HudElements/HudElement.hxx"

MultiValueHudElement::MultiValueHudElement(const QString& text1, const QString& text2, QWidget* parent)
    : HudElement(parent)
    , m_label1(new QLabel(text1, this))
    , m_label2(new QLabel(text2, this))
    , m_edit1(new QLineEdit("0.0", this))
    , m_edit2(new QLineEdit("0.0", this))
    , m_isInKeyboardMode1(false)
    , m_isInKeyboardMode2(false)
    , m_firstFromKeyboard(true) 
{
    // Set up the layout
    auto* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    setLayout(layout);

    // Default styles
    m_label1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_label1->setStyleSheet("color: white; font-size: 12px; background: none;");

    m_label2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_label2->setStyleSheet(m_label1->styleSheet());

    // Set up label and line edits
    m_edit1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_edit1->setValidator(new QDoubleValidator(this));

    m_edit2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_edit2->setValidator(new QDoubleValidator(this));

    layout->addWidget(m_label1, 0, 0);
    layout->addWidget(m_edit1, 0, 1);
    layout->addWidget(m_label2, 1, 0);
    layout->addWidget(m_edit2, 1, 1);

    // Connect signals
    connect(m_edit1, &QLineEdit::editingFinished, this, &MultiValueHudElement::_onEdit1Finished);
    connect(m_edit2, &QLineEdit::editingFinished, this, &MultiValueHudElement::_onEdit2Finished);
}

void MultiValueHudElement::SetLabel(const QString& text1, const QString& text2) 
{
    SetLabel1(text1);
    SetLabel2(text2);
}

void MultiValueHudElement::SetLabel1(const QString& text) 
{
    m_label1->setText(text);
}

void MultiValueHudElement::SetLabel2(const QString& text) 
{
    m_label2->setText(text);
}

void MultiValueHudElement::SetValues(double value1, double value2) 
{
    SetValue1(value1);
    SetValue2(value2);
}

void MultiValueHudElement::SetValue1(double value) 
{
    m_edit1->setText(QString::number(value, 'f', 3));
}

void MultiValueHudElement::SetValue2(double value) 
{
    m_edit2->setText(QString::number(value, 'f', 3));
}

void MultiValueHudElement::keyPressEvent(QKeyEvent* event) 
{
    if (m_isInKeyboardMode2) {
        simulateKeyPress(m_edit2, event);
        return;
    }

    simulateKeyPress(m_edit1, event);
    if (event->isAccepted()) {
        m_isInKeyboardMode1 = true;
        m_isInKeyboardMode2 = false;
    }
}

void MultiValueHudElement::_onEdit1Finished() 
{
    m_isInKeyboardMode1 = false;
    m_isInKeyboardMode2 = true;
    m_firstFromKeyboard = true;
}

void MultiValueHudElement::_onEdit2Finished() 
{
    m_isInKeyboardMode1 = false;
    m_isInKeyboardMode2 = false;
    m_firstFromKeyboard = true;
    emit MultiValueEntered(m_edit1->text().toDouble(), m_edit2->text().toDouble());
}

void MultiValueHudElement::simulateKeyPress(QLineEdit* edit, QKeyEvent* event) 
{
    if (event->key() == Qt::Key_Backspace) {
        edit->backspace();
        event->accept();
    }
    else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        edit->editingFinished();
        event->accept();
    }
    else {
        edit->insert(event->text());
        event->accept();
    }
}
