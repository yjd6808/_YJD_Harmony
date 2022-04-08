/*
 * �ۼ��� : ������
 */


#pragma once

#include <TF/SourceHeader.h>

#include <JCore/Container/ArrayQueue.h>

class ColoredListView : public LayerColor
{
public:
	ColoredListView() = default;
	virtual ~ColoredListView() = default;
	static ColoredListView* create(const Color4B& color);
	bool init() override;

	void SetContantSize(const Size& size);

	ListView* GetListView() const { return m_pListView; }
	void OnScrollViewEvent(Ref* pref, ScrollView::EventType type);
private:
	ListView* m_pListView;
};


