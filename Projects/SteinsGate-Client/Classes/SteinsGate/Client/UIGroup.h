/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:30:42 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Client/UIElement.h>

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
	JCORE_HIDE_BASE_CLASS_METHOD(Node, addChild)

	struct CursorPositionGuard
	{
		CursorPositionGuard(SGEventMouse* _pMouseEvent, const SGVec2& _cursorPosition)
		: event_(_pMouseEvent)
		, resetCursorPosition_(_cursorPosition)
		{
		}

		~CursorPositionGuard()
		{
			if (event_ == nullptr)
			{
				return;
			}

			event_->setCursorPosition(resetCursorPosition_);
		}

		SGEventMouse* event_;
		SGVec2 resetCursorPosition_;
	};

public:
	UIGroup(UIRootGroup* _pMaster, UIGroup* _pParent);
	UIGroup(UIRootGroup* _pMaster, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner);
	~UIGroup() override;

	static UIGroup* create(UIRootGroup* _pMaster, UIGroup* _pParent);
	static UIGroup* create(UIRootGroup* _pMaster, UIGroup* _pParent, UIGroupInfo* _pGroupInfo, bool _infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Group; }

	bool init() override;
	void initChildren();
	void initChildrenPosition();

	void Load() override;
	void Unload() override;

	void addChild(UIElement* _pChild);

	// 이하 자식들 오버라이딩을 금하기 위해 파이널로 처리
	bool OnMouseDownInternal(SGEventMouse* _pMouseEvent) final;
	bool OnMouseMoveInternal(SGEventMouse* _pMouseEvent) final;
	bool OnMouseUpInternal(SGEventMouse* _pMouseEvent) final;
	bool OnMouseScrollInternal(SGEventMouse* _pMouseEvent) final;

	virtual void onUpdate(float _dt)
	{
	}

	virtual bool onKeyPressed(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) { return true; }
	virtual bool onKeyReleased(SGEventKeyboard::KeyCode _keyCode, SGEvent* _pEvent) { return true; }

	UIElementType_t GetElementType() override { return UIElementType::Group; }
	SGString ToString() override { return SGStringUtil::Format("그룹(%d)", groupInfo_->code_); }
	bool IsGroup() const override { return true; }

	// UI 매니저에 등록되지 않은 그룹내 엘리먼트들을 검색할 떄 사용하는 용도의 함수들
	UIElement* getAt(int _index);

	template <typename TElem>
	TElem* getAtTemplated(int _index)
	{
		if (_index >= _children.size())
		{
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", pBaseInfo_->code_, _index);
			return nullptr;
		}

		UIElement* pElement = static_cast<UIElement*>(_children.at(_index));
		if (pElement->GetElementType() != TElem::type())
		{
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소가 캐스팅할려는 타입과 다릅니다.", pBaseInfo_->code_, _index);
			return nullptr;
		}

		return static_cast<TElem*>(pElement);
	}

	UIElement* findElement(int _code);
	UIGroup* findGroup(int _groupCode);
	UIButton* findButton(int _buttonCode);
	UISprite* findSprite(int _spriteCode);
	UILabel* findLabel(int _labelCode);
	UICheckBox* findCheckBox(int _checkBoxCode);
	UIEditBox* findEditBox(int _editBoxCode);
	UIToggleButton* findToggleButton(int _toggleButtonCode);
	UIProgressBar* findProgressBar(int _progressBarCode);
	UIScrollBar* findScrollBar(int _scrollBarCode);
	UIStatic* findStatic(int _staticCode);

	void addUIElement(UIGroupElemInfo* _pGroupElemInfo);

	template <typename TElem>
	void forEachRecursiveSpecificType(const SGActionFn<TElem*>& _action) const
	{
		static_assert(!JCore::IsPointerType_v<TElem>, "... TElem must not be pointer type");
		static_assert(JCore::IsBaseOf_v<UIElement, TElem>, "... TElem must be derived from UIElemenet type");

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
				pGroupElement->forEachRecursiveSpecificType<TElem>(_action);
			}
		}
	}

	void forEachRecursive(const SGActionFn<UIElement*>& _action) const;
	void forEachRecursiveContainedSelf(const SGActionFn<UIElement*>& _action) const;
	void forEach(const SGActionFn<UIElement*>& _action) const;
	void forEachContainedSelf(const SGActionFn<UIElement*>& _action) const;

	void RestoreState(State _state) override;
	void resetChildrenPosition() { initChildrenPosition(); }

	void SetUISize(const SGSize& _contentSize) override;
	void SetInfo(UIElementInfo* _pInfo, bool _infoOwner) override;
	void setInfoGroup(UIGroupInfo* _pInfo, bool _infoOwner);

protected:
	static UIElement* findElementRecursiveInternal(UIGroup* _pParent, int _code);

	template <typename TElement>
	static TElement* findElementTemplated(UIGroup* _pParent, int _code)
	{
		UIElement* pElement = findElementRecursiveInternal(_pParent, _code);

		if (pElement == nullptr)
		{
			_LogWarn_("%d를 찾지 못했습니다.", _code);
			return nullptr;
		}

		const UIElementType_t targetType = TElement::type();
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
