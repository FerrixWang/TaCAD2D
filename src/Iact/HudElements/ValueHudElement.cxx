// Copyright [2025] TaCAD

// Own include
#include "Iact/HudElements/ValueHudElement.hxx"

// Qt includes
#include <QDoubleValidator>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QWidget>

#include "Iact/HudElements/HudElement.hxx"

ValueHudElement::ValueHudElement(const QString& label, QWidget* parent)
    : HudElement(parent)
    , m_label(new QLabel(label, this))
    , m_edit(new QLineEdit("0.0", this))
    , m_isInKeyboardMode(false) 
{
    // 设置布局
    auto* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    // 默认样式
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_label->setStyleSheet("color: white; font-size: 12px; background: none;");

    m_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_edit->setValidator(new QDoubleValidator(this));

    layout->addWidget(m_label, 0, 0);
    layout->addWidget(m_edit, 0, 1);

    // 连接信号
    connect(m_edit, &QLineEdit::editingFinished, this, &ValueHudElement::onEditFinished);
}

void ValueHudElement::SetLabel(const std::string& text) 
{
    m_label->setText(QString::fromStdString(text));
}

void ValueHudElement::SetValue(double value) 
{
    m_edit->setText(QString::number(value, 'f', 3));
}

double ValueHudElement::Value() const 
{
    return m_edit->text().toDouble();
}

void ValueHudElement::keyPressEvent(QKeyEvent* event) 
{
    simulateKeyPress(m_edit, event);
    if (event->isAccepted()) {
        m_isInKeyboardMode = true;
    }
}

void ValueHudElement::onEditFinished()
{
    m_isInKeyboardMode = false;
    emit valueEntered(m_edit->text().toDouble());
}

void ValueHudElement::simulateKeyPress(QLineEdit* edit, QKeyEvent* event) 
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
