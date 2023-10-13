/*
 * 작성자 : 윤정도
 */


#pragma once

#include <cocos2d.h>
#include <cocos/ui/CocosGUI.h>

class ColoredListView : public cocos2d::LayerColor
{
public:
	ColoredListView() = default;
	virtual ~ColoredListView() = default;
	static ColoredListView* create(const cocos2d::Color4B& color);
	bool init() override;

	void SetContantSize(const cocos2d::Size& size);

	cocos2d::ui::ListView* GetListView() const { return m_pListView; }
	void OnScrollViewEvent(Ref* pref, cocos2d::ui::ScrollView::EventType type);
private:
	cocos2d::ui::ListView* m_pListView;
};


