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
	int getCode() override;

	UIElementType_t getElementType() override { return UIElementType::Group; }
	bool isGroup() const override { return true; }

	void addUIElement(const SGUIGroupElemInfo& groupElemInfo);
	void forEachRecursive(const SGActionFn<SGUIElement*>& action) const;
	void forEach(const SGActionFn<SGUIElement*>& action) const;
	void restoreState(State state) override;
protected:
	SGUIGroupInfo* m_pInfo;
	SGVector<SGUIElement*> vChildren;
};



