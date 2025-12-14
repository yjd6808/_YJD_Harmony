/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:50:32 AM
 * =====================
 * 액터 노드 관계도
 *
 *                      SGActor
 *                         |
 *               ┌─────────┴────────────┐
 *         SGDrawNode(두께 박스)      SGActorSprite
 *                                         |
 *                                     캔버스 노드
 *                                         |
 *                               SGActorPartSprite
 *                                         |
 *                               SGDrawNode(히트박스)        
 *
 */


#pragma once

#include <SteinsGate/Client/Tutturu.h>

#include <SteinsGate/Client/ActorSprite.h>
#include <SteinsGate/Client/HitRecorder.h>
#include <SteinsGate/Client/ActorListenerCollection.h>

#include <SteinsGate/Client/ComponentCollection.h>

class Actor;
class MapLayer;

class Actor : public SGNode
{
	JCORE_HIDE_BASE_CLASS_METHOD(SGNode, addComponent)
	JCORE_HIDE_BASE_CLASS_METHOD(SGNode, init)
	JCORE_HIDE_BASE_CLASS_METHOD(SGNode, cleanup)
public:
	enum CleanUpFlag
	{
		CF_None                 = 0,
		CF_ReleaseActorSprite   = 1
	};

	Actor();
	~Actor() override;

	virtual void initialize() = 0;
	virtual void initActorSprite() = 0;
	virtual void onFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void onFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void onAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void onAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void initThicknessBox(const ThicknessBox& _thicknessBox);
	virtual void initHitRecorder(int _hitPossibleListSize = 16, int _alreadyHitMapSize = 32, Actor* _pOwner = nullptr);
	virtual void hit(const HitInfo& _hitInfo);

	virtual bool initVariables();
	virtual void initComponents() = 0;
	virtual void initListeners() = 0;

	bool addListener(IActorListener* _pListener);
	bool hasListener(IActorListener::Type _type);
	IActorListener* getListener(IActorListener::Type _type);
	void update(float _dt) override;

	void addComponent(IComponent* _pComponent);
	bool hasComponent(IComponent::Type _type) const;
	template <typename TComponent>
	TComponent* getComponent() const { return components_.get<TComponent>(); }

	virtual ActorType_t getType() const = 0;
	const char* getTypeName() const;
	ActorRect getActorRect() const;
	SGRect getThicknessBoxRect() const;
	SGVec2 getPositionReal() const;
	float getPositionRealX() const;
	float getPositionRealY() const;
	float getPositionActorY() const;
	SGVec2 getPositionRealCenter() const;
	float getPositionRealCenterX() const;
	float getPositionRealCenterY() const;
	SGVec2 getCanvasPositionReal() const;
	SGSize getCanvasSize() const;
	SGRect getHitBox() const;
	ThicknessBox getThicknessBox() const;
	SGDrawNode* getThicknessBoxNode() const;
	ActorSprite* getActorSprite() const;
	SpriteDirection_t getSpriteDirection() const;
	int getRunningAnimationCode();
	ActorPartAnimation* getRunningAnimation();
	int getAllyFlag() { return allyFlag_; }
	MapLayer* getMapLayer() { return mapLayer_; }
	HitRecorder* getHitRecorder() { return hitRecorder_; }
	int getActorId() { return actorId_; }
	virtual int getCode() = 0;
	ActorListenerCollection& getListenerCollection() { return listeners_; }

	void setPositionReal(float _x, float _y);
	void setPositionReal(const SGVec2& _position);
	void setPositionRealX(float _x);
	void setPositionRealY(float _y);
	void setPositionRealCenter(float _x, float _y);
	void setPositionRealCenter(const SGVec2& _position);
	void setAllyFlag(int _flag) { allyFlag_ = _flag; }
	void setMapLayer(MapLayer* _pMapLayer);
	void setActorId(int _id);
	void setCleanUpFlag(int _flag) { cleanUpFlag_ = _flag; }
	bool hasCleanUpFlag(int _cleanUpFlag);
	void addCleanUpFlag(int _cleanUpFlag);
	void clearCleanUpFlag();

	void runAnimation(int _animationCode);
	void runAnimation(int _animationCode, int _startFrameIndexInAnimation);
	void pauseAnimation(float _delay);

	void runFrameEventSpawn(FrameEventSpawnType_t _frameEventSpawnType, int _code);
	void runFrameEvent(FrameEvent* _pFrameEvent);
	void runFrameEvent(int _frameEventCode);

	void setSpriteDirection(SpriteDirection_t _direction);
	void setForwardDirection();
	void setBackwardDirection();

	bool isCollide(Actor* _pOther, JCORE_OUT SpriteDirection_t& _otherHitDirection, JCORE_OUT SGRect& _hitRect);
	bool isCollide(const ActorRect& _otherRect, JCORE_OUT SpriteDirection_t& _otherHitDirection, JCORE_OUT SGRect& _hitRect);
	bool isCollide(const ActorRect& _otherRect);
	bool isOnTheGround();

	void releaseActorSprite();

	void cleanUpAtNextFrame();
	void cleanUp();

	void attach(Actor* _pActor);
	void detach(Actor* _pActor);
	Actor* getAttacher();
	bool hasAttacher();
public:
	// stdActor기준으로 절대 액터 렉트를 얻도록 해줌
	static ActorRect convertAbsoluteActorRect(Actor* _pStdActor, const ActorRect& _relativeRect);
private:
	AIActivity* getRunningAIActivity();
protected:
	JCORE_NOT_NULL MapLayer* mapLayer_;
	JCORE_NOT_NULL ActorSprite* actorSprite_;
	JCORE_NULLABLE HitRecorder* hitRecorder_;

	ActorListenerCollection listeners_;
	ComponentCollection components_;

	int actorId_;
	int allyFlag_;
	int cleanUpFlag_;
	bool cleanUp_;

	JCORE_NOT_NULL SGDrawNode* thicknessBox_;
	JCORE_NULLABLE Actor* attacher_;
	SGVector<Actor*> attaches_;
};
