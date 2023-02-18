/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGPhysicsActor.h>
#include <SteinsGate/Client/SGCharInfo.h>

class SGCharacter : public SGPhysicsActor
{
public:
	SGCharacter();
	SGCharacter(int code);
	SGCharacter(int code, const VisualInfo& info);
	~SGCharacter() override;

	static SGCharacter* create(int code, const VisualInfo& info);

	void initInfo(int code, const VisualInfo& visualInfo);
	void initVisualInfo(const VisualInfo& visualInfo);
	void initBaseInfo(int code);

	void initActorSprite() override;
	void initListener(SGActorListener* listener) override;
	void hit(const SGHitInfo& hitInfo) override;

	void update(float dt) override;
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture) override;
	void cleanUpImmediate();

	SGCharBaseInfo* getBaseInfo();
private:
	SGCharBaseInfo* m_pBaseInfo;
	VisualInfo m_VisualInfo;
};


