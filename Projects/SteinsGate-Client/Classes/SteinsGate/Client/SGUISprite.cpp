/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:22 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGUISprite.h"

#include <SteinsGate/Client/SGUIMasterGroup.h>

USING_NS_CC;
USING_NS_JC;

SGUISprite::SGUISprite(SGUIMasterGroup* master, SGUIGroup* parent, SGUISpriteInfo* staticInfo)
	: SGUIElement(master, parent, staticInfo)
	, m_pInfo(staticInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

SGUISprite::~SGUISprite() {
	CC_SAFE_RELEASE(m_pTexture);
}

SGUISprite* SGUISprite::create(SGUIMasterGroup* master, SGUIGroup* parent, SGUISpriteInfo* spriteInfo) {
	SGUISprite* pSprite = dbg_new SGUISprite(master, parent, spriteInfo);
	pSprite->init();
	pSprite->autorelease();
	return pSprite;
}

bool SGUISprite::init() {
	setContentSize(m_pInfo->Size);
	return true;
}

void SGUISprite::load() {
	if (m_bLoaded)
		return;

	SGImagePack* pPack = CorePackManager_v->getPackUnsafe(m_pInfo->Sga);

	if (pPack == nullptr)
		return;

	m_pTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprite, m_pInfo->LinearDodge);
	m_pTexture->retain();

	if (m_pTexture->isLink()) {
		CC_SAFE_RELEASE_NULL(m_pTexture);
		return;
	}

	CoreUIManager_v->registerLoadedUITexture({ m_pInfo->Sga, m_pInfo->Img, m_pInfo->Sprite });

	const Size spriteSize = m_pTexture->getSize();
	const float fScaleX = m_pInfo->Size.width / spriteSize.width;
	const float fScaleY = m_pInfo->Size.height / spriteSize.height;

	m_pSprite = Sprite::create();
	m_pSprite->initWithTexture(m_pTexture->getTexture());
	m_pSprite->setAnchorPoint(Vec2::ZERO);
	m_pSprite->setScale(fScaleX, fScaleY);
	this->addChild(m_pSprite);
	m_bLoaded = true;
}

void SGUISprite::unload() {
	if (m_bLoaded == false)
		return;

	removeAllChildren(); // autorelease 되기땜
	m_pSprite = nullptr;
	CC_SAFE_RELEASE_NULL(m_pTexture);
	m_bLoaded = false;
}


