/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGPhysicsActor.h>
#include <SteinsGate/Research/SGCharacterBaseInfo.h>

class SGCharacter : public SGPhysicsActor
{
public:
	SGCharacter(int code, const SGCharacterInfo& info, SGMapLayer* mapLayer);
	static SGCharacter* create(int code, const SGCharacterInfo& info, SGMapLayer* mapLayer);
	void initActorSprite() override;
	void hit(const SGHitInfo& hitInfo) override;

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


