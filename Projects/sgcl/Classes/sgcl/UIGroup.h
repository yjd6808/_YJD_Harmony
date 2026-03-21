/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:30:42 PM
 * =====================
 *
 */

#pragma once

#include "sgcl/UIElement.h"

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

	void AddChild(UIElement* _pChild);

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
	jc::String ToString() override { return jc::StringUtil::Format("그룹(%d)", groupInfo_->code_); }
	bool IsGroup() const override { return true; }

	// UI 매니저에 등록되지 않은 그룹내 엘리먼트들을 검색할 떄 사용하는 용도의 함수들
	UIElement* GetAt(int _index);

	template <typename TElem>
	TElem* GetAtTemplated(int _index)
	{
		if (_index >= _children.size())
		{
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->code_, _index);
			return nullptr;
		}

		UIElement* pElement = static_cast<UIElement*>(_children.at(_index));
		if (pElement->GetElementType() != TElem::Type())
		{
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소가 캐스팅할려는 타입과 다릅니다.", pBaseInfo_->code_, _index);
			return nullptr;
		}

		return static_cast<TElem*>(pElement);
	}

	UIElement* FindElement(int _code);
	UIGroup* FindGroup(int _groupCode);
	UIButton* FindButton(int _buttonCode);
	UISprite* FindSprite(int _spriteCode);
	UILabel* FindLabel(int _labelCode);
	UICheckBox* FindCheckBox(int _checkBoxCode);
	UIEditBox* FindEditBox(int _editBoxCode);
	UIToggleButton* FindToggleButton(int _toggleButtonCode);
	UIProgressBar* FindProgressBar(int _progressBarCode);
	UIScrollBar* FindScrollBar(int _scrollBarCode);
	UIStatic* FindStatic(int _staticCode);

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
	static UIElement* FindElementRecursiveInternal(UIGroup* _pParent, int _code);

	template <typename TElement>
	static TElement* FindElementTemplated(UIGroup* _pParent, int _code)
	{
		UIElement* pElement = FindElementRecursiveInternal(_pParent, _code);

		if (pElement == nullptr)
		{
			_LogWarn_("%d를 찾지 못했습니다.", _code);
			return nullptr;
		}

		const UIElementType_t targetType = TElement::Type();
		const UIElementType_t currentType = pElement->GetElementType();

		if (currentType != targetType)
		{
			_LogWarn_("%d가 %s타입이 아니고, %s입니다.", _code, UIElementType::Name[currentType],
			          UIElementType::Name[targetType]);
			return nullptr;
		}

		return static_cast<TElement*>(pElement);
	}

protected:
	UIGroupInfo* groupInfo_;
};
