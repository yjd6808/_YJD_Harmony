/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */


#include "SGCharacter.h"

#include  <SteinsGate/Research/SGImagePackManager.h>

USING_NS_JC;
USING_NS_CC;

SGCharacter::SGCharacter(int code, const SGCharacterInfo& info)
	: SGActor(ActorType::Character, code)
	, m_CharacterInfo(info)
{
}

SGCharacter* SGCharacter::create(int code, const SGCharacterInfo& info) {
	SGCharacter* pCharacter = new SGCharacter(code, info);

	if (pCharacter && pCharacter->init()) {
		SGCharacterBaseInfo* pBaseInfo = SGConfigManager::getInstance()->getCharacterBaseInfo(code);
		pCharacter->m_pBaseInfo = pBaseInfo;
		pCharacter->initThicknessBox({ pBaseInfo->ThicknessBoxWidth, pBaseInfo->ThicknessBoxHeight, pBaseInfo->ThicknessBoxRelativeY });
		pCharacter->initActorSprite();
		pCharacter->autorelease();
		return pCharacter;
	}

	return pCharacter;
}

void SGCharacter::initActorSprite() {
	SGConfigManager* pConfig = SGConfigManager::getInstance();
	SGImagePackManager* pImgPackManager = SGImagePackManager::getInstance();
	
	SGActorSpriteDataPtr spActorSpriteData = MakeShared<SGActorSpriteData>();

	for (int i = 0; i < VisualType::Max; ++i) {
		if (m_CharacterInfo.VisualInfo.ImgIndex[i] != InvalidIndex_v &&
			m_CharacterInfo.VisualInfo.NpkIndex[i] != InvalidIndex_v) {
			spActorSpriteData->Parts.PushBack({
				m_pBaseInfo->DefaultVisualZOrder[i],
				m_CharacterInfo.VisualInfo.NpkIndex[i],
				m_CharacterInfo.VisualInfo.ImgIndex[i]
			});
		}
	}

	SGVector<int>& vActions = m_CharacterInfo.ValidAction;

	for (int i = 0; i < vActions.Size(); ++i) {
		SGActionInfo* pActionInfo = pConfig->getActionInfo(vActions[i]);
		for (int j = 0; j < pActionInfo->Animations.Size(); ++j) {
			spActorSpriteData->Animations.PushBack(&pActionInfo->Animations[j]);
		}
	}

	BB;
}

