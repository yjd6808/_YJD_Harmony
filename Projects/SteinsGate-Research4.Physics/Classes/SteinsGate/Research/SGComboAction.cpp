/*
 * 작성자: 윤정도
 * 생성일: 1/12/2023 8:40:11 AM
 * =====================
 *
 */
#include "SGComboAction.h"


SGComboAction::SGComboAction(SGPlayer* player, SGActionInfo* actionInfo)
	: SGAction(player, actionInfo)
	, m_pComboKeys(new SGComboKeyList[actionInfo->CommandCount])
{
	for (int i = 0; i < actionInfo->CommandCount; ++i) {
		m_pComboKeys[i] = actionInfo->Commands[i];
	}
}

SGComboAction::~SGComboAction() {
	DeleteArraySafe(m_pComboKeys);
}

