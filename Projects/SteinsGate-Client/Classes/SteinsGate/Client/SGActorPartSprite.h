﻿/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 1:09:11 PM
 * =====================
 * 파츠의 0번 인덱스는 무조건 바디 파츠로 고정
 *
 */


#pragma once

#include "Tutturu.h"
#include <SteinsGate/Client/SGStruct.h>
#include <SteinsGate/Client/SGActorPartAnimation.h>


class SGActorSprite;
class SGActorPartSprite : public SGSprite
{
public:
	static SGActorPartSprite* create(
		int frameCount,
		SGActorSprite* actor,
		SGNode* canvas,
		SGDrawNode* boundingBox,
		const ActorPartSpriteData& partData,
		SGVector<SGAnimationInfo*>& animations
	);

	SGActorPartSprite(
		int frameCount, 
		SGActorSprite* actor,
		SGNode* canvas,
		SGDrawNode* boundingBox,
		const ActorPartSpriteData& partData,
		SGVector<SGAnimationInfo*>& animations
	);
	~SGActorPartSprite() override;

	bool init() override;
	void update(float dt) override;
	void updateBoundingBoxVisibleState();
	void runAnimation(int code);
	void runAnimation(int code, int startFrameIndexInAnimation);

	void setPartIndex(int index) { m_iPartIndex = index;  }
	int getPartIndex() { return m_iPartIndex; }
	ActorType_t getActorType();
	SGNode* getCanvas() { return m_pCanvas; }
	SGDrawNode* getPartBoundingBox() { return m_pBoundingBox; }
	SGActorSprite* getActorSprite() { return m_pActorSprite; }
	const ActorPartSpriteData& getPartData() { return m_PartData; }
	SGActorPartAnimation* getRunningAnimation() { return m_pRunningAnimation; }

	void onAnimationBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onAnimationEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onCustomFrameBegin(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void onCustomFrameEnd(SGActorPartAnimation* animation, SGFrameTexture* texture);
	void reflectAnimation(SGActorPartAnimation* runningAnimation);	// runningAnimation과 동일한 상태의 애니메이션을 실행하도록 함
private:
	// 주입 데이터
	int m_iPartIndex;	// 몇번째 파츠인지
	SGActorSprite* m_pActorSprite;
	ActorPartSpriteData m_PartData;
	SGVector<SGAnimationInfo*>& m_refAnimationInfoList;

	// 자체 데이터
	SGNode* m_pCanvas;
	SGDrawNode* m_pBoundingBox;
	SGActorPartAnimation* m_pRunningAnimation;
	SGHashMap<int, SGActorPartAnimation*> m_AnimationMap;
	SGVector<SGActorPartAnimation*> m_vAnimationList;
	SGVector<SGFrameTexture*> m_vFrames;	// 애니메이션 전체 프레임
};

