/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:40:11 AM
 * =====================
 *
 */
#include "ComboAction.h"

ComboAction::ComboAction(
	const ComboActionFn& runAction, 
	const JCore::String& name, 
	const ComboKeyList& keyList,
	bool cancelable,
	bool moveable)
	: m_fnRunAction(runAction)
	, m_szActionName(name)
	, m_ComboKeys(keyList)
	, m_bCancelable(cancelable)
	, m_bDoing(false)
	, m_bMoveable(moveable) {}

ComboAction::ComboAction(
	const ComboActionFn& runAction, 
	const ComboActionUpdateFn& updateAction,
	const JCore::String& name,
	const ComboKeyList& keyList, 
	bool cancelable,
	bool moveable)
	: ComboAction(runAction, name, keyList, cancelable, moveable)
{
	m_fnUpdateAction = updateAction;
}


void ComboAction::CallRun() {
	DebugAssertMessage(m_fnRunAction, "실행할 러닝 액션이 없습니다.");

	JCore::String szComboKeys;

	for (int i = 0; i < m_ComboKeys.Count(); ++i) {
		szComboKeys += ControlKey::Name[m_ComboKeys[i]];
		szComboKeys += " ";
	}
	printf("%s 액션 실행\n", szComboKeys.Source());

	m_bDoing = true;

	if (m_fnRunAction)
		m_fnRunAction();
}


void ComboAction::CallUpdate(float dt) {

	if (m_fnUpdateAction)
		m_fnUpdateAction(dt);
}

void ComboAction::CallCancel() {
	DebugAssertMessage(m_bCancelable, "취소할 수 없는 액션입니다.");
	m_bDoing = false;

	if (m_bCancelable && m_fnCancelAction)
		m_fnCancelAction();
}



