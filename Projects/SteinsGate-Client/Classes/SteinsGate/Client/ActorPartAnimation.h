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

#include <SteinsGate/Client/Tutturu.h>
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
        ActorPartSprite* _pAnimationTarget,
        AnimationInfo* _pAnimationInfo,
        SGVector<FrameTexture*>& _frames
    );

    ActorPartAnimation(
        ActorPartSprite* _pAnimationTarget,
        AnimationInfo* _pAnimationInfo,
        SGVector<FrameTexture*>& _frames
    );

    void constructFrames(int _sgaIndex, int _imgIndex);
    void update(float _dt);
    void updateLoopSequence(float _dt);
    void updateAnimation(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture, float _dt);
    void updateZeroDelayFrame(float _currentFrameDelay, FrameTexture* _pCurrentFrameTexture);
    void init();
    void run();
    void run(int _frameIndexInAnimation);
    void pause();
    void pauseTime(float _delay);
    void resume();
    void setPlaySpeed(float _speed);
    void setLoopSequence();
    void setAnimationInfo(AnimationInfo* _pAnimationInfo);

    AnimationInfo* getAnimationInfo() { return animationInfo_; }

    int getFrameIndexInAnimation() { return frameIndexInAnimation_; }
    int getTargetFrameIndex() { return animationFrames_[frameIndexInAnimation_]->getTargetFrameIndex(); }
    int getFrameIndex() { return animationFrames_[frameIndexInAnimation_]->getFrameIndex(); }
    int getPartIndex();
    int getAnimationCode() { return animationInfo_->Code; }

    FrameInfo& getFrameInfo(int _frameIndexInAnimation);
    FrameInfo& getRunningFrameInfo();
    int getRunningFrameEventCode();

    bool isFinished() { return finished_; }
    bool isPaused() { return paused_ || zeroFramePaused_; }
    bool isZeroFramePaused() { return zeroFramePaused_; }
    void reflectAnimation(ActorPartAnimation* _pRunningAnimation);

private:
    FrameTexture* changeTexture(int _frameIndexInAnimation);
    FrameTexture* getTexture(int _frameIndexInAnimation);

private:
    // 주입 데이터
    int sgaIndex_;
    int imgIndex_;

    AnimationInfo* animationInfo_;
    ActorPartSprite* target_;
    SGVector<FrameTexture*> animationFrames_;
    SGVector<FrameTexture*>& frames_;

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
