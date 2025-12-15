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

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "ActorPartAnimation.h"

#include <SteinsGate/Client/ActorPartSprite.h>
#include <SteinsGate/Client/ImagePackManager.h>
#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/Actor.h>

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation::ActorPartAnimation(
	ActorPartSprite* _pAnimationTarget,
	AnimationInfo* _pAnimationInfo,
	SGVector<FrameTexture*>& _frames)
//////////////////////////////////////////////////////////////////////////////////////////
: animationInfo_(_pAnimationInfo)
, target_(_pAnimationTarget)
, animationFrames_(_pAnimationInfo->Frames.Size(), nullptr)
, frames_(_frames)
, runningFrameTime_(0.0f)
, pauseDelay_(0.0f) // 기본 재생속도
, playSpeed_(1.0f)
, frameIndexInAnimation_(0)
, finished_(false)
, paused_(false)
, zeroFramePaused_(false)
, loopSequence_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation* ActorPartAnimation::create(
	ActorPartSprite* _pAnimationTarget,
	AnimationInfo* _pAnimationInfo,
	SGVector<FrameTexture*>& _frames)
//////////////////////////////////////////////////////////////////////////////////////////
{
	ActorPartAnimation* pAnimation = dbg_new ActorPartAnimation(_pAnimationTarget, _pAnimationInfo, _frames);

	if (pAnimation)
	{
		pAnimation->autorelease();
		return pAnimation;
	}

	JCORE_DELETE_SAFE(pAnimation);
	return nullptr;
}

void ActorPartAnimation::init()
{
	runningFrameTime_ = 0.0f;
	frameIndexInAnimation_ = 0;
	finished_ = false;
	zeroFramePaused_ = false;
	paused_ = false;
	loopSequence_ = false;
}

void ActorPartAnimation::run()
{
	run(0);
}

void ActorPartAnimation::run(int _frameIndexInAnimation)
{
	init();

	frameIndexInAnimation_ = _frameIndexInAnimation;

	FrameTexture* pStartFrameTexture = changeTexture(frameIndexInAnimation_);
	const float currentFrameDelay = animationInfo_->Frames[frameIndexInAnimation_].Delay;

	target_->onAnimationBegin(this, pStartFrameTexture);
	target_->onFrameBegin(this, pStartFrameTexture);

	updateZeroDelayFrame(currentFrameDelay, pStartFrameTexture);
}

void ActorPartAnimation::constructFrames(int _sgaIndex, int _imgIndex)
{
	sgaIndex_ = _sgaIndex;
	imgIndex_ = _imgIndex;

	for (int i = 0; i < animationFrames_.Size(); ++i)
	{
		CC_SAFE_RELEASE(animationFrames_[i]);
	}

	// 애니메이션 프레임 구성
	for (int i = 0; i < animationInfo_->Frames.Size(); ++i)
	{
		const FrameInfo& frameInfo = animationInfo_->Frames[i];
		const int frameIndex = frameInfo.FrameIndex;

		if (frameIndex < 0 || frameIndex >= frames_.Size())
		{
			// 예를들어서 애니메이션이 5프레임으로 구성하여 설정파일에 작성했는데
			// 이미지를 구성하는 텍스쳐가 4개 밖에 없는 경우 5번째 프레임텍스쳐를 가져오는 것 자체가 불가능하므로.. 체크함.
			Core::Contents.PackManager->logTexture("ActorPartAnimation::ConstructFrames()",
			                                       { sgaIndex_, imgIndex_, InvalidValue_v },
			                                       JCore::LoggerAbstract::eError);
			DebugAssertMsg(false, "전체 프레임 수(%d)에 포함되지 않는 애니메이션 프레임 인덱스(%d)입니다.", frames_.Size(), frameIndex);
		}

		animationFrames_[i] = frames_[frameIndex];
	}
}

void ActorPartAnimation::setPlaySpeed(float _speed)
{
	playSpeed_ = _speed;
}

void ActorPartAnimation::setLoopSequence()
{
	finished_ = false;
	loopSequence_ = true;
}

void ActorPartAnimation::update(float _dt)
{
	const float currentFrameDelay = animationInfo_->Frames[frameIndexInAnimation_].Delay;
	FrameTexture* pCurrentFrameTexture = getTexture(frameIndexInAnimation_);

	updateLoopSequence(_dt);
	updateAnimation(currentFrameDelay, pCurrentFrameTexture, _dt);
}

void ActorPartAnimation::updateLoopSequence(float _dt)
{
	(void)_dt;

	if (!loopSequence_)
	{
		return;
	}

	// 같은 애니메이션 연속 재생하는 경우땜에 추가함.
	// 보기 너무 안좋은데 전부 다시 작성하기엔 시간이 너무 오래 걸릴 것 같아서.
	// 일단 두고 넘어간다.
	// 나중에 인생에 여유가 생기면 그때 느긋하게 고민하는걸로
	// 아니면 이코드 땜에 심각한 문제가 발생하거나..
	frameIndexInAnimation_ = 0;
	runningFrameTime_ = 0.0f;
	FrameTexture* pStartFrameTexture = changeTexture(frameIndexInAnimation_);
	target_->onAnimationBegin(this, pStartFrameTexture);
	target_->onFrameBegin(this, pStartFrameTexture);
	loopSequence_ = false; // 토글
}

void ActorPartAnimation::updateAnimation(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture, float _dt)
{
	// Step Check. 애니메이션 실행가능 여부 체크
	if (paused_ || finished_ || zeroFramePaused_)
	{
		return;
	}

	// ==========================================================
	//  애니메이션 본격 업데이트 진행
	// ==========================================================

	// Step 1. 프레임 시간 체크
	runningFrameTime_ += _dt;

	if (runningFrameTime_ < (_currentFrameDelay + pauseDelay_))
	{
		return;
	}

	// Step 2. 마지막 프레임이 아닌 경우
	target_->onFrameEnd(this, _pCurrentFrameTexture);
	runningFrameTime_ = 0.0f;
	pauseDelay_ = 0.0f;

	if (frameIndexInAnimation_ < animationFrames_.Size() - 1)
	{
		if (loopSequence_)
		{
			return;
		}

		++frameIndexInAnimation_;

		FrameTexture* pNextFrameTexture = changeTexture(frameIndexInAnimation_);
		float nextFrameDelay = animationInfo_->Frames[frameIndexInAnimation_].Delay;

		target_->onFrameBegin(this, pNextFrameTexture);

		updateZeroDelayFrame(nextFrameDelay, pNextFrameTexture);
		return;
	}

	// Step 3. 마지막 프레임인 경우
	// onAnimateEnd에서 데이터가 정상적으로 보이도록 하기위해 전처리 진행
	//  - finished_의 경우먼저 처리해주는 이유가 예를 들어서 달리기 애니메이션 실행 후 onAnimateEnd에서
	//	  다시 동일한 애니메이션을 실행해버리는 것 같은 경우가 있을 수 있는데
	//    onAnimateEnd 함수 진입 -> runAction -> init(finished_ = false) -> onAnimateEnd 함수 종료 -> finished_ = true로
	//    만들어버버리기 떄문에 이런 상황이 나오지 않도록 하기위해 위에서 처리함
	target_->onAnimationEnd(this, _pCurrentFrameTexture);

	if (loopSequence_)
	{
		return;
	}

	if (animationInfo_->Loop)
	{
		frameIndexInAnimation_ = 0;
		FrameTexture* pStartFrameTexture = changeTexture(frameIndexInAnimation_);
		float startFrameDelay = animationInfo_->Frames[frameIndexInAnimation_].Delay;

		target_->onAnimationBegin(this, pStartFrameTexture);
		target_->onFrameBegin(this, pStartFrameTexture);

		updateZeroDelayFrame(startFrameDelay, pStartFrameTexture);
		return;
	}

	finished_ = true;
}

void ActorPartAnimation::updateZeroDelayFrame(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture)
{
	// ==========================================================
	// 일시정지 프레임 처리(기획 파일에 프레임 딜레이 0이하로 입력하면 애니메이션을 정지시키도록 한다.)
	// 개발자가 resume 시키기 전까지는 애니메이션 멈춤
	// 여기 작성했기 때문에 하나의 파츠에만 적용될거라고 생각할 수 있겠지만
	// 모든 파츠는 동일한 애니메이션을 가지기 때문에 여기서 작성해도 무방하다.
	// ==========================================================

	// Step Check. 정지 프레임 여부 체크
	if (_currentFrameDelay > 0.0f)
	{
		return;
	}

	zeroFramePaused_ = true;

	// Step 1. 딜레이가 음수인 프레임이 마지막 프레임이 아닌 경우
	target_->onFrameEnd(this, _pCurrentFrameTexture);

	if (frameIndexInAnimation_ < animationFrames_.Size() - 1)
	{
		++frameIndexInAnimation_;
		return;
	}

	// Step 2. 딜레이가 음수인 프레임이 마지막 프레임인 경우
	target_->onAnimationEnd(this, _pCurrentFrameTexture);

	if (animationInfo_->Loop)
	{
		frameIndexInAnimation_ = 0;
		return;
	}

	finished_ = true;
}

int ActorPartAnimation::getPartIndex()
{
	return target_->getPartIndex();
}

FrameInfo& ActorPartAnimation::getFrameInfo(int _frameIndexInAnimation)
{
	DebugAssertMsg(_frameIndexInAnimation >= 0 && _frameIndexInAnimation < animationInfo_->Frames.Size(),
	               "애니메이션 내 프레임 인덱스가 이상합니다.");
	return animationInfo_->Frames[_frameIndexInAnimation];
}

FrameInfo& ActorPartAnimation::getRunningFrameInfo()
{
	return animationInfo_->Frames[frameIndexInAnimation_];
}

int ActorPartAnimation::getRunningFrameEventCode()
{
	return animationInfo_->Frames[frameIndexInAnimation_].FrameEventCode;
}

void ActorPartAnimation::reflectAnimation(ActorPartAnimation* _pRunningAnimation)
{
	runningFrameTime_ = _pRunningAnimation->runningFrameTime_;
	pauseDelay_ = _pRunningAnimation->pauseDelay_;
	playSpeed_ = _pRunningAnimation->playSpeed_;
	frameIndexInAnimation_ = _pRunningAnimation->frameIndexInAnimation_;
	finished_ = _pRunningAnimation->finished_;
	paused_ = _pRunningAnimation->paused_;
	zeroFramePaused_ = _pRunningAnimation->zeroFramePaused_;
	loopSequence_ = _pRunningAnimation->loopSequence_;
}

void ActorPartAnimation::setAnimationInfo(AnimationInfo* _pAnimationInfo)
{
	animationInfo_ = _pAnimationInfo;
}

FrameTexture* ActorPartAnimation::changeTexture(int _frameIndexInAnimation)
{
	FrameTexture* pFrameTexture = animationFrames_[_frameIndexInAnimation];

	if (pFrameTexture->isLink())
	{
		int targetFrameIndex = pFrameTexture->getTargetFrameIndex();
		pFrameTexture = frames_[targetFrameIndex];
	}

	// 링크 검사 이후 체크해야함
	if (pFrameTexture->isDummy())
	{
		// null로 넣어주면 2x2 흰색 텍스쳐를 넣어줌
		// 이렇게 넣어줘야 opacity하고 컬러 조절이 가능해지기 때문이라고 엔진 코드에 적혀있다.
		// target_->setTexture(nullptr);
		// createFrameTextureRetain에서 그냥 2x2 디폴트 생성해서 넣음
		// --> 그냥 처음에 내가 2x2 텍스쳐 넣어놓음 안보이게만 해주자.

		target_->setOpacity(0);
		return pFrameTexture;
	}

	// 동일한 텍스쳐 스킵
	if (target_->getTexture() == pFrameTexture->getTexture())
	{
		return pFrameTexture;
	}

	// initWithTexture... 상당히 무거운데
	// 최적화 없는 상태에서 한 틱당 initWithTexture 2만번 호출까지는 프레임 드랍 없다.
	// 일단 문제 생기면 Sprite 풀을 구성해놓고 교체해주는 방식으로 바꾸는 걸로
	// 이것도 완성하면 나중에... ㅋㅋ 뭔 전부 나중이야
	target_->initWithTexture(pFrameTexture->getTexture());
	target_->setAnchorPoint(target_->getActorType() == ActorType::Projectile ? SGVec2::ANCHOR_MIDDLE : SGVec2::ZERO);
	target_->setOpacity(255);
	return pFrameTexture;
}

FrameTexture* ActorPartAnimation::getTexture(int _frameIndexInAnimation)
{
	FrameTexture* pFrameTexture = animationFrames_[_frameIndexInAnimation];

	if (pFrameTexture->isDummy())
	{
		target_->setTexture(nullptr);
		target_->setOpacity(0.0f);
		return pFrameTexture;
	}

	if (pFrameTexture->isLink())
	{
		int targetFrameIndex = pFrameTexture->getTargetFrameIndex();
		pFrameTexture = frames_[targetFrameIndex];
	}

	return pFrameTexture;
}

void ActorPartAnimation::pause()
{
	paused_ = true;
}

// delay 만큼만 멈춤
void ActorPartAnimation::pauseTime(float _delay)
{
	pauseDelay_ = _delay;
}

void ActorPartAnimation::resume()
{
	paused_ = false;

	// 제로 딜레이 프레임을 해제한 경우 프레임 이벤트를 실행시켜 줘야함.
	// updateZeroDelayFrame 함수에서 다음 프레임으로 인덱스 변경만 해놓고
	// 프레임, 애니메이션 시작 콜백함수는 호출안해놓은 상태이기 때문이다.
	if (zeroFramePaused_)
	{
		FrameTexture* pStartFrameTexture = changeTexture(frameIndexInAnimation_);
		target_->onAnimationBegin(this, pStartFrameTexture);
		target_->onFrameBegin(this, pStartFrameTexture);
		zeroFramePaused_ = false;
	}
}
