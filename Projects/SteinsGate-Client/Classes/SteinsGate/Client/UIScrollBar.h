/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/UIElement.h>

class UIScrollBar : public UIElement
{
	static constexpr int TextureCount = 7;
	static constexpr int IndexUpNormal = 0;
	static constexpr int IndexUpPressed = 1;
	static constexpr int IndexDownNormal = 2;
	static constexpr int IndexDownPressed = 3;
	static constexpr int IndexThumbNormal = 4;
	static constexpr int IndexThumbPressed = 5;
	static constexpr int IndexTrack = 6;

	static constexpr float MinThumbHeight = 8;
public:
	static UIScrollBar* create(UIMasterGroup* master, UIGroup* parent);
	static UIScrollBar* create(UIMasterGroup* master, UIGroup* parent, UIScrollBarInfo* scrollBarInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::ScrollBar; }

	UIScrollBar(UIMasterGroup* master, UIGroup* parent);
	UIScrollBar(UIMasterGroup* master, UIGroup* parent, UIScrollBarInfo* scrollBarInfo, bool infoOwner);
	~UIScrollBar() override;

	bool init() override;
	void createSprites();

	void updateTrackAndButtonPosition();
	void load() override;
	void unload() override;

	void updateThumbSize();
	void updateThumbPosition();

	void setRowPos(int pos);
	void setRowCount(int count);
	void setRowCountPerPage(int count);
	void setUISize(const SGSize& contentSize) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoScrollBar(UIScrollBarInfo* info, bool infoOwner);
	int getRowPos() const { return m_iPos; }
	int getEndRowPos() const { return m_iEndPos; }

	void setVisibleStateNormal();
	void setEnabled(bool enabled) override;
	void setLinkElement(UIElement* element);

	bool isUpButtonContainPoint(SGVec2 pos);
	bool isDownButtonContainPoint(SGVec2 pos);
	bool isThumbButtonContainPoint(SGVec2 pos);

	void restoreState(State state) override;

	UIScrollBarInfo* getInfo() const { return m_pInfo; }
	UIElementType_t getElementType() override { return UIElementType::ScrollBar; }
	SGString toString() override { return SGStringUtil::Format("스크롤바(%d)", m_pInfo->Code); }
protected:
	void onMouseLeaveInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownInternalDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseUpContainedInternalDetail(SGEventMouse* mouseEvent) override;
	bool onMouseScrollInternalDetail(SGEventMouse* mouseEvent) override;

	void onLinkElementMouseScroll(SGEventMouse* mouseEvent);

	void setInitialUISize(SGSize size) override;
private:
	int m_iRowCount;
	int m_iRowCountPerPage;
	int m_iPos;
	int m_iEndPos;

	float m_fThumbHeight;
	float m_fUpButtonHeight;
	float m_fDownButtonHeight;
	float m_fSplitedTrackHeight;
	float m_fTrackHeight;

	bool m_bDragBegin;
	float m_fDragStartYPos;
	int m_iDragStartPos;

	UIElement* m_pLink;
	UIScrollBarInfo* m_pInfo;
	FrameTexture* m_pTexture[TextureCount];
	SGSprite* m_pSprite[TextureCount];
};




