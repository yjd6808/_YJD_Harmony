#pragma once

#include <unordered_map>
#include <string>
#include "sgcl/Game/UI/UIElement.h"

class UIManager;
class ImagePackManager;
class DataManager;
class UIButton;
class UISprite;
class UILabel;
class UICheckBox;
class UIEditBox;
class UIToggleButton;
class UIProgressBar;
class UIScrollBar;
class UIStatic;

class UIGroup : public UIElement
{
	JC_HIDE_BASE_CLASS_METHOD(Node, addChild)

	struct CursorPositionGuard
	{
		CursorPositionGuard(cc::EventMouse* _pMouseEvent, const cc::vec2& _cursorPosition)
		: pEvent_(_pMouseEvent)
		, resetCursorPosition_(_cursorPosition)
		{
		}

		~CursorPositionGuard()
		{
			if (pEvent_ == nullptr)
			{
				return;
			}

			pEvent_->setCursorPosition(resetCursorPosition_);
		}

		cc::EventMouse* pEvent_;
		cc::vec2 resetCursorPosition_;
	};

public:
	UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent);
	UIGroup(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner);
	~UIGroup() override;

	static UIGroup* Create(UIRootGroup* _pRoot, UIGroup* _pParent);
	static UIGroup* Create(UIRootGroup* _pRoot, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner);

	static constexpr UIElementType_t Type() { return UIElementType::Group; }

	bool init() override;
	void InitChildren();
	void InitChildrenPosition();

	void Load() override;
	void Unload() override;
	void RefreshThemeVisuals() override { RefreshThemeVisualsRecursive(); }
	void RefreshThemeVisualsRecursive();

	void AddChild(UIElement* _pChild);

	// XML-based initialization
	void InitFromXml();

	// Name-based element lookup
	UIElement* FindElementByName(const char* _name);

	template<typename T>
	T* FindElementByName(const char* _name)
	{
		UIElement* pElement = FindElementByName(_name);
		if (!pElement)
			return nullptr;
		if (pElement->GetElementType() != T::Type())
		{
			_LogWarn_("%s 엘리먼트를 %s 타입으로 찾았지만 타입이 다릅니다.", _name, UIElementType::Name[T::Type()]);
			return nullptr;
		}
		return static_cast<T*>(pElement);
	}

	// 이하 자식들 오버라이딩을 금하기 위해 파이널로 처리
	bool OnMouseDownInternal(cc::EventMouse* _pMouseEvent) final;
	bool OnMouseMoveInternal(cc::EventMouse* _pMouseEvent) final;
	bool OnMouseUpInternal(cc::EventMouse* _pMouseEvent) final;
	bool OnMouseScrollInternal(cc::EventMouse* _pMouseEvent) final;

	virtual void OnUpdate(float _dt)
	{
	}

	virtual bool OnKeyPressed(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) { return true; }
	virtual bool OnKeyReleased(cc::EventKeyboard::KeyCode _keyCode, cc::Event* _pEvent) { return true; }

	UIElementType_t GetElementType() override { return UIElementType::Group; }
	jc::String ToString() override { return jc::StringUtil::Format("그룹(%s)", groupInfo_->name_); }
	bool IsGroup() const override { return true; }

	UIElement* GetAt(int _index);

	template <typename TElem>
	TElem* GetAtTemplated(int _index)
	{
		if (_index >= _children.size())
		{
			_LogWarn_("%s 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->name_, _index);
			return nullptr;
		}

		UIElement* pElement = static_cast<UIElement*>(_children.at(_index));
		if (pElement->GetElementType() != TElem::Type())
		{
			_LogWarn_("%s 그룹에서 %d번째 인덱스 원소가 캐스팅할려는 타입과 다릅니다.", pBaseInfo_->name_, _index);
			return nullptr;
		}

		return static_cast<TElem*>(pElement);
	}

	void AddUIElement(UIGroupElemInfo* _pGroupElemInfo);

	template <typename TElem>
	void ForEachRecursiveSpecificType(const jc::Action<TElem*>& _action) const
	{
		static_assert(!jc::IsPointerType_v<TElem>, "... TElem must not be pointer type");
		static_assert(jc::IsBaseOf_v<UIElement, TElem>, "... TElem must be derived from UIElemenet type");

		for (int i = 0; i < _children.size(); ++i)
		{
			UIElement* pElement = static_cast<UIElement*>(_children.at(i));
			TElem* pCastElement = dynamic_cast<TElem*>(pElement);

			if (pCastElement)
			{
				_action(pCastElement);
			}

			if (pElement->GetElementType() == UIElementType::Group)
			{
				UIGroup* pGroupElement = static_cast<UIGroup*>(pElement);
				pGroupElement->ForEachRecursiveSpecificType<TElem>(_action);
			}
		}
	}

	void ForEachRecursive(const jc::Action<UIElement*>& _action) const;
	void ForEachRecursiveContainedSelf(const jc::Action<UIElement*>& _action) const;
	void ForEach(const jc::Action<UIElement*>& _action) const;
	void ForEachContainedSelf(const jc::Action<UIElement*>& _action) const;

	void RestoreState(State _state) override;
	void ResetChildrenPosition() { InitChildrenPosition(); }

	void SetUISize(const cc::size& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void SetInfoGroup(UIGroupInfo* _pInfo, bool _infoOwner);

protected:
	static UIElement* FindElementRecursiveInternal(UIGroup* _pParent, const char* _name);

protected:
	UIGroupInfo* groupInfo_;
	std::unordered_map<std::string, UIElement*> nameMap_;
};