/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Host/UIManager.h"

#include "sgcl/Game/UI/Xml/UIXmlLoader.h"
#include "sg/Core/AppConfig.h"
#include "jc/IO/Path.h"
#include "jc/IO/File.h"

USING_NS_CC;
USING_NS_JC;

namespace sgui
{

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Init()
{
	// 윈도우 팩토리는 REGISTER_WINDOW 매크로(정적 초기화)로 등록된다.
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::Free()
{
	factories_.clear();
	pHost_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void UIManager::RegisterWindowFactory(const char* _name, const WindowFactory& _factory)
{
	if (_name == nullptr || !_factory)
	{
		_LogWarn_("[UIManager] 잘못된 윈도우 팩토리 등록입니다. (name: %s)", _name ? _name : "null");
		return;
	}

	factories_[_name] = _factory;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String UIManager::ResolveXmlPath(const char* _name) const
{
	return jc::Path::Combine(
		jc::Path::Combine(g_cAppConfig.resDataPath_, "layout"),
		jc::String(_name) + ".xml");
}

//////////////////////////////////////////////////////////////////////////////////////////
bool UIManager::HasXml(const char* _name) const
{
	return jc::File::Exist(ResolveXmlPath(_name).Source());
}

//////////////////////////////////////////////////////////////////////////////////////////
Window* UIManager::Show(const char* _name, const CDataMap<>& _dataMap /* = {} */, int _zOrder /* = 0 */)
{
	if (_name == nullptr)
	{
		_LogError_("[UIManager] 윈도우 이름이 null입니다.");
		return nullptr;
	}

	// 등록된 코드 빌드 팩토리로 인스턴스를 생성한다. (없으면 nullptr)
	Window* pWindow = nullptr;
	auto it = factories_.find(_name);
	if (it != factories_.end())
	{
		pWindow = it->second();
	}

	// 주의: 여기서 autorelease()를 호출하면 안 된다.
	// 윈도우는 이미 생성자(팩토리: REGISTER_WINDOW 또는 UIXmlLoader::LoadWindowFromFile)에서
	// 한 번 autorelease 되었고, 여기서 또 호출하면 AutoreleasePool::clear() 시 2회 release 되어
	// rc=0으로 소멸 시점에 씬 트리에서 running 상태라 Node::~Node assert가 발생한다.
	// 소유권: new(1) + addChild(1) = rc 2, 풀 정리 시 1회 release -> rc 1 (addChild 소유)
	// WPF 스타일 XML(layout/{name}.xml)이 존재하면 인스턴스에 병합한다. (XML 우선)
	// - 팩토리 인스턴스(UI_* 코드 빌드 윈도우)가 있으면 그 위에 XML 트리를 올리고, OnInit()에서
	//   IsXmlLoaded()로 구분해 FindElementByName 멤버 할당(주) 또는 코드 빌드 폴백(보조)을 수행한다.
	// - 팩토리가 없으면 순수 XML 윈도우를 생성한다.
	const jc::String filePath = ResolveXmlPath(_name);
	if (jc::File::Exist(filePath.Source()))
	{
		if (pWindow)
		{
			if (!UIXmlLoader::LoadWindowInto(pWindow, filePath.Source()))
			{
				_LogError_("[UIManager] XML 레이아웃 병합 실패: %s (코드 빌드 폴백)", filePath.Source());
			}
		}
		else
		{
			pWindow = UIXmlLoader::LoadWindowFromFile(filePath.Source());
		}
	}

	if (pWindow == nullptr)
	{
		_LogError_("[UIManager] 윈도우를 생성할 수 없습니다: %s (등록 팩토리 없음 + XML 없음)", _name);
		return nullptr;
	}

	return ShowWindow(pWindow, _dataMap, _zOrder);
}

//////////////////////////////////////////////////////////////////////////////////////////
Window* UIManager::ShowXml(const char* _name, const CDataMap<>& _dataMap /* = {} */, int _zOrder /* = 0 */)
{
	if (_name == nullptr)
	{
		_LogError_("[UIManager] 윈도우 이름이 null입니다.");
		return nullptr;
	}

	const jc::String filePath = ResolveXmlPath(_name);

	if (!jc::File::Exist(filePath.Source()))
	{
		_LogError_("[UIManager] XML 레이아웃 파일이 존재하지 않습니다: %s", filePath.Source());
		return nullptr;
	}

	Window* pWindow = UIXmlLoader::LoadWindowFromFile(filePath.Source());
	if (pWindow == nullptr)
	{
		_LogError_("[UIManager] XML 레이아웃 로드에 실패했습니다: %s", filePath.Source());
		return nullptr;
	}

	return ShowWindow(pWindow, _dataMap, _zOrder);
}

//////////////////////////////////////////////////////////////////////////////////////////
Window* UIManager::ShowWindow(Window* _pWindow, const CDataMap<>& _dataMap, int _zOrder)
{
	if (_pWindow == nullptr)
	{
		return nullptr;
	}

	UIHost* pHost = GetHost();
	if (pHost == nullptr)
	{
		_LogError_("[UIManager] UI 호스트가 설정되지 않았습니다. SetHost()를 먼저 호출하세요.");
		CC_SAFE_DELETE(_pWindow);
		return nullptr;
	}

	_pWindow->OnInit(_dataMap);
	pHost->AddWindow(_pWindow, _zOrder);
	_pWindow->OnLoaded();
	_pWindow->OnAdded();
	return _pWindow;
}

} // namespace sgui
