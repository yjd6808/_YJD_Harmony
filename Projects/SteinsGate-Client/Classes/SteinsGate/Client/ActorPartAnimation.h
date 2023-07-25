/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 3:38:18 PM
 * =====================
 * 코코스 엔진은 애니메이션 시간 조절을 세밀하게 하기 힘든것 같아서.
 * 일단 자체적으로 구현하였다.
 * 이것도 마찬가지로 프레임 각각 적용은 안되지만.
 * 훨씬 쉽게 구현가능하다.
 * 우선 프레임 각각의 시간을 변경시킬일이 생기면 그때 개발하는 걸로
 * 아직은 쓸일이 없는 듯 - 2023/01/22(설날)
 */


#pragma once


#include "Tutturu.h"
#include <SteinsGate/Client/FrameTexture.h>
#include <SteinsGate/Client/FrameEvent.h>

class ActorPartSprite;
class ActorPartAnimation : public cocos2d::Ref
{
public:
	enum State
	{
		eInitialized,
		eRunning,
		ePaused,
		eZeroFramePaused,
		eFinished
	};

	static ActorPartAnimation* create(
		ActorPartSprite* animationTarget, 
		AnimationInfo* animationInfo,
		SGVector<FrameTexture*>& frames
	);

	ActorPartAnimation(
		ActorPartSprite* animationTarget, 
		AnimationInfo* animationInfo, 
		SGVector<FrameTexture*>& frames
	);
	
	void constructFrames(int sgaIndex, int imgIndex);
	void update(float dt);
	void updateLoopSequence(float dt);
	void updateAnimation(float currentdelayFrame, FrameTexture* currentFrameTexture, float dt);
	void updateZeroDelayFrame(float currentFrameDelay, FrameTexture* currentFrameTexture);
	void init();
	void run();
	void run(int frameIndexInAnimation);
	void pause();
	void pauseTime(float delay);
	void resume();
	void setPlaySpeed(float speed);
	void setLoopSequence();
	void setAnimationInfo(AnimationInfo* animationInfo);

	AnimationInfo* getAnimationInfo() { return m_pAnimationInfo; }

	int getFrameIndexInAnimation() { return m_iFrameIndexInAnimation; }
	int getTargetFrameIndex() { return m_vAnimationFrames[m_iFrameIndexInAnimation]->getTargetFrameIndex(); }
	int getFrameIndex() { return m_vAnimationFrames[m_iFrameIndexInAnimation]->getFrameIndex(); }
	int getPartIndex();
	int getAnimationCode() { return m_pAnimationInfo->Code; }

	FrameInfo& getFrameInfo(int frameIndexInAnimation);
	FrameInfo& getRunningFrameInfo();
	int getRunningFrameEventCode();

	bool isFinished() { return m_bFinished; }
	bool isPaused() { return m_bPaused || m_bZeroFramePaused; }
	bool isZeroFramePaused() { return m_bZeroFramePaused; }
	void reflectAnimation(ActorPartAnimation* runningAnimation); // runningAnimation과 동일한 상태의 애니메이션 상태로 바꿈
	

private:
	FrameTexture* changeTexture(int frameIndexInAnimation);
	FrameTexture* getTexture(int frameIndexInAnimation);
private:
	// 주입 데이터
	int m_iSgaIndex;
	int m_iImgIndex;

	AnimationInfo* m_pAnimationInfo;
	ActorPartSprite* m_pTarget;
	SGVector<FrameTexture*>	m_vAnimationFrames;	// 해당 애니메이션을 구성하는 프레임들만
	SGVector<FrameTexture*>& m_vFrames;			// 이미지 팩의 전체 프레임 참조본 / 원본은 파츠가 들고있음

	// 자체 데이터
	float m_fRunningFrameTime;
	float m_fPauseDelay;
	float m_fPlaySpeed;
	int m_iFrameIndexInAnimation;
	bool m_bFinished;
	bool m_bPaused;
	bool m_bZeroFramePaused;
	bool m_bLoopSequence;		// 켜져있으면 한번 더 돔
};


