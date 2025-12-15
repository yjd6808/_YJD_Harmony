/*
 * 작성자: 윤정도
 * 생성일: 4/26/2023 12:33:39 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "PopupManager.h"

#include <SteinsGate/Client/Define_UI.h>

USING_NS_CC;
USING_NS_CCUI;
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
	releaseAll();
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::createPopup()
{
	UIElementInfo* pInfo = Core::DataManager->getUIElementInfo(GROUP_UI_POPUP);
	DebugAssertMsg(pInfo->Type == UIElementType::Group, "그룹 엘리먼트 타입이 아닙니다.");

	UIRootGroup* pPopupGroup = dbg_new UI_Popup(static_cast<UIGroupInfo*>(pInfo));
	pPopupGroup->autorelease();
	pPopupGroup->setTag(InvalidValue_v);
	pPopupGroup->init();
	pPopupGroup->initChildren();
	pPopupGroup->OnInit();
	pPopupGroup->SetRelativePosition(0, 0, HAlignment::Center, VAlignment::Center);
	pPopupGroup->Load();

	return static_cast<UI_Popup*>(pPopupGroup);
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::setWidth(float _width)
{
	width_ = _width;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::setPadding(float _padding)
{
	padding_ = _padding;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::pop()
{
	UI_Popup* pPopup = nullptr;

	if (popupPool_.IsEmpty())
	{
		pPopup = createPopup();
		pPopup->retain();
	}
	else
	{
		pPopup = popupPool_.Front();
		pPopup->setTag(InvalidValue_v); // 태그 초기화 해줘야함
		popupPool_.Dequeue();
	}

	pPopup->setClosed(false);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showYesNo(
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
	UI_Popup* pPopup = pop();
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eYesNo);
	pPopup->setYesCallback(_yes);
	pPopup->setNoCallback(_no);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showYesNo(
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
	UI_Popup* pOpenedPopup = findByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = pop();
	pPopup->setTag(_tag);
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eYesNo);
	pPopup->setYesCallback(_yes);
	pPopup->setNoCallback(_no);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showOk(
	const std::string& _text,
	const PopupCallback& _ok /* = nullptr */,
	bool _closeWithEsc /* = true */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Left */,
	VAlignment_t _valign /* = VAlignment::Top */
)
{
	UI_Popup* pPopup = pop();
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eOk);
	pPopup->setOkCallback(_ok);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showOk(
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
	UI_Popup* pOpenedPopup = findByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = pop();
	pPopup->setTag(_tag);
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eOk);
	pPopup->setOkCallback(_ok);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showNone(
	const std::string& _text,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Center */,
	VAlignment_t _valign /* = VAlignment::Center */
)
{
	UI_Popup* pPopup = pop();
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eNone);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::showNone(
	const std::string& _text,
	int _tag,
	bool _closeWithEsc /* = false */,
	float _timeout /* = SG_POPUP_NO_TIMEOUT */,
	const PopupCallback& _timeoutFn /* = nullptr */,
	HAlignment_t _halign /* = HAlignment::Center */,
	VAlignment_t _valign /* = VAlignment::Center */
)
{
	UI_Popup* pOpenedPopup = findByTag(_tag);
	if (pOpenedPopup != nullptr)
	{
		return pOpenedPopup;
	}

	UI_Popup* pPopup = pop();
	pPopup->setTag(_tag);
	pPopup->setText(_text);
	pPopup->setType(UI_Popup::Type::eNone);
	pPopup->setCloseWithEsc(_closeWithEsc);
	pPopup->setTimeout(_timeout);
	pPopup->setTimeoutCallback(_timeoutFn);
	pPopup->setTextHAlign(_halign);
	pPopup->setTextVAlign(_valign);
	pPopup->adjust();
	Core::Contents.World->getUILayer()->addUIGroup(pPopup);
	opendList_.PushBack(pPopup);
	return pPopup;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PopupManager::close(UI_Popup* _pPopup)
{
	if (_pPopup->isClosed())
	{
		return false;
	}

	_pPopup->setClosed(true);
	Core::Contents.World->getUILayer()->removeUIGroup(_pPopup);
	popupPool_.Enqueue(_pPopup);
	return opendList_.Remove(_pPopup);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool PopupManager::closeByTag(int _tag)
{
	UI_Popup** pFind = opendList_.Extension().FindIf(
		[_tag](UI_Popup* pPopup) { return pPopup->getTag() == _tag; });

	if (pFind == nullptr)
	{
		return false;
	}

	return close(*pFind);
}

//////////////////////////////////////////////////////////////////////////////////////////
UI_Popup* PopupManager::findByTag(int _tag)
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
int PopupManager::closeAll()
{
	int closedCount = 0;

	for (int i = 0; i < opendList_.Size(); ++i)
	{
		if (close(opendList_[i]))
		{
			--i;
			++closedCount;
		}
	}

	return closedCount;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PopupManager::releaseAll()
{
	// 열린 팝업창 모두 제거
	for (int i = 0; i < opendList_.Size(); ++i)
	{
		Core::Contents.World->getUILayer()->removeUIGroup(opendList_[i]);
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
