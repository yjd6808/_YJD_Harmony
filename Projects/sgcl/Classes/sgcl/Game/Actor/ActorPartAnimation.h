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

#include "sgcl/Game/Texture/FrameTexture.h"
#include "sg/Struct/SteinsGate_FrameEvent.h"

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

    static ActorPartAnimation* Create(
        ActorPartSprite* _pAnimationTarget,
        AnimationInfo* _pAnimationInfo,
        jc::Vector<FrameTexture*>& _frames);

    ActorPartAnimation(
        ActorPartSprite* _pAnimationTarget,
        AnimationInfo* _pAnimationInfo,
        jc::Vector<FrameTexture*>& _frame);

    void ConstructFrames(int _sgaIndex, int _imgIndex);
    void Update(float _dt);
    void UpdateLoopSequence(float _dt);
    void UpdateAnimation(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture, float _dt);
    void UpdateZeroDelayFrame(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture);

    void Init();
    void Run();
    void Run(int _frameIndexInAnimation);
    void Pause();
    void PauseTime(float _delay);
    void Resume();
    void SetPlaySpeed(float _speed);
    void SetLoopSequence();
    void SetAnimationInfo(AnimationInfo* _pAnimationInfo);

    AnimationInfo* GetAnimationInfo() { return pAnimationInfo_; }

    int GetFrameIndexInAnimation() { return frameIndexInAnimation_; }
    int GetTargetFrameIndex() { return animationFrames_[frameIndexInAnimation_]->GetTargetFrameIndex(); }
    int GetFrameIndex() { return animationFrames_[frameIndexInAnimation_]->GetFrameIndex(); }
    int GetPartIndex();
    int GetAnimationCode() { return pAnimationInfo_->code_; }

    FrameInfo& GetFrameInfo(int _frameIndexInAnimation);
    FrameInfo& GetRunningFrameInfo();
    int GetRunningFrameEventCode();

    bool IsFinished() { return finished_; }
    bool IsPaused() { return paused_ || zeroFramePaused_; }
    bool IsZeroFramePaused() { return zeroFramePaused_; }
    void ReflectAnimation(ActorPartAnimation* _pRunningAnimation);

private:
    FrameTexture* ChangeTexture(int _frameIndexInAnimation);
    FrameTexture* GetTexture(int _frameIndexInAnimation);

private:
    // 주입 데이터
    int sgaIndex_;
    int imgIndex_;

    AnimationInfo* pAnimationInfo_;
    ActorPartSprite* pTarget_;
    jc::Vector<FrameTexture*> animationFrames_;
    jc::Vector<FrameTexture*>& frames_;

    // 자체 데이터
    float runningFrameTime_;
    float pauseDelay_;
    float playSpeed_;
    int frameIndexInAnimation_;
    bool finished_;
    bool paused_;
    bool zeroFramePaused_;
    bool loopSequence_;
};
