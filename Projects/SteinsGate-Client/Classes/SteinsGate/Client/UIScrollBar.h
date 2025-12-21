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
	static constexpr int TEXTURE_COUNT = 7;
	static constexpr int INDEX_UP_NORMAL = 0;
	static constexpr int INDEX_UP_PRESSED = 1;
	static constexpr int INDEX_DOWN_NORMAL = 2;
	static constexpr int INDEX_DOWN_PRESSED = 3;
	static constexpr int INDEX_THUMB_NORMAL = 4;
	static constexpr int INDEX_THUMB_PRESSED = 5;
	static constexpr int INDEX_TRACK = 6;

	static constexpr float MIN_THUMB_HEIGHT = 8;

public:
	static UIScrollBar* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup);
	static UIScrollBar* Create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::ScrollBar; }

	UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup);
	UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo, bool _infoOwner);
	~UIScrollBar() override;

	bool init() override;
	void CreateSprites();

	void UpdateTrackAndButtonPosition();
	void Load() override;
	void Unload() override;

	void UpdateThumbSize();
	void UpdateThumbPosition();

	void SetRowPos(int _pos);
	void SetRowCount(int _count);
	void SetRowCountPerPage(int _count);
	void SetUISize(const SGSize& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoScrollBar(UIScrollBarInfo* _pInfo, bool _infoOwner);
	int GetRowPos() const { return pos_; }
	int GetEndRowPos() const { return endPos_; }

	void SetVisibleStateNormal();
	void SetEnabled(bool _enabled) override;
	void SetLinkElement(UIElement* _pElement);

	bool IsUpButtonContainPoint(SGVec2 _pos);
	bool IsDownButtonContainPoint(SGVec2 _pos);
	bool IsThumbButtonContainPoint(SGVec2 _pos);

	void RestoreState(State _state) override;

	UIScrollBarInfo* GetInfo() const { return pInfo_; }
	UIElementType_t GetElementType() override { return UIElementType::ScrollBar; }
	SGString ToString() override { return SGStringUtil::Format("스크롤바(%d)", pInfo_->code_); }

protected:
	void OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseScrollInternalDetail(SGEventMouse* _pMouseEvent) override;

	void OnLinkElementMouseScroll(SGEventMouse* _pMouseEvent);

	void SetInitialUISize(SGSize _size) override;

private:
	int rowCount_;
	int rowCountPerPage_;
	int pos_;
	int endPos_;

	float thumbHeight_;
	float upButtonHeight_;
	float downButtonHeight_;
	float splitedTrackHeight_;
	float trackHeight_;

	bool dragBegin_;
	float dragStartYPos_;
	int dragStartPos_;

	UIElement* pLinkElement_;
	UIScrollBarInfo* pInfo_;
	FrameTexture* pTextures_[TEXTURE_COUNT];
	SGSprite* pSprites_[TEXTURE_COUNT];
};
