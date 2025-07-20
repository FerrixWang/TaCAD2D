// Copyright [2025] TaCAD

#ifndef IACT_VIEWPORT_VIEWPORTPANEL_H_
#define IACT_VIEWPORT_VIEWPORTPANEL_H_

// Qt includes
#include <QFrame>
#include <QVBoxLayout>
#include <QWidget>

// Project includes
#include "Iact/HudElements/HudManager.hxx"
#include "Iact/InteractiveContext.hxx"
#include "Iact/Viewport/ViewportWidget.hxx"
#include "Iact/ViewportController.hxx"
#include "Iact/WorkspaceController.hxx"

/// @brief ViewportPanel class
class ViewportPanel : public QWidget
{
	Q_OBJECT

public:
	explicit ViewportPanel(QWidget* parent = nullptr);

	Handle(ViewportController) viewportController() const
	{
		return myViewportController;
	}

private:
	void initHudContainer()
	{
		myHudContainer = new QFrame(this);
		myHudContainer->setFrameShape(QFrame::NoFrame);
		myHudContainer->setMouseTracking(true);
		myHudContainer->setVisible(false);
		myHudContainer->setLayout(new QVBoxLayout);
		myHudContainer->setAutoFillBackground(false);
		myHudContainer->setStyleSheet("background-color: rgba(128, 128, 128, 0.5);");
	}

	void setupConnection()
	{
		InteractiveContext::Current()->viewportControllerChanged.connect(
			std::bind(&ViewportPanel::onViewportControllerChanged, this, std::placeholders::_1));
		InteractiveContext::Current()->workspaceControllerChanged.connect(
			std::bind(&ViewportPanel::onWorkspaceControllerChanged, this, std::placeholders::_1));
	}

	void onWorkspaceControllerChanged(const Handle(WorkspaceController)& WCer)
	{
		if(myHudContainer != nullptr)
		{
			auto aHudManager = WCer->GetHudManager();
			aHudManager->SetHudContainer(myHudContainer);
		}
	}

	void onViewportControllerChanged(const Handle(ViewportController)& theCV)
	{
		if(myViewportController == theCV)
		{
			return;
		}
		myViewportController = theCV;

		if(myViewportHwndHost != nullptr)
		{
			layout()->removeWidget(myViewportHwndHost);
			myViewportHwndHost->deleteLater();
		}
		theCV->GetViewport()->SetDpiScale(1.0f);
		myViewportHwndHost = new ViewportWidget(theCV);

		layout()->addWidget(myViewportHwndHost);
		myViewportHwndHost->setFocus();
	}
protected:
	virtual void mouseMoveEvent(QMouseEvent* event) override;

void updateHud(const QPointF& pos) {
        if (!myHudContainer) return; // 防止空指针

        // 获取 HUD 容器的尺寸
        int hudWidth = myHudContainer->width();
        int hudHeight = myHudContainer->height();

        // 获取父窗口的尺寸（假设有一个父窗口）
        QWidget* parent = myHudContainer->parentWidget();
        int parentWidth = parent ? parent->width() : 1920; // 默认值，需根据实际调整
        int parentHeight = parent ? parent->height() : 1080;

        // 计算位置，参考 C# 逻辑
        int x = static_cast<int>(pos.x());
        int y = static_cast<int>(pos.y());

        // 调整 x：如果右侧有足够空间，放在 pos.x，否则移到左侧
        if ((parentWidth - hudWidth - x) > 0) {
            x += 10; // 右侧偏移（参考你的原始代码）
        } else {
            x -= hudWidth + 10; // 移到左侧并留出间距
        }

        // 调整 y：如果上方有足够空间，放在 pos.y 上方，否则下方
        if ((y - hudHeight) > 0) {
            y -= hudHeight + 10; // 上方偏移（参考你的原始代码）
        } else {
            y += 10; // 移到下方并留出间距
        }

        // 确保不超出边界
        x = std::max(0, std::min(x, parentWidth - hudWidth));
        y = std::max(0, std::min(y, parentHeight - hudHeight));

        // 更新 HUD 位置
        myHudContainer->move(x, y);
		myHudContainer->raise();
    }

public:
	Handle(ViewportController) myViewportController = nullptr;
	ViewportWidget* myViewportHwndHost = nullptr;
	QFrame* myHudContainer = nullptr;
};


#endif  // IACT_VIEWPORT_VIEWPORTPANEL_H_
