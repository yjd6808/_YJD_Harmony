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
		CursorPositionGuard(SGEventMouse* mouseEvent, const SGVec2& position)
			: Event(mouseEvent)
			, ResetCursorPosition(position)
		{}
		~CursorPositionGuard() {
			Event->setCursorPosition(ResetCursorPosition);
		}

		SGEventMouse* Event;
		const SGVec2& ResetCursorPosition; // 복귀할 위치
	};

public:
	UIGroup(UIMasterGroup* master, UIGroup* parent);
	UIGroup(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo, bool infoOwner);
	~UIGroup() override;

	static UIGroup* create(UIMasterGroup* master, UIGroup* parent);
	static UIGroup* create(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo, bool infoOwner);

	static constexpr UIElementType_t type() { return UIElementType::Group; }

	bool init() override;
	void initChildren();
	void initChildrenPosition();

	void load() override;
	void unload() override;

	void addChild(UIElement* child);

	// 이하 자식들 오버라이딩을 금하기 위해 파이널로 처리
	bool onMouseDownInternal(SGEventMouse* mouseEvent) final;
	bool onMouseMoveInternal(SGEventMouse* mouseEvent) final;
	bool onMouseUpInternal(SGEventMouse* mouseEvent) final;
	bool onMouseScrollInternal(SGEventMouse* mouseEvent) final;

	virtual void onUpdate(float dt) { }
	virtual bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }
	virtual bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }

	UIElementType_t getElementType() override { return UIElementType::Group; }
	SGString toString() override { return SGStringUtil::Format("그룹(%d)", m_pInfo->Code); }
	bool isGroup() const override { return true; }

	// UI 매니저에 등록되지 않은 그룹내 엘리먼트들을 검색할 떄 사용하는 용도의 함수들
	UIElement* getAt(int index);

	template <typename TElem>
	TElem* getAtTemplated(int index) {
		if (index >= _children.size()) {
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소를 찾지 못했습니다.", m_pBaseInfo->Code, index);
			return nullptr;
		}

		UIElement* pElement = static_cast<UIElement*>(_children.at(index));
		if (pElement->getElementType() != TElem::type()) {
			_LogWarn_("%d 그룹에서 %d번째 인덱스 원소가 캐스팅할려는 타입과 다릅니다.", m_pBaseInfo->Code, index);
			return nullptr;
		}

		return static_cast<TElem*>(pElement);
	}


	UIElement* findElement(int code);
	UIGroup* findGroup(int groupCode);						
	UIButton* findButton(int buttonCode);					
	UISprite* findSprite(int spriteCode);					
	UILabel* findLabel(int labelCode);						
	UICheckBox* findCheckBox(int checkBoxCode);				
	UIEditBox* findEditBox(int editBoxCode);					
	UIToggleButton* findToggleButton(int toggleButtonCode);	
	UIProgressBar* findProgressBar(int progressBarCode);		
	UIScrollBar* findScrollBar(int scrollBarCode);			
	UIStatic* findStatic(int staticCode);

	void addUIElement(UIGroupElemInfo* groupElemInfo);

	template <typename TElem>
	void forEachRecursiveSpecificType(const SGActionFn<TElem*>& action) const {
		static_assert(!JCore::IsPointerType_v<TElem>, "... TElem must not be pointer type");
		static_assert(JCore::IsBaseOf_v<UIElement, TElem>, "... TElem must be derived from UIElemenet type");

		for (int i = 0; i < _children.size(); ++i) {
			UIElement* pElem = (UIElement*)_children.at(i);
			TElem* pCastElem = dynamic_cast<TElem*>(pElem);

			if (pCastElem) {
				action(pCastElem);
			}

			if (pElem->getElementType() == UIElementType::Group) {
				UIGroup* pGroupElem = static_cast<UIGroup*>(pElem);
				pGroupElem->forEachRecursiveSpecificType<TElem>(action);
			}
		}

	}
	void forEachRecursive(const SGActionFn<UIElement*>& action) const;
	void forEachRecursiveContainedSelf(const SGActionFn<UIElement*>& action) const;
	void forEach(const SGActionFn<UIElement*>& action) const;
	void forEachContainedSelf(const SGActionFn<UIElement*>& action) const;

	void restoreState(State state) override;
	void resetChildrenPosition() { initChildrenPosition(); }

	void setUISize(const SGSize& contentSize) override;
	void setInfo(UIElementInfo* info, bool infoOwner) override;
	void setInfoGroup(UIGroupInfo* info, bool infoOwner);
protected:
	static UIElement* findElementRecursiveInternal(UIGroup* parent, int code);

	template <typename TElement>
	static TElement* findElementTemplated(UIGroup* parent, int code) {
		UIElement* find = findElementRecursiveInternal(parent, code);

		if (find == nullptr) {
			_LogWarn_("%d를 찾지 못했습니다.", code);
			return nullptr;
		}

		const UIElementType_t eTarget = TElement::type();
		const UIElementType_t eCur = find->getElementType();

		if (find->getElementType() != eTarget) {
			_LogWarn_("%d가 %s타입이 아니고, %s입니다.", code, UIElementType::Name[eCur], UIElementType::Name[eTarget]);
			return nullptr;
		}

		return (TElement*)find;
	}

protected:
	UIGroupInfo* m_pInfo;
};



