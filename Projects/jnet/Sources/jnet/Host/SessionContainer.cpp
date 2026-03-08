/*
 * 작성자 : 윤정도
 */

#include <jnet/Core.h>
#include <jnet/Host/SessionContainer.h>

#include <jc/Utils/ProgressNotifier.h>

USING_NS_JC;
USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
SessionContainer::SessionContainer(int _capacity)
: handleSeq_(0)
, initialHandleSeq_(0)
, size_(0)
, sessionList_(_capacity, nullptr)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
SessionContainer::~SessionContainer()
{
	SessionContainer::Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////
object_id SessionContainer::CreateHandle()
{
	return initialHandleSeq_ + Interlocked<object_id>::Increment(&handleSeq_);
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SessionContainer::Add(Session* _pSession)
{
	const object_id handle = _pSession->GetHandle();
	const int handleIndex = static_cast<int>(handle - initialHandleSeq_ - 1);

	if (!IsValidHandle(handleIndex))
	{
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다. %d", 1);
		return false;
	}

	if (sessionList_[handleIndex] != nullptr)
	{
		_NetLogWarn_("동일한 핸들의 세션이 컨테이너에 존재합니다.");
		return false;
	}

	sessionList_[handleIndex] = _pSession;
	++size_;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
Session* SessionContainer::Get(object_id _handle)
{
	const int handleIndex = static_cast<int>(_handle - initialHandleSeq_ - 1);
		
	if (!IsValidHandle(handleIndex))
	{
		_NetLogWarn_("세션 컨테이너 인덱스 범위를 벗어난 핸들입니다. %d", 2);
		return nullptr;
	}

	return sessionList_[handleIndex];
}

//////////////////////////////////////////////////////////////////////////////////////////
void SessionContainer::DisconnectAll()
{
	const int size = sessionList_.Size();
	PercentProgressNotifier notifier(size, 10.0f);
	CallbackProgressListener* pListener = dbg_new CallbackProgressListener;
	pListener->ProgressedCallback = [](int _index, int _size)
	{
		_NetLogDebug_("세션 연결 닫음: %d/%d(%.1f%%)", _index, _size, _index / float(_size) * 100.0f);
	};
	notifier.SetListener(pListener, true);

	for (int i = 0; i < size; ++i)
	{
		Session* pSession = sessionList_[i];

		if (!pSession)
		{
			continue;
		}

		pSession->Disconnect();
		pSession->WaitForZeroPending();

		notifier.Progress(i + 1);
	}

	_NetLogDebug_("모든 세션 연결종료 완료");
}

//////////////////////////////////////////////////////////////////////////////////////////
void SessionContainer::Clear()
{
	const int size = sessionList_.Size();
	for (int i = 0; i < size; ++i)
	{
		Session* pSession = sessionList_[i];

		if (pSession)
		{
			delete pSession;
		}

		sessionList_[i] = nullptr;
	}

	size_ = 0;
	_NetLogDebug_("모든 세션 삭제완료");
}

//////////////////////////////////////////////////////////////////////////////////////////
void SessionContainer::ForEach(Action<Session*> _fn)
{
	const int size = sessionList_.Size();
	for (int index = 0; index < size; ++index)
	{
		Session* pSession = sessionList_[index];
		if (pSession)
		{
			_fn(pSession);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void SessionContainer::ForEachConnected(Action<Session*> _fn)
{
	const int size = sessionList_.Size();
	for (int i = 0; i < size; ++i)
	{
		Session* pSession = sessionList_[i];

		if (pSession && pSession->GetState() == Host::eConnected)
		{
			_fn(pSession);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool SessionContainer::IsValidHandle(int _handleIndex)
{
	if (_handleIndex < 0 || _handleIndex >= sessionList_.Size())
	{
		return false;
	}

	return true;
}

