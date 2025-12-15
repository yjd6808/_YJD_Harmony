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
	static UIScrollBar* create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup);
	static UIScrollBar* create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo,
	                           bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::ScrollBar; }

	UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup);
	UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo,
	            bool _infoOwner);
	~UIScrollBar() override;

	bool init() override;
	void createSprites();

	void updateTrackAndButtonPosition();
	void Load() override;
	void Unload() override;

	void updateThumbSize();
	void updateThumbPosition();

	void setRowPos(int _pos);
	void setRowCount(int _count);
	void setRowCountPerPage(int _count);
	void SetUISize(const SGSize& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoScrollBar(UIScrollBarInfo* _pInfo, bool _infoOwner);
	int getRowPos() const { return pos_; }
	int getEndRowPos() const { return endPos_; }

	void setVisibleStateNormal();
	void SetEnabled(bool _enabled) override;
	void setLinkElement(UIElement* _pElement);

	bool isUpButtonContainPoint(SGVec2 _pos);
	bool isDownButtonContainPoint(SGVec2 _pos);
	bool isThumbButtonContainPoint(SGVec2 _pos);

	void RestoreState(State _state) override;

	UIScrollBarInfo* getInfo() const { return info_; }
	UIElementType_t GetElementType() override { return UIElementType::ScrollBar; }
	SGString ToString() override { return SGStringUtil::Format("스크롤바(%d)", info_->code_); }

protected:
	void OnMouseLeaveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent) override;
	void OnMouseUpInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent) override;
	bool OnMouseScrollInternalDetail(SGEventMouse* _pMouseEvent) override;

	void onLinkElementMouseScroll(SGEventMouse* _pMouseEvent);

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

	UIElement* linkElement_;
	UIScrollBarInfo* info_;
	FrameTexture* textures_[TextureCount];
	SGSprite* sprites_[TextureCount];
};
