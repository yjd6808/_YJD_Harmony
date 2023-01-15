/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:08:41 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/ComboKeyList.h>
#include <SteinsGate/Research/ActionAbstract.h>

class ComboAction : public ActionAbstract
{
public:
	ComboKeyList& getComboKeys() { return m_ComboKeys; }
	bool initWithKeys(const ComboKeyList& keys);
	bool isComboAction() override { return true; }
private:
	ComboKeyList m_ComboKeys;

	friend class ComboTree;
	friend class ComboTreeNode;
};




