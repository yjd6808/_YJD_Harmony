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
	SGUIGroup();
	SGUIGroup(SGUIGroup* parent, SGUIGroupInfo* groupInfo);
	~SGUIGroup() override;

	static SGUIGroup* createRetain(SGUIGroup* parent, SGUIGroupInfo* groupInfo);
	static SGUIGroup* create(SGUIGroup* parent, SGUIGroupInfo* groupInfo);

	
	bool init() override;
	void load() override;
	void unload() override;
	bool onMouseMove(SGEventMouse* mouseEvent) override;
	bool onMouseDown(SGEventMouse* mouseEvent) override;
	bool onMouseUp(SGEventMouse* mouseEvent) override;
	bool onMouseScroll(SGEventMouse* mouseEvent) override;
	int getCode() override;
	UIElementType_t getElementType() override { return UIElementType::Group; }

	void addUIElement(const SGUIGroupElemInfo& groupElemInfo);
	void forEach(const SGActionFn<SGUIElement*>& iteration);

	// 정렬 방식에 따라 앵커포인트 0, 0기준으로해서 박스내의 위치를 구한다.
	SGVec2 getPositionInRect(
		const SGRect& rc,
		float origin_x,
		float origin_y);

	
	void setPositionRealNoParent(float x, float y);			// 마스타 UI 그룹이면 부모가 없으므로... 구분해서 처리해야함
	void setPositionRealHasParent(float x, float y);		// 부모가 그룹이면 그룹 내에서 상대적 위치 반영
	void setPositionReal(float x, float y);					// 호리즌탈, 바티칼 고려해서 위치 반영
	void setPositionReal(const SGVec2& pos);

	void restoreState(State state) override;
protected:
	SGUIManager* m_pUIManager;
	SGImagePackManager* m_pPackManager;
	SGDataManager* m_pDataManager;
	SGUIGroupInfo* m_pInfo;

};



