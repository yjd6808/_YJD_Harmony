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
class UIGroup : public UIElement
{
public:
	UIGroup(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo);
	~UIGroup() override;

	static UIGroup* createRetain(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo);
	static UIGroup* create(UIMasterGroup* master, UIGroup* parent, UIGroupInfo* groupInfo);

	bool init() override;
	void initChildren();
	void load() override;
	void unload() override;

	// 이하 자식들 오버라이딩을 금하기 위해 파이널로 처리
	bool onMouseMove(SGEventMouse* mouseEvent) final;
	bool onMouseDown(SGEventMouse* mouseEvent) final;
	bool onMouseUp(SGEventMouse* mouseEvent) final;
	bool onMouseScroll(SGEventMouse* mouseEvent) final;

	virtual void onUpdate(float dt) { }
	virtual bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }
	virtual bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }

	UIElementType_t getElementType() override { return UIElementType::Group; }
	SGString toString() override { return SGStringUtil::Format("그룹(%d)", m_pInfo->Code); }
	bool isGroup() const override { return true; }

	void addUIElement(UIGroupElemInfo* groupElemInfo);

	template <typename TElem>
	void forEachRecursiveSpecificType(const SGActionFn<TElem*>& action) const {
		static_assert(!JCore::IsPointerType_v<TElem>, "... TElem must not be pointer type");
		static_assert(JCore::IsBaseOf_v<UIElement, TElem>, "... TElem must be derived from SGUIElemenet type");

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
	void forEach(const SGActionFn<UIElement*>& action) const;
	void restoreState(State state) override;

	
	void setContentSize(const SGSize& contentSize) override;
protected:
	UIGroupInfo* m_pInfo;
};



