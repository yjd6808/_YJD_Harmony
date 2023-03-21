/*
 * 작성자: 윤정도
 * 생성일: 2/15/2023 4:30:42 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/SGUIElement.h>

class SGUIManager;
class SGImagePackManager;
class SGDataManager;
class SGUIGroup : public SGUIElement
{
public:
	SGUIGroup(SGUIGroup* parent, SGUIGroupInfo* groupInfo);
	~SGUIGroup() override;

	static SGUIGroup* createRetain(SGUIGroup* parent, SGUIGroupInfo* groupInfo);
	static SGUIGroup* create(SGUIGroup* parent, SGUIGroupInfo* groupInfo);

	bool init() override;
	void initChildren();
	void load() override;
	void unload() override;
	bool onMouseMove(SGEventMouse* mouseEvent) override;
	bool onMouseDown(SGEventMouse* mouseEvent) override;
	bool onMouseUp(SGEventMouse* mouseEvent) override;
	bool onMouseScroll(SGEventMouse* mouseEvent) override;

	// 마스터 그룹들만 적용 (자식 그룹은 쓸일 없을 듯? 구현 안해노음)
	// return false시에는 ZOrder이 true를 반환한 그룹보다 낮은 대상들에게 전파되지 않음
	virtual bool onUpdate(float dt) { return true; }
	virtual bool onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }
	virtual bool onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event) { return true; }

	UIElementType_t getElementType() override { return UIElementType::Group; }
	bool isGroup() const override { return true; }

	void addUIElement(const SGUIGroupElemInfo& groupElemInfo);

	template <typename TElem>
	void forEachRecursiveSpecificType(const SGActionFn<TElem*>& action) const {
		static_assert(!JCore::IsPointerType_v<TElem>, "... TElem must not be pointer type");
		static_assert(JCore::IsBaseOf_v<SGUIElement, TElem>, "... TElem must be derived from SGUIElemenet type");

		for (int i = 0; i < _children.size(); ++i) {
			SGUIElement* pElem = (SGUIElement*)_children.at(i);
			TElem* pCastElem = dynamic_cast<TElem*>(pElem);

			if (pCastElem) {
				action(pCastElem);
			}

			if (pElem->getElementType() == UIElementType::Group) {
				SGUIGroup* pGroupElem = static_cast<SGUIGroup*>(pElem);
				pGroupElem->forEachRecursiveSpecificType<TElem>(action);
			}
		}

	}
	void forEachRecursive(const SGActionFn<SGUIElement*>& action) const;
	void forEach(const SGActionFn<SGUIElement*>& action) const;
	void restoreState(State state) override;
protected:
	SGUIGroupInfo* m_pInfo;
};



