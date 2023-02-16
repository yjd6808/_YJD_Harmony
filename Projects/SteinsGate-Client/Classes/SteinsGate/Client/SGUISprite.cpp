/*
 * 작성자: 윤정도
 * 생성일: 2/16/2023 5:54:22 PM
 * =====================
 *
 */



#include "Tutturu.h"
#include "SGUISprite.h"

#include <SteinsGate/Client/SGImagePackManager.h>
#include <SteinsGate/Client/SGUIManager.h>

USING_NS_CC;
USING_NS_JC;

SGUISprite::SGUISprite(SGUIGroup* parent, SGUISpriteInfo* btnInfo)
	: SGUIElement(parent)
	, m_pInfo(btnInfo)
	, m_pTexture{}
	, m_pSprite{}
{}

SGUISprite::~SGUISprite() {
	CC_SAFE_RELEASE(m_pTexture);
}

SGUISprite* SGUISprite::create(SGUIGroup* parent, SGUISpriteInfo* spriteInfo) {
	SGUISprite* pSprite = dbg_new SGUISprite(parent, spriteInfo);
	pSprite->autorelease();
	return pSprite;
}

bool SGUISprite::init() {

	return true;
}

void SGUISprite::load() {
	if (m_bLoaded)
		return;

	SGImagePackManager* pPackManager = SGImagePackManager::get();
	SGUIManager* pUIManager = SGUIManager::get();
	SGImagePack* pPack = pPackManager->getPack(m_pInfo->Npk);

	m_pTexture = pPack->createFrameTexture(m_pInfo->Img, m_pInfo->Sprite);
	m_pTexture->retain();

	pUIManager->registerLoadedUITexture({ m_pInfo->Npk, m_pInfo->Img, m_pInfo->Sprite });

	DebugAssertMsg(!m_pTexture->isLink(), "스프라이트의 텍스쳐가 링크 텍스쳐입니다. 그래선 안됩니다.");

	m_pSprite = Sprite::create();
	m_pSprite->initWithTexture(m_pTexture->getTexture());
	m_pSprite->setAnchorPoint(Vec2::ZERO);
	this->addChild(m_pSprite);

	setContentSize(m_pSprite->getContentSize());
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



int SGUISprite::getCode() {
	return m_pInfo->Code;
}
