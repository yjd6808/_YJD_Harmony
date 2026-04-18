/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#include "GameCoreHeader.h"
#include "Game/UI/UIScrollBar.h"


#include "sgcl/Game/UI/UIRootGroup.h"
#include "sgcl/Define/Define_Event.h"
#include "sgcl/Game/Contents/UIManager.h"
#include "sgcl/Game/Texture/ImagePackManager.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar::UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup)
: UIElement(_pMasterGroup, _pParentGroup)
, rowCount_(30)
, rowCountPerPage_(4)
, pos_(10)
, endPos_(rowCount_ - rowCountPerPage_)
, thumbHeight_(0)
, upButtonHeight_(DEFAULT_SIZE15.height)
, downButtonHeight_(DEFAULT_SIZE15.height)
, splitedTrackHeight_(0)
, trackHeight_(0.0f)
, dragBegin_(false)
, dragStartYPos_(0)
, dragStartPos_(0)
, pLinkElement_(nullptr)
, pInfo_(nullptr)
, pTextures_{}
, pSprites_{}
{
}

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar::UIScrollBar(
	UIRootGroup* _pMasterGroup, 
	UIGroup* _pParentGroup, 
	UIScrollBarInfo* _pScrollBarInfo, 
	bool _infoOwner)
: UIScrollBar(_pMasterGroup, _pParentGroup)
{
	SetInfoScrollBar(_pScrollBarInfo, _infoOwner);
}

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar* UIScrollBar::Create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup)
{
	UIScrollBar* pScrollBar = dbg_new UIScrollBar(_pMasterGroup, _pParentGroup);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar* UIScrollBar::Create(
	UIRootGroup* _pMasterGroup, 
	UIGroup* _pParentGroup, 
	UIScrollBarInfo* _pScrollBarInfo, 
	bool _infoOwner)
//////////////////////////////////////////////////////////////////////////////////////////
{
	UIScrollBar* pScrollBar = dbg_new UIScrollBar(_pMasterGroup, _pParentGroup, _pScrollBarInfo, _infoOwner);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

//////////////////////////////////////////////////////////////////////////////////////////
UIScrollBar::~UIScrollBar()
{
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		CC_SAFE_RELEASE(pTextures_[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 스크롤바 버튼들은 비활성화, 마우스 오버가 없으므로.. 비활성상태를 노말로 처리한다.
void UIScrollBar::SetVisibleStateNormal()
{
	if (pSprites_[INDEX_UP_NORMAL])
	{
		pSprites_[INDEX_UP_NORMAL]->setVisible(true);
	}
	if (pSprites_[INDEX_DOWN_NORMAL])
	{
		pSprites_[INDEX_DOWN_NORMAL]->setVisible(true);
	}
	if (pSprites_[INDEX_THUMB_NORMAL])
	{
		pSprites_[INDEX_THUMB_NORMAL]->setVisible(true);
	}
	if (pSprites_[INDEX_UP_PRESSED])
	{
		pSprites_[INDEX_UP_PRESSED]->setVisible(false);
	}
	if (pSprites_[INDEX_DOWN_PRESSED])
	{
		pSprites_[INDEX_DOWN_PRESSED]->setVisible(false);
	}
	if (pSprites_[INDEX_THUMB_PRESSED])
	{
		pSprites_[INDEX_THUMB_PRESSED]->setVisible(false);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIScrollBar::SetEnabled(bool _enabled)
{
	if (_enabled)
	{
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (state_ != eDisabled)
		{
			return;
		}

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
		UpdateState();
		return;
	}

	state_ = eDisabled;
	SetVisibleStateNormal();
}

void UIScrollBar::SetLinkElement(UIElement* _pElement)
{
	if (pLinkElement_ != nullptr)
	{
		pLinkElement_->RemoveMouseEvent(eMouseEventScroll, DEF_EVENT_UI_ON_MOUSE_SCROLL);
	}

	pLinkElement_ = _pElement;
	pLinkElement_->AddMouseEvent(eMouseEventScroll, DEF_EVENT_UI_ON_MOUSE_SCROLL,
	                            CC_CALLBACK_1(UIScrollBar::OnLinkElementMouseScroll, this));
}

void UIScrollBar::OnLinkElementMouseScroll(cc::EventMouse* _pMouseEvent)
{
	if (pLinkElement_ == nullptr)
	{
		_LogWarn_("링크 엘리먼트가 없는데 여기 들어온다고?");
		return;
	}

	if (_pMouseEvent->getScrollY() < 0)
	{
		SetRowPos(pos_ - 1);
	}
	else
	{
		SetRowPos(pos_ + 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIScrollBar::SetInitialUISize(cc::size _size)
{
	UIElement::SetInitialUISize(_size);

	ApplyUIScaleFactorY(upButtonHeight_);
	ApplyUIScaleFactorY(downButtonHeight_);
	trackHeight_ = uiSize_.height - upButtonHeight_ - downButtonHeight_;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIScrollBar::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	if (state_ == _state)
	{
		SetVisibleStateNormal();
	}
}

bool UIScrollBar::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	const ImagePack* pPack = g_cImagePackMgr.GetPackUnsafe(pInfo_->Sga);

	// 위 아래 버튼높이 임시 추가 (이미지가 전혀 없더라도 스크롤바 기능이 가능토록. 하기위함)
	SetInitialUISize(pInfo_->TrackSize + Size{ 0, upButtonHeight_ + downButtonHeight_ });

	if (pPack == nullptr)
	{
		_LogWarn_("스크롤바 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	if (pInfo_->TrackSize.height < MIN_THUMB_HEIGHT)
	{
		_LogWarn_("스크롤바 손잡이 사이즈가 (%.f)보다 작습니다. (손잡이가 업/다운 버튼을 침범할 수 있습니다.)", MIN_THUMB_HEIGHT);
	}

	const SgaSpriteAbstractPtr pSpriteUpNormal = pPack->GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[INDEX_UP_NORMAL]);
	const SgaSpriteAbstractPtr pSpriteUpPressed = pPack->GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[INDEX_UP_PRESSED]);
	const SgaSpriteAbstractPtr pSpriteDownNormal = pPack->GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[INDEX_DOWN_NORMAL]);
	const SgaSpriteAbstractPtr pSpriteDownPressed = pPack->
		GetSpriteUnsafe(pInfo_->Img, pInfo_->Sprites[INDEX_DOWN_PRESSED]);

	upButtonHeight_ = SgaSpriteHelper::GetMaxHeightF(pSpriteUpNormal, pSpriteUpPressed);
	downButtonHeight_ = SgaSpriteHelper::GetMaxHeightF(pSpriteDownNormal, pSpriteDownPressed);

	if (upButtonHeight_ == 0)
	{
		upButtonHeight_ = DEFAULT_SIZE15.height;
	}

	if (downButtonHeight_ == 0)
	{
		downButtonHeight_ = DEFAULT_SIZE15.height;
	}

	SetInitialUISize(pInfo_->TrackSize + cc::size{ 0, upButtonHeight_ + downButtonHeight_ });
	isInitialized_ = true;
	return true;
}

void UIScrollBar::Load()
{
	if (pInfo_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	if (isLoaded_)
	{
		return;
	}

	CreateSprites();
	UpdateThumbSize();

	// =============================================================
	//                        포지셔닝
	// 1. 영구적으로 변하지 않는 컨트롤의 위치를 결정한다.
	// 2. 손잡이의 경우 위치가 로카운트, 로포스에 따라 변한다.
	// 이때 모든 컨트롤의 X좌표는 0으로 고정되므로 Y좌표만 신경써주면 된다.
	// =============================================================

	UpdateTrackAndButtonPosition();
	UpdateThumbPosition();

	// =============================================================
	//                       디폴트 컬러링
	// 디폴트 텍스쳐인 경우 업버튼, 다운버튼, 손잡이, 트랙 색을 지정해서 구분 가능토록 만든다.
	// =============================================================
	const FrameTexture* pDefaultTexture = SpriteFrameTexture::GetDefault();

	// 업 버튼
	if (pTextures_[INDEX_UP_NORMAL] == pDefaultTexture)
	{
		pSprites_[INDEX_UP_NORMAL]->setColor(ColorList::Acidgreen_v);
	}

	if (pTextures_[INDEX_UP_PRESSED] == pDefaultTexture)
	{
		pSprites_[INDEX_UP_PRESSED]->setColor(ColorList::CGred_v);
	}

	// 다운 버튼
	if (pTextures_[INDEX_DOWN_NORMAL] == pDefaultTexture)
	{
		pSprites_[INDEX_DOWN_NORMAL]->setColor(ColorList::Acidgreen_v);
	}

	if (pTextures_[INDEX_DOWN_PRESSED] == pDefaultTexture)
	{
		pSprites_[INDEX_DOWN_PRESSED]->setColor(ColorList::CGred_v);
	}

	// 손잡이
	if (pTextures_[INDEX_THUMB_NORMAL] == pDefaultTexture)
	{
		pSprites_[INDEX_THUMB_NORMAL]->setColor(ColorList::Aeroblue_v);
	}

	if (pTextures_[INDEX_THUMB_PRESSED] == pDefaultTexture)
	{
		pSprites_[INDEX_THUMB_PRESSED]->setColor(ColorList::Alabaster_v);
	}

	SetVisibleStateNormal();
	isLoaded_ = true;
}

void UIScrollBar::CreateSprites()
{
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		FrameTexture* pTexture = g_cUIMgr.CreateUITextureRetained(
			pInfo_->Sga, pInfo_->Img, pInfo_->Sprites[i]);
		Sprite* pSprite;

		// 나인 렉트의 중앙 렉트 크기는 setCapInsets로 설정할 수 있다.
		if (i == INDEX_THUMB_NORMAL || i == INDEX_THUMB_PRESSED)
		{
			SpriteFrame* pFrame = SpriteFrame::createWithTexture(pTexture->GetTexture(), pTexture->GetRect());
			cc_ui::Scale9Sprite* pScale9 = cc_ui::Scale9Sprite::createWithSpriteFrame(pFrame);
			pSprite = pScale9;

			// 높이는 일단 암거나 지정, 어차피 이후 updateThumbSize() 함수에서 업데이트를 하기 때문..
			pSprite->setContentSize({ uiSize_.width, DEFAULT_SIZE45.height });
		}
		else
		{
			const cc::size spriteSize = pTexture->GetSize();
			const float widthScaleX = uiSize_.width / spriteSize.width;

			pSprite = Sprite::createWithTexture(pTexture->GetTexture());
			pSprite->setScaleX(widthScaleX);
		}

		pSprite->setAnchorPoint(Vec2::ZERO);

		pTextures_[i] = pTexture;
		pSprites_[i] = pSprite;
	}

	// 높이의 경우 트랙과 손잡이만 contentSize.height에 영향을 받으므로 스케일을 변경해줘야한다.
	// 손잡이의 경우 Scale9 스프이고 updateThumbSize()에서 수행하므로 트랙만 스케일 적용
	pSprites_[INDEX_TRACK]->setScaleY(trackHeight_ / pTextures_[INDEX_TRACK]->GetHeightF());

	pSprites_[INDEX_UP_NORMAL]->setScaleY(upButtonHeight_ / pTextures_[INDEX_UP_NORMAL]->GetHeightF());
	pSprites_[INDEX_UP_PRESSED]->setScaleY(upButtonHeight_ / pTextures_[INDEX_UP_PRESSED]->GetHeightF());
	pSprites_[INDEX_DOWN_NORMAL]->setScaleY(downButtonHeight_ / pTextures_[INDEX_DOWN_NORMAL]->GetHeightF());
	pSprites_[INDEX_DOWN_PRESSED]->setScaleY(downButtonHeight_ / pTextures_[INDEX_DOWN_PRESSED]->GetHeightF());

	this->addChild(pSprites_[INDEX_UP_NORMAL]);
	this->addChild(pSprites_[INDEX_UP_PRESSED]);
	this->addChild(pSprites_[INDEX_DOWN_NORMAL]);
	this->addChild(pSprites_[INDEX_DOWN_PRESSED]);
	this->addChild(pSprites_[INDEX_TRACK]);
	this->addChild(pSprites_[INDEX_THUMB_NORMAL]);
	this->addChild(pSprites_[INDEX_THUMB_PRESSED]);
}

void UIScrollBar::UpdateTrackAndButtonPosition()
{
	// 업 버튼
	pSprites_[INDEX_UP_NORMAL]->setPosition(0, downButtonHeight_ + trackHeight_);
	pSprites_[INDEX_UP_PRESSED]->setPosition(0, downButtonHeight_ + trackHeight_);

	// sprites_[IndexDownNormal]->setPosition(0, 0);
	// sprites_[IndexDownPressed]->setPosition(0, 0);

	// 트랙
	pSprites_[INDEX_TRACK]->setPosition(0, downButtonHeight_);
}

void UIScrollBar::UpdateThumbSize()
{
	const float splitedHeight = trackHeight_ / rowCount_;
	const float thumbHeight = Math::Max(splitedHeight * rowCountPerPage_, MIN_THUMB_HEIGHT);

	// 스케일스프는 콘텐츠 사이즈로 변경해야함
	thumbHeight_ = thumbHeight;
	pSprites_[INDEX_THUMB_NORMAL]->setContentSize({ uiSize_.width, thumbHeight });
	pSprites_[INDEX_THUMB_PRESSED]->setContentSize({ uiSize_.width, thumbHeight });
}

void UIScrollBar::UpdateThumbPosition()
{
	// 손잡이 X 위치
	// sprites_[IndexThumbNormal]->setPositionX(0);
	// sprites_[IndexThumbPressed]->setPositionX(0);

	const float splitedHeight = trackHeight_ / rowCount_;
	const float thumbHeight = Math::Max(splitedHeight * rowCountPerPage_, MIN_THUMB_HEIGHT);
	const float leftTrackHeight = trackHeight_ - thumbHeight;

	splitedTrackHeight_ = endPos_ != 0 ? leftTrackHeight / endPos_ : 0;
	thumbHeight_ = thumbHeight;
	pSprites_[INDEX_THUMB_NORMAL]->setPositionY(
		downButtonHeight_ + splitedTrackHeight_ * static_cast<float>(endPos_ - pos_));
	pSprites_[INDEX_THUMB_PRESSED]->setPositionY(
		downButtonHeight_ + splitedTrackHeight_ * static_cast<float>(endPos_ - pos_));
}

void UIScrollBar::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		pSprites_[i] = nullptr;
		CC_SAFE_RELEASE_NULL(pTextures_[i]);
	}

	isLoaded_ = false;
}

void UIScrollBar::SetRowPos(int _pos)
{
	if (_pos == pos_)
	{
		return;
	}

	const int prevPos = pos_;

	if (_pos <= 0)
	{
		pos_ = 0;
	}
	else if (_pos > endPos_)
	{
		pos_ = endPos_;
	}
	else
	{
		pos_ = _pos;
	}

	pRootGroup_->OnScrollBarPositionChanged(this, prevPos, pos_);
	UpdateThumbPosition();
}

void UIScrollBar::SetRowCount(int _count)
{
	if (_count < rowCountPerPage_)
	{
		rowCount_ = rowCountPerPage_;
		_LogWarn_("스크롤바의 RowCount를 %d보다 낮출려는 시도가 있었습니다. (%d로 설정합니다.)", rowCountPerPage_, rowCountPerPage_);
	}

	rowCount_ = _count;
	endPos_ = rowCount_ - rowCountPerPage_;
	UpdateThumbSize();
	UpdateThumbPosition();
}

void UIScrollBar::SetRowCountPerPage(int _count)
{
	if (_count <= 0)
	{
		rowCountPerPage_ = 1;
		_LogWarn_("스크롤바의 rowCountPerPage_를 0으로 설정할려는 시도가 있었습니다. (1로 설정합니다.)");
	}

	rowCountPerPage_ = _count;
	endPos_ = rowCount_ - rowCountPerPage_;

	if (pos_ > endPos_)
	{
		pos_ = endPos_;
	}

	UpdateThumbSize();
	UpdateThumbPosition();
}

void UIScrollBar::SetUISize(const cc::size& _contentSize)
{
	if (!isResizable_)
	{
		return;
	}

	uiSize_ = _contentSize;
	trackHeight_ = _contentSize.height - downButtonHeight_ - upButtonHeight_;

	if (!isLoaded_)
	{
		return;
	}

	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		FrameTexture* pTexture = pTextures_[i];
		Sprite* pSprite = pSprites_[i];

		if (pTexture == nullptr || pSprite == nullptr)
		{
			continue;
		}

		// Scale9 스프이므로 updateThumbSize()에서 업데이트함
		if (i == INDEX_THUMB_NORMAL || i == INDEX_THUMB_PRESSED)
		{
			continue;
		}

		pSprite->setScaleX(uiSize_.width / pTextures_[i]->GetWidthF());
	}

	pSprites_[INDEX_TRACK]->setScaleY(trackHeight_ / pTextures_[INDEX_TRACK]->GetHeightF());

	UpdateTrackAndButtonPosition();
	UpdateThumbSize();
	UpdateThumbPosition();
}

void UIScrollBar::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->type_ != UIElementType::ScrollBar)
	{
		LogWarnInvalidInfo(_pInfo->type_);
		return;
	}

	if (isInfoOwner_)
	{
		JC_DELETE_SAFE(pInfo_);
	}

	pBaseInfo_ = _pInfo;
	pInfo_ = static_cast<UIScrollBarInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIScrollBar::SetInfoScrollBar(UIScrollBarInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

bool UIScrollBar::IsUpButtonContainPoint(cc::vec2 _pos)
{
	if (pSprites_[INDEX_UP_NORMAL] == nullptr)
	{
		return false;
	}

	const cc::vec2 upButtonPos = pSprites_[INDEX_UP_NORMAL]->getPosition();
	const Rect upButtonBoundingBox = { _position + upButtonPos, cc::size{ uiSize_.width, upButtonHeight_ } };
	return upButtonBoundingBox.containsPoint(_pos);
}

bool UIScrollBar::IsDownButtonContainPoint(cc::vec2 _pos)
{
	if (pSprites_[INDEX_DOWN_NORMAL] == nullptr)
	{
		return false;
	}

	const cc::vec2 downButtonPos = pSprites_[INDEX_DOWN_NORMAL]->getPosition();
	const Rect downButtonBoundingBox = { _position + downButtonPos, cc::size{ uiSize_.width, downButtonHeight_ } };
	return downButtonBoundingBox.containsPoint(_pos);
}

bool UIScrollBar::IsThumbButtonContainPoint(cc::vec2 _pos)
{
	if (pSprites_[INDEX_THUMB_NORMAL] == nullptr)
	{
		return false;
	}

	const cc::vec2 thumbButtonPos = pSprites_[INDEX_THUMB_NORMAL]->getPosition();
	const Rect thumbButtonBoundingBox = { _position + thumbButtonPos, pSprites_[INDEX_THUMB_NORMAL]->getContentSize() };
	return thumbButtonBoundingBox.containsPoint(_pos);
}

void UIScrollBar::OnMouseLeaveInternalDetail(cc::EventMouse* /*_pMouseEvent*/)
{
	// setVisibleStateNormal();
}

bool UIScrollBar::OnMouseMoveInternalDetail(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 cursorPos = _pMouseEvent->getCursorPos();

	if (!dragBegin_)
	{
		return true;
	}

	// 손잡이 드래그중인경우
	const float thumbDragYPos = cursorPos.y - _position.y - downButtonHeight_;
	const float dragDelta = dragStartYPos_ - thumbDragYPos;
	const int posDelta = static_cast<int>(dragDelta / splitedTrackHeight_);
	SetRowPos(dragStartPos_ + posDelta);
	return false;
}

bool UIScrollBar::OnMouseDownInternalDetail(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 cursorPos = _pMouseEvent->getCursorPos();

	const bool upButtonContained = IsUpButtonContainPoint(cursorPos);
	const bool downButtonContained = IsDownButtonContainPoint(cursorPos);
	const bool thumbButtonContained = IsThumbButtonContainPoint(cursorPos);

	if (pSprites_[INDEX_UP_PRESSED])
	{
		pSprites_[INDEX_UP_PRESSED]->setVisible(upButtonContained);
	}
	if (pSprites_[INDEX_UP_NORMAL])
	{
		pSprites_[INDEX_UP_NORMAL]->setVisible(!upButtonContained);
	}

	if (pSprites_[INDEX_DOWN_PRESSED])
	{
		pSprites_[INDEX_DOWN_PRESSED]->setVisible(downButtonContained);
	}
	if (pSprites_[INDEX_DOWN_NORMAL])
	{
		pSprites_[INDEX_DOWN_NORMAL]->setVisible(!downButtonContained);
	}

	if (pSprites_[INDEX_THUMB_PRESSED])
	{
		pSprites_[INDEX_THUMB_PRESSED]->setVisible(thumbButtonContained);
	}
	if (pSprites_[INDEX_THUMB_NORMAL])
	{
		pSprites_[INDEX_THUMB_NORMAL]->setVisible(!thumbButtonContained);
	}

	dragBegin_ = thumbButtonContained;
	dragStartYPos_ = cursorPos.y - _position.y - downButtonHeight_;
	dragStartPos_ = pos_;
	return false;
}

void UIScrollBar::OnMouseUpInternalDetail(cc::EventMouse* /*_pMouseEvent*/)
{
	SetVisibleStateNormal();
	dragBegin_ = false;
}

bool UIScrollBar::OnMouseUpContainedInternalDetail(cc::EventMouse* _pMouseEvent)
{
	const cc::vec2 cursorPos = _pMouseEvent->getCursorPos();

	if (IsUpButtonContainPoint(cursorPos))
	{
		SetRowPos(pos_ - 1);
		pRootGroup_->OnScrollBarUpButtonPressed(this, pos_);
		return false;
	}

	if (IsDownButtonContainPoint(cursorPos))
	{
		SetRowPos(pos_ + 1);
		pRootGroup_->OnScrollBarDownButtonPressed(this, pos_);
		return false;
	}

	if (IsThumbButtonContainPoint(cursorPos))
	{
		pRootGroup_->OnScrollBarThumbButtonPressed(this, pos_);
		return false;
	}

	return true;
}

bool UIScrollBar::OnMouseScrollInternalDetail(cc::EventMouse* _pMouseEvent)
{
	if (_pMouseEvent->getScrollY() < 0)
	{
		SetRowPos(pos_ - 1);
	}
	else
	{
		SetRowPos(pos_ + 1);
	}

	return true;
}
