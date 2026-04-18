/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 8:50:32 AM
 * =====================
 * 액터 노드 관계도
 *
 *                      SGActor
 *                         |
 *               ┌─────────┴────────────┐
 *           cc::DrawNode(두께 박스)      SGActorSprite
 *                                         |
 *                                     캔버스 노드
 *                                         |
 *                               SGActorPartSprite
 *                                         |
 *                               cc::DrawNode(히트박스)        
 *
 */

#pragma once


#include "sgcl/Game/Actor/ActorSprite.h"
#include "sgcl/Game/Util/HitRecorder.h"
#include "sgcl/Game/Listener/ActorListenerCollection.h"

#include "sgcl/Base/ComponentCollection.h"

class Actor;
class MapLayer;

struct HitInfo;

class Actor : public cc::Node
{
	JC_HIDE_BASE_CLASS_METHOD(cc::Node, addComponent)
	JC_HIDE_BASE_CLASS_METHOD(cc::Node, init)
	JC_HIDE_BASE_CLASS_METHOD(cc::Node, cleanup)

public:
	enum CleanUpFlag
	{
		cfNone = 0,
		cfReleaseActorSprite = 1
	};

	Actor();
	~Actor() override;

	virtual void			Initialize() = 0;
	virtual void			InitActorSprite() = 0;
	virtual void			OnFrameBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void			OnFrameEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void			OnAnimationBegin(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void			OnAnimationEnd(ActorPartAnimation* _pAnimation, FrameTexture* _pTexture);
	virtual void			InitThicknessBox(const ThicknessBox& _thicknessBox);
	virtual void			InitHitRecorder(int _hitPossibleListSize = 16, int _alreadyHitMapSize = 32, Actor* _pOwner = nullptr);
	virtual void			Hit(const HitInfo& _hitInfo);

	virtual bool			InitVariables();
	virtual void			InitComponents() = 0;
	virtual void			InitListeners() = 0;

	bool					AddListener(IActorListener* _pListener);
	bool					HasListener(IActorListener::Type _type);
	IActorListener*			GetListener(IActorListener::Type _type);
	void					update(float _dt) override; // 자식에서도 오버라이딩시 이거 호출하도록

	void					AddComponent(IComponent* _pComponenet);
	bool					HasComponent(IComponent::Type _type) const;

	template <typename TComponent>
	TComponent*				GetComponent() const { return components_.Get<TComponent>(); }

	virtual ActorType_t		GetType() const = 0;
	const char*				GetTypeName() const;
	ActorRect				GetActorRect() const;
	cc::rect				GetThicknessBoxRect() const;
	cc::vec2				GetPositionReal() const;
	float					GetPositionRealX() const;
	float					GetPositionRealY() const;
	float					GetPositionActorY() const;
	cc::vec2				GetPositionRealCenter() const;
	float					GetPositionRealCenterX() const;
	float					GetPositionRealCenterY() const;
	cc::vec2				GetCanvasPositionReal() const;
	cc::size				GetCanvasSize() const;
	cc::rect				GetHitBox() const;
	ThicknessBox			GetThicknessBox() const;
	cc::DrawNode*			GetThicknessBoxNode() const;
	ActorSprite*			GetActorSprite() const;
	SpriteDirection_t		GetSpriteDirection() const;

	int						GetRunningAnimationCode();
	ActorPartAnimation*		GetRunningAnimation();
	int						GetAllyFlag() { return allyFlag_; }
	MapLayer*				GetMapLayer() { return pMapLayer_; }
	HitRecorder*			GetHitRecorder() { return pHitRecorder_; }
	int						GetActorId() { return actorId_; }
	virtual int				GetCode() = 0;
	ActorListenerCollection& GetListenerCollection() { return listeners_; }

	void					SetPositionReal(float _x, float _y);
	void					SetPositionReal(const cc::vec2& _v);
	void					SetPositionRealX(float _x);
	void					SetPositionRealY(float _y);
	void					SetPositionRealCenter(float _x, float _y);
	void					SetPositionRealCenter(const cc::vec2& _v);
	void					SetAllyFlag(int _flag) { allyFlag_ = _flag; }
	void					SetMapLayer(MapLayer* _pMapLayer);
	void					SetActorId(int _id);
	void					SetCleanUpFlag(int _flag) { cleanUpFlag_ = _flag; }
	bool					HasCleanUpFlag(int _cleanUpFlag);
	void					AddCleanUpFlag(int _cleanUpFlag);
	void					ClearCleanUpFlag();

	void					RunAnimation(int _animationCode);
	void					RunAnimation(int _animationCode, int _startFrameIndexInAnimation);
	void					PauseAnimation(float _delay);

	void					RunFrameEventSpawn(FrameEventSpawnType_t _frameEventSpawnType, int _code);
	void					RunFrameEvent(FrameEvent* _pFrameEvent);
	void					RunFrameEvent(int _frameEventCode);

	void					SetSpriteDirection(SpriteDirection_t _direction);
	void					SetForwardDirection();
	void					SetBackwardDirection();

	bool					IsCollide(Actor* _pOther, OUT SpriteDirection_t& _otherHitDirection, OUT cc::rect& _hitRect);
	bool					IsCollide(const ActorRect& _otherRect, OUT SpriteDirection_t& _otherHitDirection, OUT cc::rect& _hitRect);
	bool					IsCollide(const ActorRect& _otherRect);
	bool					IsOnTheGround();

	void					ReleaseActorSprite(); // 액터의 텍스쳐를 수동으로 제거하기 위해

	void		CleanUpAtNextFrame();
	void		CleanUp();

	void		Attach(Actor* _pActor); // 이 액터에 전달받은 액터를 붙임
	void		Detach(Actor* _pActor); // 이 액터에게서 전달받은 액터를 떼어냄
	Actor*		GetAttacher();
	bool		HasAttacher();

public:
	// stdActor기준으로 절대 액터 렉트를 얻도록 해줌
	static ActorRect ConvertAbsoluteActorRect(Actor* _pStdActor, const ActorRect& _relativeRect);

private:
	AIActivity* GetRunningAiActivity();

protected:
	NOT_NULL MapLayer* pMapLayer_;
	NOT_NULL ActorSprite* pActorSprite_;
	NULLABLE HitRecorder* pHitRecorder_;

	ActorListenerCollection listeners_;
	ComponentCollection components_;

	int actorId_; // 액터 박스에게서 부여된 고유 ID
	int allyFlag_; // 값이 같으면 동맹, 다르면 적
	int cleanUpFlag_; // 클린업 될 때 수행할 작업 플래그
	bool isCleanUp_; // 액터가 될 예정 혹은 제거되었는지 여부

	NOT_NULL cc::DrawNode* pThicknessBox_; // TODO: Debug 시각화를 위해 노드로 표현하였다. 추후에는 단순 렉트로 표현해도 됨
	NULLABLE Actor* pAttacher_; // 내가 누구에게 붙어있는가?
	jc::Vector<Actor*> attches_; // 내게 붙어 있는 녀석들
};
