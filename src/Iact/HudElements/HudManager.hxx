//! @file HudManager.hxx
//! @brief 管理 HUD 元素的容器与布局

#ifndef IACT_HUDMANAGER_HXX_
#define IACT_HUDMANAGER_HXX_

#include <algorithm>
#include <vector>

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

#include <Standard_DefineHandle.hxx>
#include <Standard_Transient.hxx>

#include "Iact/HudElements/HudElement.hxx"

class HudManager : public Standard_Transient
{
public:
    HudManager() = default;

    //! 初始化 HUD 容器（外部应手动设置 layout 和样式）
    void SetHudContainer(QWidget* container)
    {
        if(!container)
            return;

        m_hudContainer = container;
    }

	void SetHintMessager(QLabel* theMessager)
	{
		if(!theMessager)
			return;

        m_hintMessageLabel = theMessager;
	}

    //! 添加 HUD 元素（避免重复）
    void AddElement(HudElement* theElement)
    {
        if(!m_hudContainer || !theElement)
            return;

        if(std::find(m_hudElements.begin(), m_hudElements.end(), theElement) != m_hudElements.end())
            return;

        if(auto layout = layout_())
        {
            layout->addWidget(theElement);
            theElement->show();

            m_hudElements.push_back(theElement);
            m_hudContainer->show();
        }
    }

    //! 移除 HUD 元素
    void RemoveElement(HudElement* theElement)
    {
        if(!m_hudContainer || !theElement)
            return;

        auto it = std::find(m_hudElements.begin(), m_hudElements.end(), theElement);
        if(it == m_hudElements.end())
            return;

        if(auto layout = layout_())
        {
            layout->removeWidget(theElement);
            theElement->hide();
            theElement->setParent(nullptr);

            m_hudElements.erase(it);

            if(m_hudElements.empty())
                m_hudContainer->hide();
        }
    }

    //! 清空所有 HUD 元素
    void ClearElements()
    {
        if(!m_hudContainer)
            return;

        if(auto layout = layout_())
        {
            for(HudElement* element : m_hudElements)
            {
                layout->removeWidget(element);
                element->hide();
                element->setParent(nullptr);
            }
            m_hudElements.clear();
            m_hudContainer->hide();
        }
    }

    void SetHintMessage(const std::any& owner, const std::string& message)
    {
        if(!myHintMessage.empty() && owner.has_value())
        {
            // 提取指针地址并比较
            try
            {
                void* ownerPtr = std::any_cast<void*>(owner);
                void* currentOwnerPtr = myHintMessageOwner.has_value() ? std::any_cast<void*>(myHintMessageOwner) : nullptr;
                if(ownerPtr != currentOwnerPtr)
                {
                    return; // 指针地址不匹配，不更新
                }
            }
            catch(const std::bad_any_cast&)
            {
                return; // 类型不匹配，不更新
            }
        }
        myHintMessage = message;
        myHintMessageOwner = message.empty() ? std::any() : owner;

		if(m_hintMessageLabel)
		{
			m_hintMessageLabel->setText(QString::fromStdString(myHintMessage));
			m_hintMessageLabel->show();
		}
    }

    //! 返回当前已添加的元素
    const std::vector<HudElement*>& Elements() const { return m_hudElements; }

private:
    QVBoxLayout* layout_() const
    {
        return m_hudContainer ? qobject_cast<QVBoxLayout*>(m_hudContainer->layout()) : nullptr;
    }

private:
    std::vector<HudElement*> m_hudElements;          //!< 已添加的 HUD 元素列表
	QLabel* m_hintMessageLabel = nullptr;           //!< 提示信息标签
    QWidget* m_hudContainer = nullptr;                //!< 外部传入的 HUD 容器
    std::any myHintMessageOwner;
	std::string myHintMessage;
};

// 定义句柄类型
DEFINE_STANDARD_HANDLE(HudManager, Standard_Transient)

#endif // IACT_HUDMANAGER_HXX_
