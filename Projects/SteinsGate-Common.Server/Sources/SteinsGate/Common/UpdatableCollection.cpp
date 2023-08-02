/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 8:18:15 AM
 * =====================
 *
 */


#include "Server.h"
#include "ServerCoreHeader.h"
#include "UpdatableCollection.h"

void UpdatableCollection::OnUpdate(const JCore::TimeSpan& elapsed) {
	for (int i = 0; i < m_iSize; ++i) {
		m_pArray[i]->OnUpdate(elapsed);
	}
}
