/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 10:30:26 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "CharacterListener.h"

//////////////////////////////////////////////////////////////////////////////////////////
void CharacterListener::OnCleanUp()
{
	IActorListener::OnCleanUp();

	if (pChar_->HasCleanUpFlag(Actor::cfReleaseActorSprite))
	{
		pChar_->ReleaseActorSprite();
	}
}
