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
	Character(int code, const VisualInfo& info);
	~Character() override;

	static Character* create(int code, const VisualInfo& info);

	void initActorId();
	void initInfo(int code, const VisualInfo& visualInfo);
	void initVisualInfo(const VisualInfo& visualInfo);
	void initBaseInfo(int code);

	void initActorSprite() override;
	void initListener(ActorListener* listener) override;
	void hit(const SGHitInfo& hitInfo) override;

	void update(float dt) override;
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture) override;
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture) override;
	void cleanUpImmediate();

	CharBaseInfo* getBaseInfo();
private:
	CharBaseInfo* m_pBaseInfo;
	VisualInfo m_VisualInfo;
};


