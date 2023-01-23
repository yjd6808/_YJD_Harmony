/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:41 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGComboKeyList.h>
#include <SteinsGate/Research/SGAction.h>

class SGComboAction : public SGAction
{
public:
	SGComboAction(SGPlayer* player, SGActionInfo* actionInfo);
	~SGComboAction() override;

	SGComboKeyList* getComboKeys() { return m_pComboKeys; }
	bool isComboAction() override { return true; }
private:
	SGComboKeyList* m_pComboKeys;
	friend class SGComboTree;
	friend class SGComboTreeNode;
};




