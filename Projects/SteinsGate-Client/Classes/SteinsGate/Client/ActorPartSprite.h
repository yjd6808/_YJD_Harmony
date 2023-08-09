/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:11 PM
 * =====================
 * 파츠의 0번 인덱스는 무조건 바디 파츠로 고정
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>
#include <SteinsGate/Client/ActorPartAnimation.h>


class ActorSprite;
class ActorPartSprite : public SGSprite
{
public:
	static ActorPartSprite* create(
		int frameCount,
		ActorSprite* actor,
		SGNode* canvas,
		SGDrawNode* boundingBox,
		const ActorPartSpriteData& partData,
		SGVector<AnimationInfo>* animations
	);

	ActorPartSprite(
		int frameCount, 
		ActorSprite* actor,
		SGNode* canvas,
		SGDrawNode* boundingBox,
		const ActorPartSpriteData& partData,
		SGVector<AnimationInfo>* animations
	);
	~ActorPartSprite() override;

	bool init() override;
	void initFrames();
	void initAnimations();

	void update(float dt) override;
	void updateBoundingBoxVisibleState();
	void updateAnimationReference(SGVector<AnimationInfo>* animations);

	void runAnimation(int code);
	void runAnimation(int code, int startFrameIndexInAnimation);

	void setPartIndex(int index) { m_iPartIndex = index;  }
	int getPartIndex() { return m_iPartIndex; }
	ActorType_t getActorType();
	SGNode* getCanvas() { return m_pCanvas; }
	SGDrawNode* getPartBoundingBox() { return m_pBoundingBox; }
	ActorSprite* getActorSprite() { return m_pActorSprite; }
	const ActorPartSpriteData& getPartData() { return m_PartData; }
	ActorPartAnimation* getRunningAnimation() { return m_pRunningAnimation; }

	void onAnimationBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onAnimationEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void onCustomFrameBegin(ActorPartAnimation* animation, FrameTexture* texture);
	void onCustomFrameEnd(ActorPartAnimation* animation, FrameTexture* texture);
	void reflectAnimation(ActorPartAnimation* runningAnimation);	// runningAnimation과 동일한 상태의 애니메이션을 실행하도록 함
private:
	// 주입 데이터
	int m_iPartIndex;	// 몇번째 파츠인지
	ActorSprite* m_pActorSprite;
	ActorPartSpriteData m_PartData;
	SGVector<AnimationInfo>* m_pAnimationInfoList;

	// 자체 데이터
	SGNode* m_pCanvas;
	SGDrawNode* m_pBoundingBox;
	ActorPartAnimation* m_pRunningAnimation;
	SGHashMap<int, ActorPartAnimation*> m_AnimationMap;
	SGVector<ActorPartAnimation*> m_vAnimationList;
	SGVector<FrameTexture*> m_vFrames;	// 애니메이션 전체 프레임
};


