/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/PhysicsActor.h>
#include <SteinsGate/Client/CharInfo.h>

class Character : public PhysicsActor
{
public:
	Character();
	Character(int code);
	~Character() override;

	static Character* create(int code, const VisualInfo& info);

	void initActorId();
	void initInfo(int code, const VisualInfo& visualInfo);
	void initBaseInfo(int code);
	void initActorSpriteData(const VisualInfo& visualInfo);
	void initActorSprite() override;
	void initListener();
	void initComponents() override;
	void hit(const HitInfo& hitInfo) override;

	void update(float dt) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;

	CharBaseInfo* getBaseInfo();
	int getCode() override { return m_pBaseInfo->Code; }
protected:
	CharBaseInfo* m_pBaseInfo;
	ActorSpriteData* m_pSpriteData;
};


