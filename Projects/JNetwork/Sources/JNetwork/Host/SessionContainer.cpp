/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/SessionContainer.h>

USING_NS_JC;

NS_JNET_BEGIN

SessionContainer::SessionContainer(int capacity)
	: m_iSize(0)
	, m_vSessionList(capacity, nullptr) {
}

SessionContainer::~SessionContainer() {
	SessionContainer::Clear();
}

bool SessionContainer::Add(Session* session) {
	const int iHandle = session->GetHandle();

	if (!IsValidHandle(iHandle)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다.");
		return false;
	}

	if (m_vSessionList[iHandle] != nullptr) {
		_NetLogWarn_("동일한 핸들의 세션이 컨테이너에 존재합니다.");
		return false;
	}

	m_vSessionList[iHandle] = session;
	m_iSize++;
	return true;
}

Session* SessionContainer::Get(int handle) {
	if (!IsValidHandle(handle)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다.");
		return nullptr;
	}

	return m_vSessionList[handle];
}

bool SessionContainer::Remove(int handle) {
	if (!IsValidHandle(handle)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다.");
		return false;
	}

	const Session* pSession = m_vSessionList[handle];
	if (pSession) {
		delete pSession;
		m_vSessionList[handle] = nullptr;
		return true;
	}

	return false;
}

void SessionContainer::DisconnectAll() {
	const int iSize = m_vSessionList.Size();
	bool bCheck[10]{};

	for (int i = 0; i < iSize; ++i) {
		Session* pSession = m_vSessionList[i];

		if (!pSession) {
			continue;
		}
			
		pSession->Disconnect();
		pSession->WaitForZeroPending();

		// 10등분
		const int step = int(((float(i) / float(iSize)) * 100.0f) / 10);
		if (!bCheck[step]) {
			bCheck[step] = true;
			_NetLogDebug_("세션 연결 닫음: %d/%d(%d%%)", i, iSize, step * 10);
		}
	}
	_NetLogDebug_("세션 연결 닫음: %d/%d(%d%%)", iSize, iSize, 100);
}

void SessionContainer::Clear() {
	const int iSize = m_vSessionList.Size();
	for (int i = 0; i < iSize; ++i) {
		const Session* pSession = m_vSessionList[i];

		if (pSession) {
			delete pSession;
		}

		m_vSessionList[i] = nullptr;
	}

	m_iSize = 0;
}

void SessionContainer::ForEach(Action<Session*> fn) {
	const int iSize = m_vSessionList.Size();
	for (int i = 0; i < iSize; ++i) {
		Session* pSession = m_vSessionList[i];
		if (pSession) {
			fn(pSession);
		}
	}
}

void SessionContainer::ForEachConnected(Action<Session*> fn) {
	const int iSize = m_vSessionList.Size();
	for (int i = 0; i < iSize; ++i) {
		Session* pSession = m_vSessionList[i];

		if (pSession && pSession->GetState() == Host::eConnected) {
			fn(pSession);
		}
	}
}

bool SessionContainer::IsValidHandle(int handle) {
	if (handle < 0 || handle >= m_vSessionList.Size()) {
		return false;
	}
	return true;
}

NS_JNET_END
