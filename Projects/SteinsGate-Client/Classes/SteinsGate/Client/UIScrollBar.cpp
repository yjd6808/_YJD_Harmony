/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIScrollBar.h"

#include <SteinsGate/Common/RectEx.h>

#include <SteinsGate/Client/UIRootGroup.h>
#include <SteinsGate/Client/Define_Event.h>

USING_NS_CC;
USING_NS_JC;

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
, linkElement_(nullptr)
, info_(nullptr)
, textures_{}
, sprites_{}
{
}

UIScrollBar::UIScrollBar(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo,
                         bool _infoOwner)
: UIScrollBar(_pMasterGroup, _pParentGroup)
{
	setInfoScrollBar(_pScrollBarInfo, _infoOwner);
}

UIScrollBar* UIScrollBar::create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup)
{
	UIScrollBar* pScrollBar = dbg_new UIScrollBar(_pMasterGroup, _pParentGroup);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

UIScrollBar* UIScrollBar::create(UIRootGroup* _pMasterGroup, UIGroup* _pParentGroup, UIScrollBarInfo* _pScrollBarInfo,
                                 bool _infoOwner)
{
	UIScrollBar* pScrollBar = dbg_new UIScrollBar(_pMasterGroup, _pParentGroup, _pScrollBarInfo, _infoOwner);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

UIScrollBar::~UIScrollBar()
{
	for (int i = 0; i < TextureCount; ++i)
	{
		CC_SAFE_RELEASE(textures_[i]);
	}
}

// 스크롤바 버튼들은 비활성화, 마우스 오버가 없으므로.. 비활성상태를 노말로 처리한다.
void UIScrollBar::setVisibleStateNormal()
{
	if (sprites_[IndexUpNormal])
	{
		sprites_[IndexUpNormal]->setVisible(true);
	}
	if (sprites_[IndexDownNormal])
	{
		sprites_[IndexDownNormal]->setVisible(true);
	}
	if (sprites_[IndexThumbNormal])
	{
		sprites_[IndexThumbNormal]->setVisible(true);
	}
	if (sprites_[IndexUpPressed])
	{
		sprites_[IndexUpPressed]->setVisible(false);
	}
	if (sprites_[IndexDownPressed])
	{
		sprites_[IndexDownPressed]->setVisible(false);
	}
	if (sprites_[IndexThumbPressed])
	{
		sprites_[IndexThumbPressed]->setVisible(false);
	}
}

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
	setVisibleStateNormal();
}

void UIScrollBar::setLinkElement(UIElement* _pElement)
{
	if (linkElement_ != nullptr)
	{
		linkElement_->RemoveMouseEvent(eMouseEventScroll, DEF_EVENT_UI_ON_MOUSE_SCROLL);
	}

	linkElement_ = _pElement;
	linkElement_->AddMouseEvent(eMouseEventScroll, DEF_EVENT_UI_ON_MOUSE_SCROLL,
	                            CC_CALLBACK_1(UIScrollBar::onLinkElementMouseScroll, this));
}

void UIScrollBar::onLinkElementMouseScroll(SGEventMouse* _pMouseEvent)
{
	if (linkElement_ == nullptr)
	{
		_LogWarn_("링크 엘리먼트가 없는데 여기 들어온다고?");
		return;
	}

	if (_pMouseEvent->getScrollY() < 0)
	{
		setRowPos(pos_ - 1);
	}
	else
	{
		setRowPos(pos_ + 1);
	}
}

void UIScrollBar::SetInitialUISize(SGSize _size)
{
	UIElement::SetInitialUISize(_size);

	upButtonHeight_ *= Core::ClientInfo->uiScaleYFactor_;
	downButtonHeight_ *= Core::ClientInfo->uiScaleYFactor_;
	trackHeight_ = uiSize_.height - upButtonHeight_ - downButtonHeight_;
}

void UIScrollBar::RestoreState(State _state)
{
	if (state_ == eDisabled)
	{
		return;
	}

	if (state_ == _state)
	{
		setVisibleStateNormal();
	}
}

bool UIScrollBar::init()
{
	if (!UIElement::init())
	{
		return false;
	}

	if (info_ == nullptr)
	{
		LogWarnMissingInfo();
		return false;
	}

	const ImagePack* pPack = Core::Contents.PackManager->getPackUnsafe(info_->Sga);

	// 위 아래 버튼높이 임시 추가 (이미지가 전혀 없더라도 스크롤바 기능이 가능토록. 하기위함)
	SetInitialUISize(info_->TrackSize + Size{ 0, upButtonHeight_ + downButtonHeight_ });

	if (pPack == nullptr)
	{
		_LogWarn_("스크롤바 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	if (info_->TrackSize.height < MinThumbHeight)
	{
		_LogWarn_("스크롤바 손잡이 사이즈가 (%.f)보다 작습니다. (손잡이가 업/다운 버튼을 침범할 수 있습니다.)", MinThumbHeight);
	}

	const SgaSpriteAbstractPtr pSpriteUpNormal = pPack->getSpriteUnsafe(info_->Img, info_->Sprites[IndexUpNormal]);
	const SgaSpriteAbstractPtr pSpriteUpPressed = pPack->getSpriteUnsafe(info_->Img, info_->Sprites[IndexUpPressed]);
	const SgaSpriteAbstractPtr pSpriteDownNormal = pPack->getSpriteUnsafe(info_->Img, info_->Sprites[IndexDownNormal]);
	const SgaSpriteAbstractPtr pSpriteDownPressed = pPack->
		getSpriteUnsafe(info_->Img, info_->Sprites[IndexDownPressed]);

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

	SetInitialUISize(info_->TrackSize + SGSize{ 0, upButtonHeight_ + downButtonHeight_ });
	isInitialized_ = true;
	return true;
}

void UIScrollBar::Load()
{
	if (info_ == nullptr)
	{
		LogWarnMissingInfo();
		return;
	}

	if (isLoaded_)
	{
		return;
	}

	createSprites();
	updateThumbSize();

	// =============================================================
	//                        포지셔닝
	// 1. 영구적으로 변하지 않는 컨트롤의 위치를 결정한다.
	// 2. 손잡이의 경우 위치가 로카운트, 로포스에 따라 변한다.
	// 이때 모든 컨트롤의 X좌표는 0으로 고정되므로 Y좌표만 신경써주면 된다.
	// =============================================================

	updateTrackAndButtonPosition();
	updateThumbPosition();

	// =============================================================
	//                       디폴트 컬러링
	// 디폴트 텍스쳐인 경우 업버튼, 다운버튼, 손잡이, 트랙 색을 지정해서 구분 가능토록 만든다.
	// =============================================================
	const FrameTexture* pDefaultTexture = Core::Contents.Global->getDefaultFrameTexture();

	// 업 버튼
	if (textures_[IndexUpNormal] == pDefaultTexture)
	{
		sprites_[IndexUpNormal]->setColor(SGColorList::Acidgreen_v);
	}

	if (textures_[IndexUpPressed] == pDefaultTexture)
	{
		sprites_[IndexUpPressed]->setColor(SGColorList::CGred_v);
	}

	// 다운 버튼
	if (textures_[IndexDownNormal] == pDefaultTexture)
	{
		sprites_[IndexDownNormal]->setColor(SGColorList::Acidgreen_v);
	}

	if (textures_[IndexDownPressed] == pDefaultTexture)
	{
		sprites_[IndexDownPressed]->setColor(SGColorList::CGred_v);
	}

	// 손잡이
	if (textures_[IndexThumbNormal] == pDefaultTexture)
	{
		sprites_[IndexThumbNormal]->setColor(SGColorList::Aeroblue_v);
	}

	if (textures_[IndexThumbPressed] == pDefaultTexture)
	{
		sprites_[IndexThumbPressed]->setColor(SGColorList::Alabaster_v);
	}

	setVisibleStateNormal();
	isLoaded_ = true;
}

void UIScrollBar::createSprites()
{
	for (int i = 0; i < TextureCount; ++i)
	{
		FrameTexture* pTexture = Core::Contents.UIManager->createUITextureRetained(
			info_->Sga, info_->Img, info_->Sprites[i]);
		Sprite* pSprite;

		// 나인 렉트의 중앙 렉트 크기는 setCapInsets로 설정할 수 있다.
		if (i == IndexThumbNormal || i == IndexThumbPressed)
		{
			SpriteFrame* pFrame = SpriteFrame::createWithTexture(pTexture->getTexture(), pTexture->getRect());
			SGScale9Sprite* pScale9 = SGScale9Sprite::createWithSpriteFrame(pFrame);
			pSprite = pScale9;

			// 높이는 일단 암거나 지정, 어차피 이후 updateThumbSize() 함수에서 업데이트를 하기 때문..
			pSprite->setContentSize({ uiSize_.width, DEFAULT_SIZE45.height });
		}
		else
		{
			const SGSize spriteSize = pTexture->getSize();
			const float widthScaleX = uiSize_.width / spriteSize.width;

			pSprite = Sprite::createWithTexture(pTexture->getTexture());
			pSprite->setScaleX(widthScaleX);
		}

		pSprite->setAnchorPoint(Vec2::ZERO);

		textures_[i] = pTexture;
		sprites_[i] = pSprite;
	}

	// 높이의 경우 트랙과 손잡이만 contentSize.height에 영향을 받으므로 스케일을 변경해줘야한다.
	// 손잡이의 경우 Scale9 스프이고 updateThumbSize()에서 수행하므로 트랙만 스케일 적용
	sprites_[IndexTrack]->setScaleY(trackHeight_ / textures_[IndexTrack]->getHeightF());

	sprites_[IndexUpNormal]->setScaleY(upButtonHeight_ / textures_[IndexUpNormal]->getHeightF());
	sprites_[IndexUpPressed]->setScaleY(upButtonHeight_ / textures_[IndexUpPressed]->getHeightF());
	sprites_[IndexDownNormal]->setScaleY(downButtonHeight_ / textures_[IndexDownNormal]->getHeightF());
	sprites_[IndexDownPressed]->setScaleY(downButtonHeight_ / textures_[IndexDownPressed]->getHeightF());

	this->addChild(sprites_[IndexUpNormal]);
	this->addChild(sprites_[IndexUpPressed]);
	this->addChild(sprites_[IndexDownNormal]);
	this->addChild(sprites_[IndexDownPressed]);
	this->addChild(sprites_[IndexTrack]);
	this->addChild(sprites_[IndexThumbNormal]);
	this->addChild(sprites_[IndexThumbPressed]);
}

void UIScrollBar::updateTrackAndButtonPosition()
{
	// 업 버튼
	sprites_[IndexUpNormal]->setPosition(0, downButtonHeight_ + trackHeight_);
	sprites_[IndexUpPressed]->setPosition(0, downButtonHeight_ + trackHeight_);

	// sprites_[IndexDownNormal]->setPosition(0, 0);
	// sprites_[IndexDownPressed]->setPosition(0, 0);

	// 트랙
	sprites_[IndexTrack]->setPosition(0, downButtonHeight_);
}

void UIScrollBar::updateThumbSize()
{
	const float splitedHeight = trackHeight_ / rowCount_;
	const float thumbHeight = Math::Max(splitedHeight * rowCountPerPage_, MinThumbHeight);

	// 스케일스프는 콘텐츠 사이즈로 변경해야함
	thumbHeight_ = thumbHeight;
	sprites_[IndexThumbNormal]->setContentSize({ uiSize_.width, thumbHeight });
	sprites_[IndexThumbPressed]->setContentSize({ uiSize_.width, thumbHeight });
}

void UIScrollBar::updateThumbPosition()
{
	// 손잡이 X 위치
	// sprites_[IndexThumbNormal]->setPositionX(0);
	// sprites_[IndexThumbPressed]->setPositionX(0);

	const float splitedHeight = trackHeight_ / rowCount_;
	const float thumbHeight = Math::Max(splitedHeight * rowCountPerPage_, MinThumbHeight);
	const float leftTrackHeight = trackHeight_ - thumbHeight;

	splitedTrackHeight_ = endPos_ != 0 ? leftTrackHeight / endPos_ : 0;
	thumbHeight_ = thumbHeight;
	sprites_[IndexThumbNormal]->setPositionY(
		downButtonHeight_ + splitedTrackHeight_ * static_cast<float>(endPos_ - pos_));
	sprites_[IndexThumbPressed]->setPositionY(
		downButtonHeight_ + splitedTrackHeight_ * static_cast<float>(endPos_ - pos_));
}

void UIScrollBar::Unload()
{
	if (!isLoaded_)
	{
		return;
	}

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i)
	{
		sprites_[i] = nullptr;
		CC_SAFE_RELEASE_NULL(textures_[i]);
	}

	isLoaded_ = false;
}

void UIScrollBar::setRowPos(int _pos)
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
	updateThumbPosition();
}

void UIScrollBar::setRowCount(int _count)
{
	if (_count < rowCountPerPage_)
	{
		rowCount_ = rowCountPerPage_;
		_LogWarn_("스크롤바의 RowCount를 %d보다 낮출려는 시도가 있었습니다. (%d로 설정합니다.)", rowCountPerPage_, rowCountPerPage_);
	}

	rowCount_ = _count;
	endPos_ = rowCount_ - rowCountPerPage_;
	updateThumbSize();
	updateThumbPosition();
}

void UIScrollBar::setRowCountPerPage(int _count)
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

	updateThumbSize();
	updateThumbPosition();
}

void UIScrollBar::SetUISize(const SGSize& _contentSize)
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

	for (int i = 0; i < TextureCount; ++i)
	{
		FrameTexture* pTexture = textures_[i];
		Sprite* pSprite = sprites_[i];

		if (pTexture == nullptr || pSprite == nullptr)
		{
			continue;
		}

		// Scale9 스프이므로 updateThumbSize()에서 업데이트함
		if (i == IndexThumbNormal || i == IndexThumbPressed)
		{
			continue;
		}

		pSprite->setScaleX(uiSize_.width / textures_[i]->getWidthF());
	}

	sprites_[IndexTrack]->setScaleY(trackHeight_ / textures_[IndexTrack]->getHeightF());

	updateTrackAndButtonPosition();
	updateThumbSize();
	updateThumbPosition();
}

void UIScrollBar::SetInfo(UIElementInfo* _pInfo, bool _infoOwner)
{
	if (_pInfo->Type != UIElementType::ScrollBar)
	{
		LogWarnInvalidInfo(_pInfo->Type);
		return;
	}

	if (isInfoOwner_)
	{
		JCORE_DELETE_SAFE(info_);
	}

	pBaseInfo_ = _pInfo;
	info_ = static_cast<UIScrollBarInfo*>(_pInfo);
	isInfoOwner_ = _infoOwner;
}

void UIScrollBar::setInfoScrollBar(UIScrollBarInfo* _pInfo, bool _infoOwner)
{
	SetInfo(_pInfo, _infoOwner);
}

bool UIScrollBar::isUpButtonContainPoint(SGVec2 _pos)
{
	if (sprites_[IndexUpNormal] == nullptr)
	{
		return false;
	}

	const SGVec2 upButtonPos = sprites_[IndexUpNormal]->getPosition();
	const Rect upButtonBoundingBox = { _position + upButtonPos, SGSize{ uiSize_.width, upButtonHeight_ } };
	return upButtonBoundingBox.containsPoint(_pos);
}

bool UIScrollBar::isDownButtonContainPoint(SGVec2 _pos)
{
	if (sprites_[IndexDownNormal] == nullptr)
	{
		return false;
	}

	const SGVec2 downButtonPos = sprites_[IndexDownNormal]->getPosition();
	const Rect downButtonBoundingBox = { _position + downButtonPos, SGSize{ uiSize_.width, downButtonHeight_ } };
	return downButtonBoundingBox.containsPoint(_pos);
}

bool UIScrollBar::isThumbButtonContainPoint(SGVec2 _pos)
{
	if (sprites_[IndexThumbNormal] == nullptr)
	{
		return false;
	}

	const SGVec2 thumbButtonPos = sprites_[IndexThumbNormal]->getPosition();
	const Rect thumbButtonBoundingBox = { _position + thumbButtonPos, sprites_[IndexThumbNormal]->getContentSize() };
	return thumbButtonBoundingBox.containsPoint(_pos);
}

void UIScrollBar::OnMouseLeaveInternalDetail(SGEventMouse* /*_pMouseEvent*/)
{
	// setVisibleStateNormal();
}

bool UIScrollBar::OnMouseMoveInternalDetail(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	if (!dragBegin_)
	{
		return true;
	}

	// 손잡이 드래그중인경우
	const float thumbDragYPos = cursorPos.y - _position.y - downButtonHeight_;
	const float dragDelta = dragStartYPos_ - thumbDragYPos;
	const int posDelta = static_cast<int>(dragDelta / splitedTrackHeight_);
	setRowPos(dragStartPos_ + posDelta);
	return false;
}

bool UIScrollBar::OnMouseDownInternalDetail(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	const bool upButtonContained = isUpButtonContainPoint(cursorPos);
	const bool downButtonContained = isDownButtonContainPoint(cursorPos);
	const bool thumbButtonContained = isThumbButtonContainPoint(cursorPos);

	if (sprites_[IndexUpPressed])
	{
		sprites_[IndexUpPressed]->setVisible(upButtonContained);
	}
	if (sprites_[IndexUpNormal])
	{
		sprites_[IndexUpNormal]->setVisible(!upButtonContained);
	}

	if (sprites_[IndexDownPressed])
	{
		sprites_[IndexDownPressed]->setVisible(downButtonContained);
	}
	if (sprites_[IndexDownNormal])
	{
		sprites_[IndexDownNormal]->setVisible(!downButtonContained);
	}

	if (sprites_[IndexThumbPressed])
	{
		sprites_[IndexThumbPressed]->setVisible(thumbButtonContained);
	}
	if (sprites_[IndexThumbNormal])
	{
		sprites_[IndexThumbNormal]->setVisible(!thumbButtonContained);
	}

	dragBegin_ = thumbButtonContained;
	dragStartYPos_ = cursorPos.y - _position.y - downButtonHeight_;
	dragStartPos_ = pos_;
	return false;
}

void UIScrollBar::OnMouseUpInternalDetail(SGEventMouse* /*_pMouseEvent*/)
{
	setVisibleStateNormal();
	dragBegin_ = false;
}

bool UIScrollBar::OnMouseUpContainedInternalDetail(SGEventMouse* _pMouseEvent)
{
	const SGVec2 cursorPos = _pMouseEvent->getCursorPos();

	if (isUpButtonContainPoint(cursorPos))
	{
		setRowPos(pos_ - 1);
		pRootGroup_->OnScrollBarUpButtonPressed(this, pos_);
		return false;
	}

	if (isDownButtonContainPoint(cursorPos))
	{
		setRowPos(pos_ + 1);
		pRootGroup_->OnScrollBarDownButtonPressed(this, pos_);
		return false;
	}

	if (isThumbButtonContainPoint(cursorPos))
	{
		pRootGroup_->OnScrollBarThumbButtonPressed(this, pos_);
		return false;
	}

	return true;
}

bool UIScrollBar::OnMouseScrollInternalDetail(SGEventMouse* _pMouseEvent)
{
	if (_pMouseEvent->getScrollY() < 0)
	{
		setRowPos(pos_ - 1);
	}
	else
	{
		setRowPos(pos_ + 1);
	}

	return true;
}
