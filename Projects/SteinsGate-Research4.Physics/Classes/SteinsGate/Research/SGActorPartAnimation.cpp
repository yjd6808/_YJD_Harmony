/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 3:38:32 PM
 * =====================
 *
 */


#include "SGActorPartAnimation.h"

#include <SteinsGate/Research/SGActorPartSprite.h>
#include  <SteinsGate/Research/SGImagePackManager.h>

SGActorPartAnimation::SGActorPartAnimation(
	SGActorPartSprite* animationTarget,
	SGAnimationInfo* animationInfo,
	SGVector<SGFrameTexture*>& frames
)
	: m_pTarget(animationTarget)
	, m_pAnimationInfo(animationInfo)
	, m_vAnimationFrames(animationInfo->Frames.Size(), nullptr)
	, m_vFrames(frames)
	, m_fRunningFrameTime(0.0f)
	, m_fPlaySpeed(1.0f)			// 기본 재생속도
	, m_fPauseDelay(0.0f)
	, m_iFrameIndexInAnimation(0)
	, m_bFinished(false)
{}


SGActorPartAnimation* SGActorPartAnimation::create(
	SGActorPartSprite* animationTarget,
	SGAnimationInfo* animationInfo,
	SGVector<SGFrameTexture*>& frames
) {
	SGActorPartAnimation* pAnimation = new SGActorPartAnimation(animationTarget, animationInfo, frames);

	if (pAnimation) {
		pAnimation->autorelease();
		return pAnimation;
	}

	DeleteSafe(pAnimation);
	return nullptr;
}

void SGActorPartAnimation::init() {
	m_fRunningFrameTime = 0.0f;
	// m_fPlaySpeed = 1.0f;
	m_iFrameIndexInAnimation = 0;
	m_fPauseDelay = 0.0f;
	m_bFinished = false;
	m_bPaused = false;
	m_bLoopSequence = false;
}

void SGActorPartAnimation::run() {
	init();

	SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
	m_pTarget->onAnimationBegin(this, pStartFrameTexture);
	m_pTarget->onFrameBegin(this, pStartFrameTexture);
}

void SGActorPartAnimation::constructFrames(int npkIndex, int imgIndex) {
	SGImagePack* pImgPack = SGImagePackManager::getInstance()->getPack(npkIndex);

	m_iNpkIndex = npkIndex;
	m_iImgIndex = imgIndex;

	for (int i = 0; i < m_vAnimationFrames.Size(); ++i) {
		CC_SAFE_RELEASE(m_vAnimationFrames[i]);
	}

	// 애니메이션 프레임 구성
	for (int i = 0; i < m_pAnimationInfo->Frames.Size(); ++i) {
		SGFrameInfo& frameInfo = m_pAnimationInfo->Frames[i];
		m_vAnimationFrames[i] = m_vFrames[frameInfo.FrameIndex];
	}
}




void SGActorPartAnimation::setPlaySpeed(float speed) {
	m_fPlaySpeed = speed;
}

void SGActorPartAnimation::setLoopSequence() {
	m_bFinished = false;
	m_bLoopSequence = true;
}

void SGActorPartAnimation::update(float dt) {

	if (m_bPaused || m_bFinished)
		return;

	// 같은 애니메이션 연속 재생하는 경우땜에 추가함.
	// 보기 너무 안좋은데 전부 다시 작성하기엔 시간이 너무 오래 걸릴 것 같아서.
	// 일단 두고 넘어간다.
	// 나중에 취직하고 여유 생기면 그때 느긋하게 고민하는걸로
	// 아니면 이코드 땜에 심각한 문제가 발생하거나..
	if (m_bLoopSequence) {
		m_iFrameIndexInAnimation = 0;
		m_fRunningFrameTime = 0.0f;
		SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
		m_pTarget->onAnimationBegin(this, pStartFrameTexture);
		m_pTarget->onFrameBegin(this, pStartFrameTexture);
		m_bLoopSequence = false;
	}


	m_fRunningFrameTime += dt;
	float fCurrentFrameDelay = m_pAnimationInfo->Frames[m_iFrameIndexInAnimation].Delay;
	SGFrameTexture* pCurrentFrameTexture = getTexture(m_iFrameIndexInAnimation);

	if (m_fRunningFrameTime >= fCurrentFrameDelay + m_fPauseDelay) {
		m_pTarget->onFrameEnd(this, pCurrentFrameTexture);

		m_fRunningFrameTime = 0;
		m_fPauseDelay = 0;

		++m_iFrameIndexInAnimation;


		if (m_iFrameIndexInAnimation >= m_vAnimationFrames.Size()) {

			// onAnimateEnd에서 데이터가 정상적으로 보이도록 하기위해 전처리 진행
			//  - m_iFrameIndexInAnimation의 경우 프레임 내의 인덱스 값이 보장되어야한다.
			//
			//  - m_bFinished의 경우먼저 처리해주는 이유가 달리기 애니메이션 실행 후 onAnimateEnd에서
			//	  다시 동일한 애니메이션을 실행해버리는 것 같은 경우가 있을 수 있는데
			//    onAnimateEnd 함수 진입 -> runAction -> init(m_bFinshed = false) -> onAnimateEnd 함수 종료 -> m_bFinished = true로
			//    만들어버버리기 떄문에 이런 상황이 나오지 않도록 하기위해 위에서 처리함
			m_iFrameIndexInAnimation -= 1;
			m_bFinished = m_pAnimationInfo->Loop ? false : true;

			m_pTarget->onAnimationEnd(this, pCurrentFrameTexture);
			m_iFrameIndexInAnimation += 1;	// 다시 원래대로...

			if (m_pAnimationInfo->Loop) {
				m_iFrameIndexInAnimation = 0;
				SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
				m_pTarget->onAnimationBegin(this, pStartFrameTexture);
				m_pTarget->onFrameBegin(this, pStartFrameTexture);
			}
		}


		if (m_iFrameIndexInAnimation < m_vAnimationFrames.Size()) {
			SGFrameTexture* pNextFrameTexture = changeTexture(m_iFrameIndexInAnimation);
			m_pTarget->onFrameBegin(this, pNextFrameTexture);
		}
	}
}

int SGActorPartAnimation::getPartIndex() {
	return m_pTarget->getPartIndex();
}

SGFrameTexture* SGActorPartAnimation::changeTexture(int frameIndexInAnimation) {
	SGFrameTexture* pFrameTexture = m_vAnimationFrames[frameIndexInAnimation];

	if (pFrameTexture->isDummy()) {
		// null로 넣어주면 2x2 흰색 텍스쳐를 넣어줌
		// 이렇게 넣어줘야 opacity하고 컬러 조절이 가능해지기 때문이라고 엔진 코드에 적혀있다.
		// m_pTarget->setTexture(nullptr);
		// createFrameTextureRetain에서 그냥 2x2 디폴트 생성해서 넣음
		// --> 그냥 처음에 내가 2x2 텍스쳐 넣어놓음 안보이게만 해주자.

		m_pTarget->setOpacity(0);
		return pFrameTexture;
	}

	if (pFrameTexture->isLink()) {
		int iTargetFrameIndex = pFrameTexture->getTargetFrameIndex();
		pFrameTexture = m_vFrames[iTargetFrameIndex];
	}

	// initWithTexture... 상당히 무거운데
	// 한 틱당 initWithTexture 2만번 호출까지는 프레임 드랍 없다.
	// 일단 문제 생기면 Sprite 풀을 구성해놓고 교체해주는 방식으로 바꾸는 걸로
	// 이것도 완성하면 나중에... ㅋㅋ 뭔 전부 나중이야
	m_pTarget->initWithTexture(pFrameTexture->getTexture());
	m_pTarget->setAnchorPoint(SGVec2::ZERO);
	m_pTarget->setOpacity(255);
	return pFrameTexture;
}

SGFrameTexture* SGActorPartAnimation::getTexture(int frameIndexInAnimation) {
	SGFrameTexture* pFrameTexture = m_vAnimationFrames[frameIndexInAnimation];

	if (pFrameTexture->isDummy()) {
		m_pTarget->setTexture(nullptr);
		m_pTarget->setOpacity(0.0f);
		return pFrameTexture;
	}

	if (pFrameTexture->isLink()) {
		int iTargetFrameIndex = pFrameTexture->getTargetFrameIndex();
		pFrameTexture = m_vFrames[iTargetFrameIndex];
	}

	return pFrameTexture;
}

void SGActorPartAnimation::pause() {
	m_bPaused = true;
}

// delay 만큼만 멈춤
void SGActorPartAnimation::pauseTime(float delay) {
	m_fPauseDelay = delay;
}

void SGActorPartAnimation::resume() {
	m_bPaused = false;
}


