/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUIScrollBar : public SGUIElement
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
	SGUIScrollBar(SGUIMasterGroup* master, SGUIGroup* parent, SGUIScrollBarInfo* scrollBarInfo);
	~SGUIScrollBar() override;

	bool init() override;
	void createSprites();
	
	void load() override;
	void unload() override;

	void updateThumbSize();
	void updateThumbPosition();

	void setRowPos(int pos);
	void setRowCount(int count);
	void setRowCountPerPage(int count);
	int getRowPos() const { return m_iPos; }
	int getEndRowPos() const { return m_iEndPos; }

	void setVisibleStateNormal();
	void setEnabled(bool enabled) override;
	void setLinkElement(SGUIElement* element);

	bool isUpButtonContainPoint(SGVec2 pos);
	bool isDownButtonContainPoint(SGVec2 pos);
	bool isThumbButtonContainPoint(SGVec2 pos);

	static SGUIScrollBar* create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIScrollBarInfo* scrollBarInfo);
	void restoreState(State state) override;

	void onMouseLeaveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseMoveDetail(SGEventMouse* mouseEvent) override;
	bool onMouseDownDetail(SGEventMouse* mouseEvent) override;
	void onMouseUpDetail(SGEventMouse* mouseEvent) override;
	bool onMouseScrollDetail(SGEventMouse* mouseEvent) override;

	UIElementType_t getElementType() override { return UIElementType::ScrollBar; }
	SGString toString() override { return SGStringUtil::Format("스크롤바(%d)", m_pInfo->Code); }

private:
	void onLinkElementMouseScroll(SGEventMouse* mouseEvent);
private:
	int m_iRowCount;
	int m_iRowCountPerPage;
	int m_iPos;
	int m_iEndPos;

	float m_fThumbHeight;
	float m_fUpButtonHeight;
	float m_fDownButtonHeight;
	float m_fSplitedTrackHeight;

	bool m_bDragBegin;
	float m_fDragStartYPos;
	int m_iDragStartPos;

	SGUIElement* m_pLink;
	SGUIScrollBarInfo* m_pInfo;
	SGFrameTexture* m_pTexture[TextureCount];
	SGSprite* m_pSprite[TextureCount];
};




