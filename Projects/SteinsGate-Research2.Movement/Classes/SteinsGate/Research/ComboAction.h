/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:41 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/ComboKeyList.h>

#include <JCore/Container/Vector.h>

#include <JCore/Primitives/String.h>
#include <JCore/Primitives/SmartPtr.h>

using ComboActionFn			= std::function<void()>;
using ComboActionUpdateFn	= std::function<void(float)>;

class ComboAction
{
public:
	ComboAction(
		const ComboActionFn& runAction, 
		const JCore::String& name, 
		const ComboKeyList& keyList, 
		bool cancelable,
		bool moveable);

	ComboAction(
		const ComboActionFn& runAction,
		const ComboActionUpdateFn& updateAction,
		const JCore::String& name, 
		const ComboKeyList& keyList, 
		bool cancelable,
		bool moveable);
public:
	void CallRun();
	void CallUpdate(float dt);
	void CallCancel();
	bool HasRunAction()		{ return m_fnRunAction != nullptr;		}
	bool HasUpdateAction()	{ return m_fnUpdateAction != nullptr;	}
	bool HasCancelAction()	{ return m_fnCancelAction != nullptr;	}
	void SetRunAction(const ComboActionFn& runAction)				{ m_fnRunAction = runAction; }
	void SetUpdateAction(const ComboActionUpdateFn& updateAction)	{ m_fnUpdateAction = updateAction; }
	void SetCancelAction(const ComboActionFn& cancelAction)			{ m_fnCancelAction = cancelAction; }
	void Done()			{ m_bDoing = false; }

	bool IsDoing()		{ return m_bDoing;  }
	bool IsCancelable() { return m_bCancelable; }
	bool IsMoveable()	{ return m_bMoveable; }
private:
	JCore::String		m_szActionName;
	ComboActionFn		m_fnRunAction;			// 콤보 완성시 수행될 액션
	ComboActionUpdateFn m_fnUpdateAction;		// 동작 수행중에 지속적으로 수행될 액션
	ComboActionFn		m_fnCancelAction;		// 이 액션이 캔슬시 수행될 액션

	ComboKeyList		m_ComboKeys;
	bool m_bCancelable;							// 액션 캔슬 가능여부
	bool m_bDoing;								// 액션이 실행중인지
	bool m_bMoveable;							// 액션중 움직일 수 있는지

	friend class ComboTree;
	friend class ComboTreeNode;
};

using ComboActionPtr = JCore::SharedPtr<ComboAction>;



