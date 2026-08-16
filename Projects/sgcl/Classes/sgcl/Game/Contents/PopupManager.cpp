/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:33:39 AM
 * =====================
 * sgui 기반으로 재구현. XML 레이아웃 로드 없이 코드로 팝업을 구성한다.
 */

#include "GameCoreHeader.h"
#include "Game/Contents/PopupManager.h"

#include "sgcl/Game/UI/Host/UIManager.h"

USING_NS_CC;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
PopupManager::PopupManager()
: width_(250.0f)
, padding_(10.0f)
, popupPool_(8)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
PopupManager::~PopupManager()
{
	ReleaseAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::CreatePopup()
{
	UI_Popup* pPopup = UI_Popup::Create();
	pPopup->setTag(InvalidValue_v);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::SetWidth(float _width)
{
	width_ = _width;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::SetPadding(float _padding)
{
	padding_ = _padding;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::Pop()
{
	UI_Popup* pPopup = nullptr;

	if (popupPool_.IsEmpty())
	{
		pPopup = CreatePopup();
		pPopup->retain();
	}
	else
	{
		pPopup = popupPool_.Front();
		pPopup->setTag(InvalidValue_v); // 태그 초기화 해줘야함
		popupPool_.Dequeue();
	}

	pPopup->SetClosed(false);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::Present(UI_Popup* _pPopup)
{
	sgui::UIHost* pHost = g_cUIMgr.GetHost();
	jc_assert_msg(pHost != nullptr, "UI 호스트가 없는 상태에서 팝업을 열 수 없습니다.");

	_pPopup->Adjust();
	pHost->AddWindow(_pPopup, 1000);	// 팝업은 항상 최상위
	_pPopup->OnLoaded();
	_pPopup->OnAdded();
	opendList_.PushBack(_pPopup);
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowYesNo(
	const std::string& _text,
	const PopupCallback& _yes /* = nullptr */,
	const PopupCallback& _no /* = nullptr */,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /*= nullptr*/,
	HAlignment_t _halign /* = HAlignment::Left */,
	VAlignment_t _valign /* = VAlignment::Top */
)
{
	UI_Popup* pPopup = Pop();
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eYesNo);
	pPopup->SetYesCallback(_yes);
	pPopup->SetNoCallback(_no);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowYesNo(
	const std::string& _text,
	int _tag,
	const PopupCallback& _yes /* = nullptr */,
	const PopupCallback& _no /* = nullptr */,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /*= nullptr*/,
	HAlignment_t _halign /* = HAlignment::Left */,
	VAlignment_t _valign /* = VAlignment::Top */
)
{
	UI_Popup* pOpenedPopup = FindByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = Pop();
	pPopup->setTag(_tag);
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eYesNo);
	pPopup->SetYesCallback(_yes);
	pPopup->SetNoCallback(_no);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowOk(
	const std::string& _text,
	const PopupCallback& _ok /* = nullptr */,
	bool _closeWithEsc /* = true */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Left */,
	VAlignment_t _valign /* = VAlignment::Top */
)
{
	UI_Popup* pPopup = Pop();
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eOk);
	pPopup->SetOkCallback(_ok);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowOk(
	const std::string& _text,
	int _tag,
	const PopupCallback& _ok /* = nullptr */,
	bool _closeWithEsc /* = true */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Left */,
	VAlignment_t _valign /* = VAlignment::Top */
)
{
	UI_Popup* pOpenedPopup = FindByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = Pop();
	pPopup->setTag(_tag);
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eOk);
	pPopup->SetOkCallback(_ok);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowNone(
	const std::string& _text,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Center */,
	VAlignment_t _valign /* = VAlignment::Center */
)
{
	UI_Popup* pPopup = Pop();
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eNone);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::ShowNone(
	const std::string& _text,
	int _tag,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Center */,
	VAlignment_t _valign /* = VAlignment::Center */
)
{
	UI_Popup* pOpenedPopup = FindByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = Pop();
	pPopup->setTag(_tag);
	pPopup->SetText(_text);
	pPopup->SetType(UI_Popup::Type::eNone);
	pPopup->SetCloseWithEsc(_closeWithEsc);
	pPopup->SetTimeout(_timeout);
	pPopup->SetTimeoutCallback(_timeoutFn);
	pPopup->SetTextHAlign(_halign);
	pPopup->SetTextVAlign(_valign);
	Present(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PopupManager::Close(UI_Popup* _pPopup)
{
	if (_pPopup->IsClosed())
	{
		return false;
	}

	_pPopup->SetClosed(true);

	if (sgui::UIHost* pHost = g_cUIMgr.GetHost())
	{
		pHost->RemoveWindow(_pPopup);
	}

	popupPool_.Enqueue(_pPopup);
	return opendList_.Remove(_pPopup);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PopupManager::CloseByTag(int _tag)
{
	UI_Popup** pFind = opendList_.FindIf(
		[_tag](UI_Popup* _pPopup) { return _pPopup->getTag() == _tag; });

	if (pFind == nullptr)
	{
		return false;
	}

	return Close(*pFind);
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::FindByTag(int _tag)
{
	for (int i = 0; i < opendList_.Size(); ++i)
	{
		if (opendList_[i]->getTag() == _tag)
		{
			return opendList_[i];
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
int PopupManager::CloseAll()
{
	int closedCount = 0;

	for (int i = 0; i < opendList_.Size(); ++i)
	{
		if (Close(opendList_[i]))
		{
			--i;
			++closedCount;
		}
	}

	return closedCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::ReleaseAll()
{
	// 열린 팝업창 모두 제거
	for (int i = 0; i < opendList_.Size(); ++i)
	{
		if (sgui::UIHost* pHost = g_cUIMgr.GetHost())
		{
			pHost->RemoveWindow(opendList_[i]);
		}

		opendList_[i]->release();
	}

	opendList_.Clear();

	// 풀에 있는 팝업창 모두 제거
	while (!popupPool_.IsEmpty())
	{
		popupPool_.Front()->release();
		popupPool_.Dequeue();
	}
}
