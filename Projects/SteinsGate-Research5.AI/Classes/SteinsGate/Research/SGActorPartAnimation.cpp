/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 3:38:32 PM
 * =====================
 * 제로 프레임 처리시 onFrameBegin과 onFrameEnd 둘다 동시 호출이 이뤄지도록 구현했다.
 * 둘중 어느 프레임에서 기능을 구현해도 되지만. isZeroFramePaused()의 결과값은
 * 함수는 onFrameEnd, onAnimateEnd에서만 true로 관측된다.
 * 따라서 제로 프레임처리시 왠만하면 Begin보단 End에서 처리하도록 하자.
 * Begin, End 둘중 하나만 호출하도록 구현하면 뭔가 좀 깔끔하지 않아서. 일단 둘다 호출하도록 만듬
 */


#include "SGActorPartAnimation.h"

#include <SteinsGate/Research/SGActorPartSprite.h>
#include <SteinsGate/Research/SGImagePackManager.h>

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
	, m_bPaused(false)
	, m_bZeroFramePaused(false)
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
	m_bZeroFramePaused = false;
	m_bPaused = false;
	m_bLoopSequence = false;
}

void SGActorPartAnimation::run() {
	run(0);
}

void SGActorPartAnimation::run(int frameIndexInAnimation) {
	init();

	m_iFrameIndexInAnimation = frameIndexInAnimation;

	SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
	float fCurrentFrameDelay = m_pAnimationInfo->Frames[m_iFrameIndexInAnimation].Delay;

	m_pTarget->onAnimationBegin(this, pStartFrameTexture);
	m_pTarget->onFrameBegin(this, pStartFrameTexture);

	updateZeroDelayFrame(fCurrentFrameDelay, pStartFrameTexture);
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

	float fCurrentFrameDelay = m_pAnimationInfo->Frames[m_iFrameIndexInAnimation].Delay;
	SGFrameTexture* pCurrentFrameTexture = getTexture(m_iFrameIndexInAnimation);

	updateLoopSequence(dt);
	updateAnimation(fCurrentFrameDelay, pCurrentFrameTexture, dt);
}


void SGActorPartAnimation::updateLoopSequence(float dt) {

	if (m_bLoopSequence == false)
		return;

	// 같은 애니메이션 연속 재생하는 경우땜에 추가함.
	// 보기 너무 안좋은데 전부 다시 작성하기엔 시간이 너무 오래 걸릴 것 같아서.
	// 일단 두고 넘어간다.
	// 나중에 인생에 여유가 생기면 그때 느긋하게 고민하는걸로
	// 아니면 이코드 땜에 심각한 문제가 발생하거나..
	m_iFrameIndexInAnimation = 0;
	m_fRunningFrameTime = 0.0f;
	SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
	m_pTarget->onAnimationBegin(this, pStartFrameTexture);
	m_pTarget->onFrameBegin(this, pStartFrameTexture);
	m_bLoopSequence = false;	// 토글
}



void SGActorPartAnimation::updateAnimation(float currentFrameDelay, SGFrameTexture* currentFrameTexture, float dt) {



	// Step Check. 애니메이션 실행가능 여부 체크
	if (m_bPaused || m_bFinished || m_bZeroFramePaused)
		return;

	// ==========================================================
	//  애니메이션 본격 업데이트 진행
	// ==========================================================

	// Step 1. 프레임 시간 체크
	m_fRunningFrameTime += dt;

	if (m_fRunningFrameTime < currentFrameDelay + m_fPauseDelay) {
		return;
	}


	// Step 2. 마지막 프레임이 아닌 경우
	m_pTarget->onFrameEnd(this, currentFrameTexture);
	m_fRunningFrameTime = 0;
	m_fPauseDelay = 0;

	if (m_iFrameIndexInAnimation < m_vAnimationFrames.Size() - 1) {

		if (m_bLoopSequence)
			return;

		++m_iFrameIndexInAnimation;

		SGFrameTexture* pNextFrameTexture = changeTexture(m_iFrameIndexInAnimation);
		float fNextFrameDelay = m_pAnimationInfo->Frames[m_iFrameIndexInAnimation].Delay;

		m_pTarget->onFrameBegin(this, pNextFrameTexture);

		updateZeroDelayFrame(fNextFrameDelay, pNextFrameTexture);
		return;
	}

	// Step 3. 마지막 프레임인 경우
	// onAnimateEnd에서 데이터가 정상적으로 보이도록 하기위해 전처리 진행
	//  - m_bFinished의 경우먼저 처리해주는 이유가 예를 들어서 달리기 애니메이션 실행 후 onAnimateEnd에서
	//	  다시 동일한 애니메이션을 실행해버리는 것 같은 경우가 있을 수 있는데
	//    onAnimateEnd 함수 진입 -> runAction -> init(m_bFinshed = false) -> onAnimateEnd 함수 종료 -> m_bFinished = true로
	//    만들어버버리기 떄문에 이런 상황이 나오지 않도록 하기위해 위에서 처리함
	m_pTarget->onAnimationEnd(this, currentFrameTexture);

	if (m_bLoopSequence) {
		return;
	}


	if (m_pAnimationInfo->Loop) {
		m_iFrameIndexInAnimation = 0;
		SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
		float fStartFrameDelay = m_pAnimationInfo->Frames[m_iFrameIndexInAnimation].Delay;

		m_pTarget->onAnimationBegin(this, pStartFrameTexture);
		m_pTarget->onFrameBegin(this, pStartFrameTexture);

		updateZeroDelayFrame(fStartFrameDelay, pStartFrameTexture);
		return;
	}

	m_bFinished = true;
}

void SGActorPartAnimation::updateZeroDelayFrame(float currentFrameDelay, SGFrameTexture* currentFrameTexture) {

	// ==========================================================
	// 일시정지 프레임 처리(기획 파일에 프레임 딜레이 0이하로 입력하면 애니메이션을 정지시키도록 한다.)
	// 개발자가 resume 시키기 전까지는 애니메이션 멈춤
	// 여기 작성했기 때문에 하나의 파츠에만 적용될거라고 생각할 수 있겠지만
	// 모든 파츠는 동일한 애니메이션을 가지기 때문에 여기서 작성해도 무방하다.
	// ==========================================================

	// Step Check. 정지 프레임 여부 체크
	if (currentFrameDelay > 0.0f)
		return;


	m_bZeroFramePaused = true;

	// Step 1. 딜레이가 음수인 프레임이 마지막 프레임이 아닌 경우
	m_pTarget->onFrameEnd(this, currentFrameTexture);

	if (m_iFrameIndexInAnimation < m_vAnimationFrames.Size() - 1) {
		m_iFrameIndexInAnimation++;
		return;
	}

	// Step 2. 딜레이가 음수인 프레임이 마지막 프레임인 경우
	m_pTarget->onAnimationEnd(this, currentFrameTexture);

	if (m_pAnimationInfo->Loop) {
		m_iFrameIndexInAnimation = 0;
		return;
	}

	m_bFinished = true;
}


int SGActorPartAnimation::getPartIndex() {
	return m_pTarget->getPartIndex();
}

SGFrameInfo* SGActorPartAnimation::getFrameInfo(int frameIndexInAnimation) {
	DebugAssertMessage(frameIndexInAnimation >= 0 && frameIndexInAnimation < m_pAnimationInfo->Frames.Size(), "애니메이션 내 프레임 인덱스가 이상합니다.");
	return &m_pAnimationInfo->Frames[frameIndexInAnimation];
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

	
	// 동일한 텍스쳐 스킵
	if (m_pTarget->getTexture() == pFrameTexture->getTexture())
		return pFrameTexture;


	// initWithTexture... 상당히 무거운데
	// 최적화 없는 상태에서 한 틱당 initWithTexture 2만번 호출까지는 프레임 드랍 없다.
	// 일단 문제 생기면 Sprite 풀을 구성해놓고 교체해주는 방식으로 바꾸는 걸로
	// 이것도 완성하면 나중에... ㅋㅋ 뭔 전부 나중이야
	m_pTarget->initWithTexture(pFrameTexture->getTexture());
	m_pTarget->setAnchorPoint(m_pTarget->getActorType() == ActorType::Projectile ? SGVec2::ANCHOR_MIDDLE : SGVec2::ZERO);
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
	

	// 제로 딜레이 프레임을 해제한 경우 프레임 이벤트를 실행시켜 줘야함.
	// updateZeroDelayFrame 함수에서 다음 프레임으로 인덱스 변경만 해놓고
	// 프레임, 애니메이션 시작 콜백함수는 호출안해놓은 상태이기 때문이다.
	if (m_bZeroFramePaused) {
		SGFrameTexture* pStartFrameTexture = changeTexture(m_iFrameIndexInAnimation);
		m_pTarget->onAnimationBegin(this, pStartFrameTexture);
		m_pTarget->onFrameBegin(this, pStartFrameTexture);
		m_bZeroFramePaused = false;
	}
}


