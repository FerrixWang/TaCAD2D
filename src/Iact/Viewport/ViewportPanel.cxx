// Copyright [2025] TaCAD

// Own Libraries
#include "Iact/Viewport/ViewportPanel.hxx"

// Qt includes
#include <QWidget>
#include <QVBoxLayout>
#include <QMouseEvent>

ViewportPanel::ViewportPanel(QWidget* parent)
	: QWidget(parent)
	, myHudContainer(nullptr)
{
	// Initialize layout for the panel
	setLayout(new QVBoxLayout(this));
	setMouseTracking(true);

	initHudContainer();

	setupConnection();
}

void ViewportPanel::mouseMoveEvent(QMouseEvent* event)
{
	QWidget::mouseMoveEvent(event);

	auto m_mouseMovePosition = this->mapFromGlobal(event->globalPos());

	if(myHudContainer != nullptr)
	{
		myHudContainer->adjustSize();
		myHudContainer->update();
		updateHud(m_mouseMovePosition);
	}
}
