/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "UIScrollBar.h"

#include <SteinsGate/Client/UIMasterGroup.h>
#include <SteinsGate/Common/RectEx.h>

USING_NS_CC;
USING_NS_JC;

UIScrollBar::UIScrollBar(UIMasterGroup* master, UIGroup* parent, UIScrollBarInfo* scrollBarInfo)
	: UIElement(master, parent, scrollBarInfo)
	, m_iRowCount(30)
	, m_iRowCountPerPage(4)
	, m_iPos(10)
	, m_iEndPos(m_iRowCount - m_iRowCountPerPage)
	, m_fThumbHeight(0)
	, m_fUpButtonHeight(DefaultSize15.height)
	, m_fDownButtonHeight(DefaultSize15.height)
	, m_fSplitedTrackHeight(0)
	, m_fTrackHeight(0.0f)
	, m_bDragBegin(false)
	, m_fDragStartYPos(0)
	, m_iDragStartPos(0)
	, m_pLink(nullptr)
	, m_pInfo(scrollBarInfo)
	, m_pTexture{}
    , m_pSprite{}
{}

UIScrollBar* UIScrollBar::create(UIMasterGroup* master, UIGroup* parent, UIScrollBarInfo* scrollBarInfo) {
	UIScrollBar* pScrollBar = dbg_new UIScrollBar(master, parent, scrollBarInfo);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

UIScrollBar::~UIScrollBar() {
	for (int i = 0; i < TextureCount; ++i) {
		CC_SAFE_RELEASE(m_pTexture[i]);
	}
}



// 스크롤바 버튼들은 비활성화, 마우스 오버가 없으므로.. 비활성상태를 노말로 처리한다.
void UIScrollBar::setVisibleStateNormal() {
	m_pSprite[IndexUpNormal]->setVisible(true);
	m_pSprite[IndexDownNormal]->setVisible(true);
	m_pSprite[IndexThumbNormal]->setVisible(true);
	m_pSprite[IndexUpPressed]->setVisible(false);
	m_pSprite[IndexDownPressed]->setVisible(false);
	m_pSprite[IndexThumbPressed]->setVisible(false);
}

void UIScrollBar::setEnabled(bool enabled) {

	if (enabled) {
		// 활성화 요청을 했지만 이미 활성화 상태인 경우는 무시
		if (m_eState != eDisabled)
			return;

		// 이전 상태가 비활성화 상태 인경우 현재 상태가 Pressed인지, Moved인지 체크해서 업데이트
		updateState();
		return;
	}

	m_eState = eDisabled;
	setVisibleStateNormal();
}

void UIScrollBar::setLinkElement(UIElement* element) {
	if (m_pLink != nullptr) {
		m_pLink->removeMouseEvent(eMouseEventScroll, CC_CALLBACK_1(UIScrollBar::onLinkElementMouseScroll, this));
	}

	m_pLink = element;
	m_pLink->addMouseEvent(eMouseEventScroll, CC_CALLBACK_1(UIScrollBar::onLinkElementMouseScroll, this));
}

void UIScrollBar::onLinkElementMouseScroll(SGEventMouse* mouseEvent) {
	if (m_pLink == nullptr) {
		_LogWarn_("링크 엘리먼트가 없는데 여기 들어온다고?");
		return;
	}

	if (mouseEvent->getScrollY() < 0) {
		setRowPos(m_iPos - 1);
	} else {
		setRowPos(m_iPos + 1);
	}
}

void UIScrollBar::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleStateNormal();
}

bool UIScrollBar::init() {
	const ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	_contentSize = m_pInfo->TrackSize + Size{0, m_fUpButtonHeight + m_fDownButtonHeight }; // 위 아래 버튼높이 임시 추가 (이미지가 전혀 없더라도 스크롤바 기능이 가능토록. 하기위함)

	if (pPack == nullptr) {
		_LogWarn_("스크롤바 Sga패키지를 찾지 못했습니다.");
		return false;
	}

	if (m_pInfo->TrackSize.height < MinThumbHeight) {
		_LogWarn_("스크롤바 손잡이 사이즈가 (%.f)보다 작습니다. (손잡이가 업/다운 버튼을 침범할 수 있습니다.)", MinThumbHeight);
	}

	const SgaSpriteAbstractPtr spSpriteUpNormal = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[IndexUpNormal]);
	const SgaSpriteAbstractPtr spSpriteUpPressed = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[IndexUpPressed]);
	const SgaSpriteAbstractPtr spSpriteDownNormal = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[IndexDownNormal]);
	const SgaSpriteAbstractPtr spSpriteDownPressed = pPack->getSpriteUnsafe(m_pInfo->Img, m_pInfo->Sprites[IndexDownPressed]);

	m_fUpButtonHeight = SgaSpriteHelper::GetMaxHeightF(spSpriteUpNormal, spSpriteUpPressed);
	m_fDownButtonHeight = SgaSpriteHelper::GetMaxHeightF(spSpriteDownNormal, spSpriteDownPressed);

	if (m_fUpButtonHeight == 0)
		m_fUpButtonHeight = DefaultSize15.height;

	if (m_fDownButtonHeight == 0)
		m_fDownButtonHeight = DefaultSize15.height;

	_contentSize = m_pInfo->TrackSize + SGSize{ 0, m_fUpButtonHeight + m_fDownButtonHeight };
	m_fTrackHeight = m_pInfo->TrackSize.height;
	return true;
}

void UIScrollBar::load() {
	if (m_bLoaded)
		return;

	

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
	const FrameTexture* pDefaultTexture = CoreGlobal_v->getDefaultFrameTexture();

	// 업 버튼
	if (m_pTexture[IndexUpNormal] == pDefaultTexture)
		m_pSprite[IndexUpNormal]->setColor(SGColorList::Acidgreen_v);

	if (m_pTexture[IndexUpPressed] == pDefaultTexture)
		m_pSprite[IndexUpPressed]->setColor(SGColorList::CGred_v);

	// 다운 버튼
	if (m_pTexture[IndexDownNormal] == pDefaultTexture)
		m_pSprite[IndexDownNormal]->setColor(SGColorList::Acidgreen_v);

	if (m_pTexture[IndexDownPressed] == pDefaultTexture)
		m_pSprite[IndexDownPressed]->setColor(SGColorList::CGred_v);

	// 손잡이
	if (m_pTexture[IndexThumbNormal] == pDefaultTexture)
		m_pSprite[IndexThumbNormal]->setColor(SGColorList::Aeroblue_v);

	if (m_pTexture[IndexThumbPressed] == pDefaultTexture)
		m_pSprite[IndexThumbPressed]->setColor(SGColorList::Alabaster_v);

	setVisibleStateNormal();
	m_bLoaded = true;
}


void UIScrollBar::createSprites() {
	ImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	const bool bPackIsNull = pPack == nullptr;
	FrameTexture* pDefaultTexture = CoreGlobal_v->getDefaultFrameTexture();

	for (int i = 0; i < TextureCount; ++i) {
		FrameTexture* pTexture = bPackIsNull || m_pInfo->Sprites[i] == InvalidValue_v ? pDefaultTexture : pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprites[i]);
		pTexture->retain();

		Sprite* pSprite;

		// 나인 렉트의 중앙 렉트 크기는 setCapInsets로 설정할 수 있다.
		if (i == IndexThumbNormal || i == IndexThumbPressed) {
			SpriteFrame* pFrame = SpriteFrame::createWithTexture(pTexture->getTexture(), pTexture->getRect());
			SGScale9Sprite* pScale9 = SGScale9Sprite::createWithSpriteFrame(pFrame);
			pSprite = pScale9;

			// 높이는 일단 암거나 지정, 어차피 이후 updateThumbSize() 함수에서 업데이트를 하기 때문..
			pSprite->setContentSize({ _contentSize.width, DefaultSize45.height }); 
			
		} else {
			const SGSize size = pTexture->getSize();
			const float fWidthScaleX = _contentSize.width / size.width;

			pSprite = Sprite::createWithTexture(pTexture->getTexture());
			pSprite->setScaleX(fWidthScaleX);
		}

		pSprite->setAnchorPoint(Vec2::ZERO);

		m_pTexture[i] = pTexture;
		m_pSprite[i] = pSprite;
	}

	// 높이의 경우 트랙과 손잡이만 contentSize.height에 영향을 받으므로 스케일을 변경해줘야한다.
	// 손잡이의 경우 Scale9 스프이고 updateThumbSize()에서 수행하므로 트랙만 스케일 적용
	m_pSprite[IndexTrack]->setScaleY(m_fTrackHeight / m_pTexture[IndexTrack]->getHeightF());

	this->addChild(m_pSprite[IndexUpNormal]);
	this->addChild(m_pSprite[IndexUpPressed]);
	this->addChild(m_pSprite[IndexDownNormal]);
	this->addChild(m_pSprite[IndexDownPressed]);
	this->addChild(m_pSprite[IndexTrack]);
	this->addChild(m_pSprite[IndexThumbNormal]);
	this->addChild(m_pSprite[IndexThumbPressed]);
}

void UIScrollBar::updateTrackAndButtonPosition() {
	// 업 버튼
	m_pSprite[IndexUpNormal]->setPosition(0, m_fDownButtonHeight + m_fTrackHeight);
	m_pSprite[IndexUpPressed]->setPosition(0, m_fDownButtonHeight + m_fTrackHeight);

	// m_pSprite[IndexDownNormal]->setPosition(0, 0);
	// m_pSprite[IndexDownPressed]->setPosition(0, 0);

	// 트랙
	m_pSprite[IndexTrack]->setPosition(0, m_fDownButtonHeight);

}

void UIScrollBar::updateThumbSize() {
	const float fSplitedHeight = m_fTrackHeight / m_iRowCount;
	const float fThumbHeight = Math::Max(fSplitedHeight * m_iRowCountPerPage, MinThumbHeight);

	// 스케일스프는 콘텐츠 사이즈로 변경해야함
	m_fThumbHeight = fThumbHeight;
	m_pSprite[IndexThumbNormal]->setContentSize({ _contentSize.width, fThumbHeight });
	m_pSprite[IndexThumbPressed]->setContentSize({ _contentSize.width, fThumbHeight });
}

void UIScrollBar::updateThumbPosition() {
	// 손잡이 X 위치
	// m_pSprite[IndexThumbNormal]->setPositionX(0);
	// m_pSprite[IndexThumbPressed]->setPositionX(0);

	const float fSplitedHeight = m_fTrackHeight / m_iRowCount;
	const float fThumbHeight = Math::Max(fSplitedHeight * m_iRowCountPerPage, MinThumbHeight);
	const float fLeftTrackHeight = m_fTrackHeight - fThumbHeight;

	m_fSplitedTrackHeight = m_iEndPos != 0 ? fLeftTrackHeight / m_iEndPos : 0;
	m_fThumbHeight = fThumbHeight;
	m_pSprite[IndexThumbNormal]->setPositionY(m_fDownButtonHeight + m_fSplitedTrackHeight * (float)(m_iEndPos - m_iPos));
	m_pSprite[IndexThumbPressed]->setPositionY(m_fDownButtonHeight + m_fSplitedTrackHeight * (float)(m_iEndPos - m_iPos));
}

void UIScrollBar::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[eNormal]);
	}

	m_bLoaded = false;
}



void UIScrollBar::setRowPos(int pos) {
	if (pos == m_iPos)
		return;

	const int prev = m_iPos;

	if (pos <= 0) {
		m_iPos = 0;
	} else if (pos > m_iEndPos) {
		m_iPos = m_iEndPos;
	} else {
		m_iPos = pos;
	}

	m_pMasterGroup->onScrollBarPositionChanged(this, prev, m_iPos);
	updateThumbPosition();
}

void UIScrollBar::setRowCount(int count) {
	if (count < m_iRowCountPerPage) {
		m_iRowCount = m_iRowCountPerPage;
		_LogWarn_("스크롤바의 RowCount를 %d보다 낮출려는 시도가 있었습니다. (%d로 설정합니다.)", m_iRowCountPerPage, m_iRowCountPerPage);
	}

	m_iRowCount = count;
	m_iEndPos = m_iRowCount - m_iRowCountPerPage;
	updateThumbSize();
	updateThumbPosition();
}

void UIScrollBar::setRowCountPerPage(int count) {
	if (count <= 0) {
		m_iRowCountPerPage = 1;
		_LogWarn_("스크롤바의 m_iRowCountPerPage를 0으로 설정할려는 시도가 있었습니다. (1로 설정합니다.)");
	}

	m_iRowCountPerPage = count;
	m_iEndPos = m_iRowCount - m_iRowCountPerPage;

	if (m_iPos > m_iEndPos)
		m_iPos = m_iEndPos;

	updateThumbSize();
	updateThumbPosition();
}

void UIScrollBar::setContentSize(const SGSize& contentSize) {
	if (!m_bResizable)
		return;

	_contentSize = contentSize;
	m_fTrackHeight = contentSize.height - m_fDownButtonHeight - m_fUpButtonHeight;

	if (!m_bLoaded)
		return;

	for (int i = 0; i < TextureCount; ++i) {
		FrameTexture* pTexture = m_pTexture[i];
		Sprite* pSprite = m_pSprite[i];

		if (pTexture == nullptr || pSprite == nullptr) {
			continue;
		}

		// Scale9 스프이므로 updateThumbSize()에서 업데이트함
		if (i == IndexThumbNormal || i == IndexThumbPressed) {
			continue;
		}

		pSprite->setScaleX(_contentSize.width / m_pTexture[i]->getWidthF());
	}

	m_pSprite[IndexTrack]->setScaleY(m_fTrackHeight / m_pTexture[IndexTrack]->getHeightF());

	updateTrackAndButtonPosition();
	updateThumbSize();
	updateThumbPosition();
}


bool UIScrollBar::isUpButtonContainPoint(SGVec2 pos) {
	const SGVec2 upButtonPos = m_pSprite[IndexUpNormal]->getPosition();
	const Rect upButtonBoundingBox = { _position + upButtonPos, SGSize{ _contentSize.width, m_fUpButtonHeight } };
	return upButtonBoundingBox.containsPoint(pos);
}

bool UIScrollBar::isDownButtonContainPoint(SGVec2 pos) {
	const SGVec2 downButtonPos = m_pSprite[IndexDownNormal]->getPosition();
	const Rect downButtonBoundingBox = { _position + downButtonPos, SGSize{ _contentSize.width, m_fDownButtonHeight } };
	return downButtonBoundingBox.containsPoint(pos);
}

bool UIScrollBar::isThumbButtonContainPoint(SGVec2 pos) {
	const SGVec2 thumbButtonPos = m_pSprite[IndexThumbNormal]->getPosition();
	const Rect thumbButtonBoundingBox = { _position + thumbButtonPos, m_pSprite[IndexThumbNormal]->getContentSize() };
	return thumbButtonBoundingBox.containsPoint(pos);
}


void UIScrollBar::onMouseLeaveDetail(SGEventMouse* mouseEvent) {
	//setVisibleStateNormal();
}

bool UIScrollBar::onMouseMoveDetail(SGEventMouse* mouseEvent) {
	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	if (!m_bDragBegin)
		return true;

	// 손잡이 드래그중인경우
	const float fThumbDragYPos = cursorPos.y - _position.y - m_fDownButtonHeight;
	const float fDragDelta = m_fDragStartYPos - fThumbDragYPos;
	const int iPosDelta = int(fDragDelta / m_fSplitedTrackHeight);
	setRowPos(m_iDragStartPos + iPosDelta);
	return false;
}
bool UIScrollBar::onMouseDownDetail(SGEventMouse* mouseEvent) {
	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	const bool bUpButtonContained = isUpButtonContainPoint(cursorPos);
	const bool bDownButtonContained = isDownButtonContainPoint(cursorPos);
	const bool bThumbButtonContained = isThumbButtonContainPoint(cursorPos);

	m_pSprite[IndexUpPressed]->setVisible(bUpButtonContained);
	m_pSprite[IndexUpNormal]->setVisible(!bUpButtonContained);

	m_pSprite[IndexDownPressed]->setVisible(bDownButtonContained);
	m_pSprite[IndexDownNormal]->setVisible(!bDownButtonContained);

	m_pSprite[IndexThumbPressed]->setVisible(bThumbButtonContained);
	m_pSprite[IndexThumbNormal]->setVisible(!bThumbButtonContained);

	m_bDragBegin = bThumbButtonContained;
	m_fDragStartYPos = cursorPos.y - _position.y - m_fDownButtonHeight;
	m_iDragStartPos = m_iPos;
	return false;
}

void UIScrollBar::onMouseUpDetail(SGEventMouse* mouseEvent) {
	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	setVisibleStateNormal();
	m_bDragBegin = false;

	if (isUpButtonContainPoint(cursorPos)) {
		setRowPos(m_iPos - 1);
		m_pMasterGroup->onScrollBarUpButtonPressed(this, m_iPos);
	} else if (isDownButtonContainPoint(cursorPos)) {
		setRowPos(m_iPos + 1);
		m_pMasterGroup->onScrollBarDownButtonPressed(this, m_iPos);
	} else if (isThumbButtonContainPoint(cursorPos)) {
		m_pMasterGroup->onScrollBarThumbButtonPressed(this, m_iPos);
	}
}

bool UIScrollBar::onMouseScrollDetail(SGEventMouse* mouseEvent) {

	if (mouseEvent->getScrollY() < 0) {
		setRowPos(m_iPos - 1);
	} else {
		setRowPos(m_iPos + 1);
	}

	return true;
}
