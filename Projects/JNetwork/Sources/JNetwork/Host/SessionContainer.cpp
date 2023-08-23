/*
 * 작성자 : 윤정도
 */

#include <JNetwork/Network.h>
#include <JNetwork/Host/SessionContainer.h>

#include <JCore/Utils/ProgressNotifier.h>

USING_NS_JC;

NS_JNET_BEGIN

SessionContainer::SessionContainer(int capacity)
	: m_iInitialHandleSeq(0)
	, m_iSize(0)
	, m_vSessionList(capacity, nullptr) {
}

SessionContainer::~SessionContainer() {
	SessionContainer::Clear();
}

int SessionContainer::CreateHandle() {
	return m_iInitialHandleSeq + Interlocked<int>::Increment(&m_iHandleSeq) - 1;
}

bool SessionContainer::Add(Session* session) {
	const int iHandle = session->GetHandle();
	const int iHandleIndex = iHandle - m_iInitialHandleSeq;

	if (!IsValidHandle(iHandleIndex)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다. %d", 1);
		return false;
	}

	if (m_vSessionList[iHandleIndex] != nullptr) {
		_NetLogWarn_("동일한 핸들의 세션이 컨테이너에 존재합니다.");
		return false;
	}

	m_vSessionList[iHandleIndex] = session;
	m_iSize++;
	return true;
}

Session* SessionContainer::Get(int handle) {
	const int iHandleIndex = handle - m_iInitialHandleSeq;

	if (!IsValidHandle(iHandleIndex)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다. %d", 2);
		return nullptr;
	}

	return m_vSessionList[iHandleIndex];
}

bool SessionContainer::Remove(int handle) {
	const int iHandleIndex = handle - m_iInitialHandleSeq;

	if (!IsValidHandle(iHandleIndex)) {
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다. %d", 3);
		return false;
	}

	const Session* pSession = m_vSessionList[iHandleIndex];
	if (pSession) {
		delete pSession;
		m_vSessionList[iHandleIndex] = nullptr;
		return true;
	}

	return false;
}

void SessionContainer::DisconnectAll() {
	const int iSize = m_vSessionList.Size();
	PercentProgressNotifier notifier(iSize, 10.0f);
	CallbackProgressListener* pListener = dbg_new CallbackProgressListener;
	pListener->ProgressedCallback = [](int i, int size) { _NetLogDebug_("세션 연결 닫음: %d/%d(%.1f%%)", i, size, i / float(size) * 100.f); };
	notifier.SetListener(pListener, true);
	
	for (int i = 0; i < iSize; ++i) {
		Session* pSession = m_vSessionList[i];

		if (!pSession) {
			continue;
		}
			
		pSession->Disconnect();
		pSession->WaitForZeroPending();

		notifier.Progress(i + 1);
	}
	
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

bool SessionContainer::IsValidHandle(int handleIndex) {
	if (handleIndex < 0 || handleIndex >= 0 + m_vSessionList.Size()) {
		return false;
	}
	return true;
}

NS_JNET_END
