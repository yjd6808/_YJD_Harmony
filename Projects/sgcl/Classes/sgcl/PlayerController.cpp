/*
 * 작성자: 윤정도
 * 생성일: 1/22/2023 8:59:10 AM
 * =====================
 *
 */

#include "Core.h"
#include "PlayerController.h"

#include <sg/_Struct/SteinsGate_Map.h>

#include <sgcl/HostPlayer.h>
#include <sgcl/SGAction.h>
#include <sgcl/_Layer/Layer_Map.h>
#include <sgcl/MoveComponent.h>

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerController* PlayerController::Create(HostPlayer* _pPlayer, ActionMgr* _pActionManager)
{
	PlayerController* pController = dbg_new PlayerController(_pPlayer, _pActionManager);
	pController->Init();
	return pController;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerController::PlayerController(HostPlayer* _pPlayer, ActionMgr* _pActionManager)
: player_(_pPlayer)
, actionManager_(_pActionManager)
, pressedArrowKeyState_(4)
{
	Init();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
PlayerController::~PlayerController()
{
	_LogDebug_("플레이어 컨트롤러 소멸");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::Init()
{
	for (int index = 0; index < int(cc::EventKeyboard::KeyCode::MAX); ++index)
	{
		cocosKeyCodeToControlKeyMap_[index] = ControlKey::None;
	}

	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_LEFT_ARROW)] = ControlKey::Left;
	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_RIGHT_ARROW)] = ControlKey::Right;
	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_UP_ARROW)] = ControlKey::Up;
	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_DOWN_ARROW)] = ControlKey::Down;
	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_X)] = ControlKey::Attack;
	cocosKeyCodeToControlKeyMap_[int(cc::EventKeyboard::KeyCode::KEY_C)] = ControlKey::Jump;
	cabUseCommand_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::Update(float _delta)
{
	UpdateMove(_delta);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	ControlKey_t pressedControlKey = cocosKeyCodeToControlKeyMap_[int(_keyCode)];
	if (pressedControlKey == ControlKey::None)
		return;
	actionManager_->OnKeyPressedBefore(this, _keyCode);
	OnKeyPressed(pressedControlKey);
	actionManager_->OnKeyPressed(this, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cocos2d::Event* _pEvent)
{
	ControlKey_t releasedControlKey = cocosKeyCodeToControlKeyMap_[int(_keyCode)];
	if (releasedControlKey == ControlKey::None)
		return;
	actionManager_->OnKeyReleasedBefore(this, _keyCode);
	OnKeyReleased(releasedControlKey);
	actionManager_->OnKeyReleased(this, _keyCode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::OnKeyPressed(ControlKey_t _pressedKey)
{
	controlKeyPressedMap_[_pressedKey] = true;
	DateTime now = DateTime::Now();

	lastestPressedKey_.controlKey_ = _pressedKey;
	lastestPressedKey_.time_ = now;

	if (CannotUseCommand())
		return;

	// 덮어쓰기 방지를 위해 뒤에서부터 복사
	for (int index = Const::Action::ComboSequenceCount - 2; index >= 0; --index)
	{
		controlKeySequence_[index + 1] = controlKeySequence_[index];
	}

	controlKeySequence_[0].time_ = now;
	controlKeySequence_[0].controlKey_ = _pressedKey;

	CheckComboSequence();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::OnKeyReleased(ControlKey_t _releasedKey)
{
	controlKeyPressedMap_[_releasedKey] = false;
	lastestReleasedKey_.controlKey_ = _releasedKey;
	lastestReleasedKey_.time_ = DateTime::Now();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteDirection_t PlayerController::GetSpriteDirection()
{
	return player_->GetActorSprite()->GetSpriteDirection();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerController::IsKeyPressed(ControlKey_t _controlKey)
{
	return controlKeyPressedMap_[int(_controlKey)];
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PlayerController::IsMoveKeyPressed()
{
	if (IsKeyPressed(ControlKey::Left) ||
		IsKeyPressed(ControlKey::Right) ||
		IsKeyPressed(ControlKey::Up) ||
		IsKeyPressed(ControlKey::Down))
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::CheckComboSequence()
{
	ComboKeyList sequence;

	// 우선 현재 입력한 키는 무조건 콤보에 포함되어야하므로
	int sequenceCount = 1;
	DateTime currentTime = controlKeySequence_[0].time_;
	sequence[0] = controlKeySequence_[0].controlKey_;

	for (int index = 1; index <= Const::Action::ComboSequenceCount; ++index, ++sequenceCount)
	{
		DateTime& beforeTime = controlKeySequence_[index].time_;
		float diffSeconds = static_cast<float>(currentTime.Diff(beforeTime).GetTotalSeconds());

		if (diffSeconds > Const::Action::ComboSequenceDelay)
		{
			break;
		}

		sequence[index] = controlKeySequence_[index].controlKey_;
		currentTime = beforeTime;
	}

	for (int index = 0; index < sequenceCount; ++index)
	{
		sequence[index] = controlKeySequence_[index].controlKey_;
	}

	if (sequenceCount == 0)
		return;

	SGAction* pAction;

	// 없는 경우 위로 거슬러 올라가서 확인
	// 예를 들어 -> -> X를 입력했는데 키가 바인딩 되어있지 않으면
	// 음..		-> -> 를 검사하고 키가 바인딩 되어있지 않으면
	//          ->    를 검사한다.
	// 
	// ↑ -> -> 키를 순서대로 입력했을 때
	//   -> -> 키가 바인딩 되어있음에도 불구하고 실행 안되는 경우를 방지하기 위함
	while ((pAction = actionManager_->GetComboAction(sequence)) == nullptr && sequenceCount > 0)
	{
		sequence[--sequenceCount] = ControlKey::None;
	}

	if (pAction != nullptr)
	{
		actionManager_->RunAction(pAction);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::Idle()
{
	actionManager_->RunBaseAction(BaseAction::Idle);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::Walk()
{
	actionManager_->RunBaseAction(BaseAction::Walk);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::UpdateMove(float _delta)
{
	MoveComponent* pMoveComponent = player_->GetComponent<MoveComponent>();

	if (pMoveComponent == nullptr)
		return;

	SGAction* pRunningAction = actionManager_->GetRunningAction();

	// 액션중 이동가능한 액션인 경우 해당 액션의 이동속도로 움직일 수 있도록 한다.
	if (pRunningAction == nullptr)
	{
		pMoveComponent->SetSpeed(0, 0);
		return;
	}

	float speedX = 0.0f;
	float speedY = 0.0f;

	if (IsKeyPressed(ControlKey::Left) && pRunningAction->IsMoveableNegativeX())
	{
		speedX = pRunningAction->GetMoveSpeedX() / 60.0f * -1;
	}
	else if (IsKeyPressed(ControlKey::Right) && pRunningAction->IsMoveablePositiveX())
	{
		speedX = pRunningAction->GetMoveSpeedX() / 60.0f;
	}

	if (IsKeyPressed(ControlKey::Up) && pRunningAction->IsMoveablePositiveY())
	{
		speedY = pRunningAction->GetMoveSpeedY() / 60.0f;
	}
	else if (IsKeyPressed(ControlKey::Down) && pRunningAction->IsMoveableNegativeY())
	{
		speedY = pRunningAction->GetMoveSpeedY() / 60.0f * -1;
	}

	pMoveComponent->SetSpeed(speedX, speedY);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerController::UpdateDirection(ControlKey_t _pressedKey)
{
	// 방향전환 가능 여부
	if (_pressedKey == ControlKey::Right)
	{
		player_->SetForwardDirection();
	}
	else if (_pressedKey == ControlKey::Left)
	{
		player_->SetBackwardDirection();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 1. 스킬시전 중 꾹 누른키에 대한 처리를 액션이 끝난 후 키 입력 상태를 확인해서 상태를 변경해줘야한다. 처리해줘야한다.
// 2. 액션이 없는 경우 키 입력시마다 반영해줘야한다.
void PlayerController::ReflectPressedMoveKeys()
{
	// 액션 수행중 방향키로 움직일 수 있는 액션인 경우라도 걷기 나 평상시 애니메이션이 실행되어선 안된다.
	// 방향키만 읽음
	pressedArrowKeyState_.Clear();

	for (int directionIndex = 0; directionIndex < Direction::Max; ++directionIndex)
	{
		if (IsKeyPressed(ControlKey_t(directionIndex)))
		{
			pressedArrowKeyState_.PushBack(ControlKey_t(directionIndex));
		}
	}

	if (pressedArrowKeyState_.Size() == 0)
	{
		Idle();
		return;
	}

	// 동시 키입력 소거
	// 남은건 대각이동 또는 한가지 방향밖에 없다.
	if (pressedArrowKeyState_.Exist(ControlKey::Left) && pressedArrowKeyState_.Exist(ControlKey::Right))
	{
		pressedArrowKeyState_.Remove(ControlKey::Left);
		pressedArrowKeyState_.Remove(ControlKey::Right);
	}

	if (pressedArrowKeyState_.Exist(ControlKey::Up) && pressedArrowKeyState_.Exist(ControlKey::Down))
	{
		pressedArrowKeyState_.Remove(ControlKey::Up);
		pressedArrowKeyState_.Remove(ControlKey::Down);
	}

	if (pressedArrowKeyState_.Exist(ControlKey::Right))
	{
		UpdateDirection(ControlKey::Right);
	}
	else if (pressedArrowKeyState_.Exist(ControlKey::Left))
	{
		UpdateDirection(ControlKey::Left);
	}

	Walk();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
ControlKey_t PlayerController::ConvertControlKey(cc::EventKeyboard::KeyCode _keyCode)
{
	return cocosKeyCodeToControlKeyMap_[int(_keyCode)];
}
