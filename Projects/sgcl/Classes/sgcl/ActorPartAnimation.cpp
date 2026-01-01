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

#include "Core.h"
#include "GameCoreHeader.h"
#include "ActorPartAnimation.h"

#include <sgcl/ActorPartSprite.h>
#include <sgcl/ImagePackManager.h>
#include <sgcl/ActorSprite.h>
#include <sgcl/Actor.h>

//////////////////////////////////////////////////////////////////////////////////////////
ActorPartAnimation::ActorPartAnimation(
	ActorPartSprite* _pAnimationTarget,
	AnimationInfo* _pAnimationInfo,
	jc::Vector<FrameTexture*>& _frames)
//////////////////////////////////////////////////////////////////////////////////////////
: sgaIndex_(0)
, imgIndex_(0)
, pAnimationInfo_(_pAnimationInfo)
, pTarget_(_pAnimationTarget)
, animationFrames_(_pAnimationInfo->frames_.Size(), nullptr)
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
ActorPartAnimation* ActorPartAnimation::Create(
	ActorPartSprite* _pAnimationTarget,
	AnimationInfo* _pAnimationInfo,
	jc::Vector<FrameTexture*>& _frames)
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

void ActorPartAnimation::Init()
{
	runningFrameTime_ = 0.0f;
	frameIndexInAnimation_ = 0;
	finished_ = false;
	zeroFramePaused_ = false;
	paused_ = false;
	loopSequence_ = false;
}

void ActorPartAnimation::Run()
{
	Run(0);
}

void ActorPartAnimation::Run(int _frameIndexInAnimation)
{
	Init();

	frameIndexInAnimation_ = _frameIndexInAnimation;

	FrameTexture* pStartFrameTexture = ChangeTexture(frameIndexInAnimation_);
	const float currentFrameDelay = pAnimationInfo_->frames_[frameIndexInAnimation_].delay_;

	pTarget_->OnAnimationBegin(this, pStartFrameTexture);
	pTarget_->OnFrameBegin(this, pStartFrameTexture);

	UpdateZeroDelayFrame(currentFrameDelay, pStartFrameTexture);
}

void ActorPartAnimation::ConstructFrames(int _sgaIndex, int _imgIndex)
{
	sgaIndex_ = _sgaIndex;
	imgIndex_ = _imgIndex;

	for (int i = 0; i < animationFrames_.Size(); ++i)
	{
		CC_SAFE_RELEASE(animationFrames_[i]);
	}

	// 애니메이션 프레임 구성
	for (int i = 0; i < pAnimationInfo_->frames_.Size(); ++i)
	{
		const FrameInfo& frameInfo = pAnimationInfo_->frames_[i];
		const int frameIndex = frameInfo.frameIndex_;

		if (frameIndex < 0 || frameIndex >= frames_.Size())
		{
			// 예를들어서 애니메이션이 5프레임으로 구성하여 설정파일에 작성했는데
			// 이미지를 구성하는 텍스쳐가 4개 밖에 없는 경우 5번째 프레임텍스쳐를 가져오는 것 자체가 불가능하므로.. 체크함.
			g_cImagePackMgr.LogTexture("ActorPartAnimation::ConstructFrames()",
			                                       { sgaIndex_, imgIndex_, InvalidValue_v },
			                                       jc::LoggerAbstract::eError);
			jc_assert_msg(false, "전체 프레임 수(%d)에 포함되지 않는 애니메이션 프레임 인덱스(%d)입니다.", frames_.Size(), frameIndex);
		}

		animationFrames_[i] = frames_[frameIndex];
	}
}

void ActorPartAnimation::SetPlaySpeed(float _speed)
{
	playSpeed_ = _speed;
}

void ActorPartAnimation::SetLoopSequence()
{
	finished_ = false;
	loopSequence_ = true;
}

void ActorPartAnimation::Update(float _dt)
{
	const float currentFrameDelay = pAnimationInfo_->frames_[frameIndexInAnimation_].delay_;
	FrameTexture* pCurrentFrameTexture = GetTexture(frameIndexInAnimation_);

	UpdateLoopSequence(_dt);
	UpdateAnimation(currentFrameDelay, pCurrentFrameTexture, _dt);
}

void ActorPartAnimation::UpdateLoopSequence(float _dt)
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
	FrameTexture* pStartFrameTexture = ChangeTexture(frameIndexInAnimation_);
	pTarget_->OnAnimationBegin(this, pStartFrameTexture);
	pTarget_->OnFrameBegin(this, pStartFrameTexture);
	loopSequence_ = false; // 토글
}

void ActorPartAnimation::UpdateAnimation(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture, float _dt)
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
	pTarget_->OnFrameEnd(this, _pCurrentFrameTexture);
	runningFrameTime_ = 0.0f;
	pauseDelay_ = 0.0f;

	if (frameIndexInAnimation_ < animationFrames_.Size() - 1)
	{
		if (loopSequence_)
		{
			return;
		}

		++frameIndexInAnimation_;

		FrameTexture* pNextFrameTexture = ChangeTexture(frameIndexInAnimation_);
		float nextFrameDelay = pAnimationInfo_->frames_[frameIndexInAnimation_].delay_;

		pTarget_->OnFrameBegin(this, pNextFrameTexture);

		UpdateZeroDelayFrame(nextFrameDelay, pNextFrameTexture);
		return;
	}

	// Step 3. 마지막 프레임인 경우
	// onAnimateEnd에서 데이터가 정상적으로 보이도록 하기위해 전처리 진행
	//  - finished_의 경우먼저 처리해주는 이유가 예를 들어서 달리기 애니메이션 실행 후 onAnimateEnd에서
	//	  다시 동일한 애니메이션을 실행해버리는 것 같은 경우가 있을 수 있는데
	//    onAnimateEnd 함수 진입 -> runAction -> init(finished_ = false) -> onAnimateEnd 함수 종료 -> finished_ = true로
	//    만들어버버리기 떄문에 이런 상황이 나오지 않도록 하기위해 위에서 처리함
	pTarget_->OnAnimationEnd(this, _pCurrentFrameTexture);

	if (loopSequence_)
	{
		return;
	}

	if (pAnimationInfo_->loop_)
	{
		frameIndexInAnimation_ = 0;
		FrameTexture* pStartFrameTexture = ChangeTexture(frameIndexInAnimation_);
		float startFrameDelay = pAnimationInfo_->frames_[frameIndexInAnimation_].delay_;

		pTarget_->OnAnimationBegin(this, pStartFrameTexture);
		pTarget_->OnFrameBegin(this, pStartFrameTexture);

		UpdateZeroDelayFrame(startFrameDelay, pStartFrameTexture);
		return;
	}

	finished_ = true;
}

void ActorPartAnimation::UpdateZeroDelayFrame(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture)
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
	pTarget_->OnFrameEnd(this, _pCurrentFrameTexture);

	if (frameIndexInAnimation_ < animationFrames_.Size() - 1)
	{
		++frameIndexInAnimation_;
		return;
	}

	// Step 2. 딜레이가 음수인 프레임이 마지막 프레임인 경우
	pTarget_->OnAnimationEnd(this, _pCurrentFrameTexture);

	if (pAnimationInfo_->loop_)
	{
		frameIndexInAnimation_ = 0;
		return;
	}

	finished_ = true;
}

int ActorPartAnimation::GetPartIndex()
{
	return pTarget_->GetPartIndex();
}

FrameInfo& ActorPartAnimation::GetFrameInfo(int _frameIndexInAnimation)
{
	jc_assert_msg(_frameIndexInAnimation >= 0 && _frameIndexInAnimation < pAnimationInfo_->frames_.Size(),
	               "애니메이션 내 프레임 인덱스가 이상합니다.");
	return pAnimationInfo_->frames_[_frameIndexInAnimation];
}

FrameInfo& ActorPartAnimation::GetRunningFrameInfo()
{
	return pAnimationInfo_->frames_[frameIndexInAnimation_];
}

int ActorPartAnimation::GetRunningFrameEventCode()
{
	return pAnimationInfo_->frames_[frameIndexInAnimation_].frameEventCode_;
}

void ActorPartAnimation::ReflectAnimation(ActorPartAnimation* _pRunningAnimation)
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

void ActorPartAnimation::SetAnimationInfo(AnimationInfo* _pAnimationInfo)
{
	pAnimationInfo_ = _pAnimationInfo;
}

FrameTexture* ActorPartAnimation::ChangeTexture(int _frameIndexInAnimation)
{
	FrameTexture* pFrameTexture = animationFrames_[_frameIndexInAnimation];

	if (pFrameTexture->IsLink())
	{
		int targetFrameIndex = pFrameTexture->GetTargetFrameIndex();
		pFrameTexture = frames_[targetFrameIndex];
	}

	// 링크 검사 이후 체크해야함
	if (pFrameTexture->IsDummy())
	{
		// null로 넣어주면 2x2 흰색 텍스쳐를 넣어줌
		// 이렇게 넣어줘야 opacity하고 컬러 조절이 가능해지기 때문이라고 엔진 코드에 적혀있다.
		// target_->setTexture(nullptr);
		// createFrameTextureRetain에서 그냥 2x2 디폴트 생성해서 넣음
		// --> 그냥 처음에 내가 2x2 텍스쳐 넣어놓음 안보이게만 해주자.

		pTarget_->setOpacity(0);
		return pFrameTexture;
	}

	// 동일한 텍스쳐 스킵
	if (pTarget_->getTexture() == pFrameTexture->GetTexture())
	{
		return pFrameTexture;
	}

	// initWithTexture... 상당히 무거운데
	// 최적화 없는 상태에서 한 틱당 initWithTexture 2만번 호출까지는 프레임 드랍 없다.
	// 일단 문제 생기면 Sprite 풀을 구성해놓고 교체해주는 방식으로 바꾸는 걸로
	// 이것도 완성하면 나중에... ㅋㅋ 뭔 전부 나중이야
	pTarget_->initWithTexture(pFrameTexture->GetTexture());
	pTarget_->setAnchorPoint(pTarget_->GetActorType() == ActorType::Projectile ? c2d::vec2::ANCHOR_MIDDLE : c2d::vec2::ZERO);
	pTarget_->setOpacity(255);
	return pFrameTexture;
}

FrameTexture* ActorPartAnimation::GetTexture(int _frameIndexInAnimation)
{
	FrameTexture* pFrameTexture = animationFrames_[_frameIndexInAnimation];

	if (pFrameTexture->IsDummy())
	{
		pTarget_->setTexture(nullptr);
		pTarget_->setOpacity(0.0f);
		return pFrameTexture;
	}

	if (pFrameTexture->IsLink())
	{
		int targetFrameIndex = pFrameTexture->GetTargetFrameIndex();
		pFrameTexture = frames_[targetFrameIndex];
	}

	return pFrameTexture;
}

void ActorPartAnimation::Pause()
{
	paused_ = true;
}

// delay 만큼만 멈춤
void ActorPartAnimation::PauseTime(float _delay)
{
	pauseDelay_ = _delay;
}

void ActorPartAnimation::Resume()
{
	paused_ = false;

	// 제로 딜레이 프레임을 해제한 경우 프레임 이벤트를 실행시켜 줘야함.
	// updateZeroDelayFrame 함수에서 다음 프레임으로 인덱스 변경만 해놓고
	// 프레임, 애니메이션 시작 콜백함수는 호출안해놓은 상태이기 때문이다.
	if (zeroFramePaused_)
	{
		FrameTexture* pStartFrameTexture = ChangeTexture(frameIndexInAnimation_);
		pTarget_->OnAnimationBegin(this, pStartFrameTexture);
		pTarget_->OnFrameBegin(this, pStartFrameTexture);
		zeroFramePaused_ = false;
	}
}
