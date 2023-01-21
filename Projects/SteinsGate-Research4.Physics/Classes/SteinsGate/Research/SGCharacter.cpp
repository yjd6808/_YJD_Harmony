/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:30:07 AM
 * =====================
 *
 */


#include "SGCharacter.h"


SGCharacter::SGCharacter(int code)
	: SGActor(ActorType::Character, code)
{
}

SGCharacter* SGCharacter::create(int code) {
	SGCharacter* pCharacter = new SGCharacter(code);

	if (pCharacter && pCharacter->init()) {
		SGCharacterBaseInfo* pBaseInfo = SGConfigManager::getInstance()->getCharacterBaseInfo(code);
		pCharacter->m_pBaseInfo = pBaseInfo;
		pCharacter->initThicknessBox({ pBaseInfo->ThicknessBoxWidth, pBaseInfo->ThicknessBoxHeight, pBaseInfo->ThicknessBoxRelativeY });
		pCharacter->autorelease();
		return pCharacter;
	}

	return pCharacter;
}
