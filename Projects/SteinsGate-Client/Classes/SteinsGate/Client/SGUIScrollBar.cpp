/*
 * 작성자: 윤정도
 * 생성일: 3/22/2023 2:23:20 AM
 * =====================
 *
 */


#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUIScrollBar.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>
#include <SteinsGate/Common/SGRectEx.h>

USING_NS_CC;
USING_NS_JC;

SGUIScrollBar::SGUIScrollBar(SGUIMasterGroup* master, SGUIGroup* parent, SGUIScrollBarInfo* scrollBarInfo)
	: SGUIElement(master, parent, scrollBarInfo)
	, m_iRowCount(30)
	, m_iRowCountPerPage(4)
	, m_iPos(10)
	, m_iEndPos(m_iRowCount - m_iRowCountPerPage)
	, m_fUpButtonHeight(DefaultSize15.height)
	, m_fDownButtonHeight(DefaultSize15.height)
	, m_pLink(nullptr)
	, m_pInfo(scrollBarInfo)
	, m_pTexture{}
	, m_pSprite{}
	, m_bDragBegin(false)
{}

SGUIScrollBar* SGUIScrollBar::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUIScrollBarInfo* scrollBarInfo) {
	SGUIScrollBar* pScrollBar = dbg_new SGUIScrollBar(master, parent, scrollBarInfo);
	pScrollBar->init();
	pScrollBar->autorelease();
	return pScrollBar;
}

SGUIScrollBar::~SGUIScrollBar() {
	for (int i = 0; i < TextureCount; ++i) {
		CC_SAFE_RELEASE(m_pTexture[i]);
	}
}



// 스크롤바 버튼들은 비활성화, 마우스 오버가 없으므로.. 비활성상태를 노말로 처리한다.
void SGUIScrollBar::setVisibleStateNormal() {
	m_pSprite[IndexUpNormal]->setVisible(true);
	m_pSprite[IndexDownNormal]->setVisible(true);
	m_pSprite[IndexThumbNormal]->setVisible(true);
	m_pSprite[IndexUpPressed]->setVisible(false);
	m_pSprite[IndexDownPressed]->setVisible(false);
	m_pSprite[IndexThumbPressed]->setVisible(false);
}

void SGUIScrollBar::setEnabled(bool enabled) {

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

void SGUIScrollBar::setLinkElement(SGUIElement* element) {
	if (m_pLink != nullptr) {
		m_pLink->removeMouseEvent(eMouseEventScroll, CC_CALLBACK_1(SGUIScrollBar::onLinkElementMouseScroll, this));
	}

	m_pLink = element;
	m_pLink->addMouseEvent(eMouseEventScroll, CC_CALLBACK_1(SGUIScrollBar::onLinkElementMouseScroll, this));
}

void SGUIScrollBar::onLinkElementMouseScroll(SGEventMouse* mouseEvent) {
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

void SGUIScrollBar::restoreState(State state) {
	if (m_eState == eDisabled)
		return;

	if (m_eState == state)
		setVisibleStateNormal();
}

bool SGUIScrollBar::init() {
	const SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	setContentSize(m_pInfo->TrackSize + SGSize{0, m_fUpButtonHeight + m_fDownButtonHeight }); // 위 아래 버튼높이 임시 추가 (이미지가 전혀 없더라도 스크롤바 기능이 가능토록. 하기위함)

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

	setContentSize(m_pInfo->TrackSize + SGSize{ 0, m_fUpButtonHeight + m_fDownButtonHeight });
	return true;
}

void SGUIScrollBar::load() {
	if (m_bLoaded)
		return;

	

	createSprites();

	// =============================================================
	//                        사이즈 스케일링
	// 1. 영구적으로 변하지 않는 컨트롤의 너비와 높이를 결정한다.
	// 2. 손잡이의 경우 너비는 트랙 너비와 동일하게 변하지 않는다. 하지만 높이의 경우 로카운트에따라 변한다.
	// =============================================================

	// 업 버튼
	const SGSize upNormalSize = m_pSprite[IndexUpNormal]->getContentSize();
	const SGSize upPressedSize = m_pSprite[IndexUpPressed]->getContentSize();

	m_pSprite[IndexUpNormal]->setScaleX(m_pInfo->TrackSize.width / upNormalSize.width);
	m_pSprite[IndexUpPressed]->setScaleX(m_pInfo->TrackSize.width / upPressedSize.width);

	m_pSprite[IndexUpNormal]->setScaleY(m_fUpButtonHeight / upNormalSize.height);
	m_pSprite[IndexUpPressed]->setScaleY(m_fUpButtonHeight / upPressedSize.height);

	// 다운 버튼
	const SGSize downNormalSize = m_pSprite[IndexDownNormal]->getContentSize();
	const SGSize downPressedSize = m_pSprite[IndexDownPressed]->getContentSize();

	m_pSprite[IndexDownNormal]->setScaleX(m_pInfo->TrackSize.width / downNormalSize.width);
	m_pSprite[IndexDownPressed]->setScaleX(m_pInfo->TrackSize.width / downPressedSize.width);

	m_pSprite[IndexDownNormal]->setScaleY(m_fDownButtonHeight / downNormalSize.height);
	m_pSprite[IndexDownPressed]->setScaleY(m_fDownButtonHeight / downPressedSize.height);

	// 트랙
	const SGSize trackSize = m_pSprite[IndexTrack]->getContentSize();

	m_pSprite[IndexTrack]->setScaleX(m_pInfo->TrackSize.width / trackSize.width);
	m_pSprite[IndexTrack]->setScaleY(m_pInfo->TrackSize.height / trackSize.height);

	// 손잡이 너비는 평생 고정
	const SGSize thumbNormalSize = m_pSprite[IndexThumbNormal]->getContentSize();
	const SGSize thumbPressedSize = m_pSprite[IndexThumbPressed]->getContentSize();

	m_pSprite[IndexThumbNormal]->setScaleX(m_pInfo->TrackSize.width / thumbNormalSize.width);
	m_pSprite[IndexThumbPressed]->setScaleX(m_pInfo->TrackSize.width / thumbPressedSize.width);

	// 손잡이 높이 업데이트: 현재 로카운트 기준으로 높이 결정
	updateThumbSize();


	// =============================================================
	//                        포지셔닝
	// 1. 영구적으로 변하지 않는 컨트롤의 위치를 결정한다.
	// 2. 손잡이의 경우 위치가 로카운트, 로포스에 따라 변한다.
	// 이때 모든 컨트롤의 X좌표는 0으로 고정되므로 Y좌표만 신경써주면 된다.
	// =============================================================

	// 업 버튼
	m_pSprite[IndexUpNormal]->setPosition(0, m_fDownButtonHeight + m_pInfo->TrackSize.height);
	m_pSprite[IndexUpPressed]->setPosition(0, m_fDownButtonHeight + m_pInfo->TrackSize.height);

	// 다운 버튼 (어차피 0이므로 생략)
	// m_pSprite[IndexDownNormal]->setPosition(0, 0);
	// m_pSprite[IndexDownPressed]->setPosition(0, 0);

	// 트랙
	m_pSprite[IndexTrack]->setPosition(0, m_fDownButtonHeight);

	// 손잡이 X 위치
	// m_pSprite[IndexThumbNormal]->setPositionX(0);
	// m_pSprite[IndexThumbPressed]->setPositionX(0);
	

	// 손잡이 Y 위치업데이트
	updateThumbPosition();

	// =============================================================
	//                       디폴트 컬러링
	// 디폴트 텍스쳐인 경우 업버튼, 다운버튼, 손잡이, 트랙 색을 지정해서 구분 가능토록 만든다.
	// =============================================================
	const SGFrameTexture* pDefaultTexture = CoreGlobal_v->getDefaultFrameTexture();

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


void SGUIScrollBar::createSprites() {
	SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);
	const bool bPackIsNull = pPack == nullptr;
	SGFrameTexture* pDefaultTexture = CoreGlobal_v->getDefaultFrameTexture();

	for (int i = 0; i < TextureCount; ++i) {
		SGFrameTexture* pTexture = bPackIsNull || m_pInfo->Sprites[i] == InvalidValue_v ? pDefaultTexture : pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprites[i]);
		pTexture->retain();

		Sprite* pSprite;

		// 나인 렉트의 중앙 렉트 크기는 setCapInsets로 설정할 수 있다.
		if (i == IndexThumbNormal || i == IndexThumbPressed) {
			SpriteFrame* pFrame = SpriteFrame::createWithTexture(pTexture->getTexture(), pTexture->getRect());
			SGScale9Sprite* pScale9 = SGScale9Sprite::createWithSpriteFrame(pFrame);
			pSprite = pScale9;
			pSprite->setContentSize({ m_pInfo->TrackSize.width, DefaultSize45.height }); // 높이는 일단 암거나 지정, 어차피 업데이트 할거라
			
		} else {
			const SGSize size = pTexture->getSize();
			const float fWidthScaleX = m_pInfo->TrackSize.width / size.width;

			pSprite = Sprite::createWithTexture(pTexture->getTexture());
			pSprite->setScaleX(fWidthScaleX);
			pSprite->setContentSize({ size.width * fWidthScaleX, size.height });
		}

		pSprite->setAnchorPoint(Vec2::ZERO);

		m_pTexture[i] = pTexture;
		m_pSprite[i] = pSprite;
	}

	this->addChild(m_pSprite[IndexUpNormal]);
	this->addChild(m_pSprite[IndexUpPressed]);
	this->addChild(m_pSprite[IndexDownNormal]);
	this->addChild(m_pSprite[IndexDownPressed]);
	this->addChild(m_pSprite[IndexTrack]);
	this->addChild(m_pSprite[IndexThumbNormal]);
	this->addChild(m_pSprite[IndexThumbPressed]);
}

void SGUIScrollBar::updateThumbSize() {
	const float fSplitedHeight = m_pInfo->TrackSize.height / m_iRowCount;
	const float fThumbHeight = Math::Max(fSplitedHeight * m_iRowCountPerPage, MinThumbHeight);

	// 스케일스프는 콘텐츠 사이즈로 변경해야함
	m_fThumbHeight = fThumbHeight;
	m_pSprite[IndexThumbNormal]->setContentSize({ m_pInfo->TrackSize.width, fThumbHeight });
	m_pSprite[IndexThumbPressed]->setContentSize({ m_pInfo->TrackSize.width, fThumbHeight });
}

void SGUIScrollBar::updateThumbPosition() {
	const float fSplitedHeight = m_pInfo->TrackSize.height / m_iRowCount;
	const float fThumbHeight = Math::Max(fSplitedHeight * m_iRowCountPerPage, MinThumbHeight);
	const float fLeftTrackHeight = m_pInfo->TrackSize.height - fThumbHeight;

	m_fSplitedTrackHeight = m_iEndPos != 0 ? fLeftTrackHeight / m_iEndPos : 0;
	m_fThumbHeight = fThumbHeight;
	m_pSprite[IndexThumbNormal]->setPositionY(m_fDownButtonHeight + m_fSplitedTrackHeight * (float)(m_iEndPos - m_iPos));
	m_pSprite[IndexThumbPressed]->setPositionY(m_fDownButtonHeight + m_fSplitedTrackHeight * (float)(m_iEndPos - m_iPos));
}

void SGUIScrollBar::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜

	for (int i = 0; i < TextureCount; ++i) {
		m_pSprite[i] = nullptr;
		CC_SAFE_RELEASE_NULL(m_pTexture[eNormal]);
	}

	m_bLoaded = false;
}



void SGUIScrollBar::setRowPos(int pos) {
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

void SGUIScrollBar::setRowCount(int count) {
	if (count < m_iRowCountPerPage) {
		m_iRowCount = m_iRowCountPerPage;
		_LogWarn_("스크롤바의 RowCount를 %d보다 낮출려는 시도가 있었습니다. (%d로 설정합니다.)", m_iRowCountPerPage, m_iRowCountPerPage);
	}

	m_iRowCount = count;
	m_iEndPos = m_iRowCount - m_iRowCountPerPage;
	updateThumbSize();
	updateThumbPosition();
}

void SGUIScrollBar::setRowCountPerPage(int count) {
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


bool SGUIScrollBar::isUpButtonContainPoint(SGVec2 pos) {
	const SGVec2 upButtonPos = m_pSprite[IndexUpNormal]->getPosition();
	const Rect upButtonBoundingBox = { _position + upButtonPos, m_pSprite[IndexUpNormal]->getContentSize() };
	return upButtonBoundingBox.containsPoint(pos);
}

bool SGUIScrollBar::isDownButtonContainPoint(SGVec2 pos) {
	const SGVec2 downButtonPos = m_pSprite[IndexDownNormal]->getPosition();
	const Rect downButtonBoundingBox = { _position + downButtonPos, m_pSprite[IndexDownNormal]->getContentSize() };
	return downButtonBoundingBox.containsPoint(pos);
}

bool SGUIScrollBar::isThumbButtonContainPoint(SGVec2 pos) {
	const SGVec2 thumbButtonPos = m_pSprite[IndexThumbNormal]->getPosition();
	const Rect thumbButtonBoundingBox = { _position + thumbButtonPos, m_pSprite[IndexThumbNormal]->getContentSize() };
	return thumbButtonBoundingBox.containsPoint(pos);
}


void SGUIScrollBar::onMouseLeaveDetail(SGEventMouse* mouseEvent) {
	//setVisibleStateNormal();
}

bool SGUIScrollBar::onMouseMoveDetail(SGEventMouse* mouseEvent) {
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
bool SGUIScrollBar::onMouseDownDetail(SGEventMouse* mouseEvent) {
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

void SGUIScrollBar::onMouseUpDetail(SGEventMouse* mouseEvent) {
	const SGVec2 cursorPos = mouseEvent->getCursorPos();

	setVisibleStateNormal();
	m_bDragBegin = false;

	if (isUpButtonContainPoint(cursorPos)) {
		setRowPos(m_iPos - 1);
		m_pMasterGroup->onScrollBarUpButtonPressed(this, m_iPos);
	}
	else if (isDownButtonContainPoint(cursorPos)) {
		setRowPos(m_iPos + 1);
		m_pMasterGroup->onScrollBarDownButtonPressed(this, m_iPos);
	}
	else if (isThumbButtonContainPoint(cursorPos)) {
		m_pMasterGroup->onScrollBarThumbButtonPressed(this, m_iPos);
	}
}

bool SGUIScrollBar::onMouseScrollDetail(SGEventMouse* mouseEvent) {

	if (mouseEvent->getScrollY() < 0) {
		setRowPos(m_iPos - 1);
	} else {
		setRowPos(m_iPos + 1);
	}

	return true;
}
