/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGActor.h>
#include <SteinsGate/Research/SGDataManager.h>

class SGCharacter : public SGActor
{
public:
	SGCharacter(int code, const SGCharacterInfo& info);
	static SGCharacter* create(int code, const SGCharacterInfo& info);
	void initActorSprite() override;

	void update(float dt) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;

	SGCharacterBaseInfo* getBaseInfo();
private:
	SGCharacterBaseInfo* m_pBaseInfo;
	SGCharacterInfo m_CharacterInfo;
};


