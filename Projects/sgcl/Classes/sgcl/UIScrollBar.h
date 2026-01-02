/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#pragma once

#include <sgcl/UIElement.h>

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
	void SetUISize(const cc::size& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoScrollBar(UIScrollBarInfo* _pInfo, bool _infoOwner);
	int GetRowPos() const { return pos_; }
	int GetEndRowPos() const { return endPos_; }

	void SetVisibleStateNormal();
	void SetEnabled(bool _enabled) override;
	void SetLinkElement(UIElement* _pElement);

	bool IsUpButtonContainPoint(cc::vec2 _pos);
	bool IsDownButtonContainPoint(cc::vec2 _pos);
	bool IsThumbButtonContainPoint(cc::vec2 _pos);

	void RestoreState(State _state) override;

	UIScrollBarInfo* GetInfo() const { return pInfo_; }
	UIElementType_t GetElementType() override { return UIElementType::ScrollBar; }
	jc::String ToString() override { return jc::StringUtil::Format("스크롤바(%d)", pInfo_->code_); }

protected:
	void OnMouseLeaveInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseUpContainedInternalDetail(cc::EventMouse* _pMouseEvent) override;
	bool OnMouseScrollInternalDetail(cc::EventMouse* _pMouseEvent) override;

	void OnLinkElementMouseScroll(cc::EventMouse* _pMouseEvent);

	void SetInitialUISize(cc::size _size) override;

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
	cc::Sprite* pSprites_[TEXTURE_COUNT];
};
