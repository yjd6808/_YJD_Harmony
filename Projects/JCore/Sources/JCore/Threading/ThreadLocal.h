/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 6:41:35 PM
 * =====================
 * 기존 thread_local은 멤버변수로 둘 수가 없다.
 * 그래서 내가 생각한대로 동작하는 TLS를 구현하고자한다.
 *
 * 메모리풀 릭 탐지시 쓰레드 개별적으로 탐지가능하도록 만들고 싶어진게
 * 구현을 시도해보게된 계기이다
 *
 * 락을 달고 있는게 너무 거슬리는데... 
 *
 */


#pragma once

#include <JCore/Sync/NormalRwLock.h>
#include <JCore/Container/Vector.h>
#include <JCore/Threading/Thread.h>


NS_JC_BEGIN


template <typename T, typename TAllocator = DefaultAllocator>
class ThreadLocal
{
	using TlsObjectId = Int64U;
	using TlsRefMap = HashMap<TlsObjectId, T, TAllocator>;
	using TlsRefMapPtr = SharedPtr<TlsRefMap>;
	using TlsValueMaps = LinkedList<WeakPtr<TlsRefMap>, TAllocator>;
public:
	template <typename... Args>
	explicit ThreadLocal(Args&&... args) {
		if (m_uiObjectId != 0ULL) {
			DebugAssertMsg(false, "이미 생성자가 한번 호출이 되었습니다.");
			return;
		}

		m_uiObjectId = ++ms_uiObjectUniuqeIdSeq;

		thread_local TlsRefMap tls_RefMap
			= JCORE_INLINE_RETURN_MESSAGE(TlsRefMap, {}, "[%5d] ThreadLocal<%s> 해쉬맵 초기화", Thread::GetThreadId(), typeid(T).name());



		/*
		 * 내가 구현했지만 나중에 다시보면 이해하기 힘들 수 있는 난해한 로직이므로
		 * 왜 내가 이렇게 구현했는지 자세하게 적어본다.
		 *
		 * [tls_RefMap의 역할]
		 * 1. 원본 객체의 데이터를 저장하는 용도	★	│
		 * ─────────────────────────────────────┘
		 * 예를들어 ThreadLocal<T>라는 객체를 처음 1번 쓰레드에서 생성하면 이 녀석의 제일 초기 원본 데이터는
		 * 1번 쓰레드에서만 처음 생성자로 접근할 수 있는 tls_RefMap에 저장되도록 한다.
		 * 그리고 다른 쓰레드에서 1번 쓰레드의 tls_RefMap에 접근할 수 있도록 멤버변수 m_pRefMap이 thread_local tls_RefMap의 참조본을 가지고 있도록한다.
		 * 처음 생성 당시 상태의 데이터를 저장해놓고 다른 쓰레드에서 접근할때 데이터를 복사하여 사용하도록 한다.
		 *
		 *
		 *
		 *
		 * 2. 굳이 생성자에 thread_local로 둔 이유는 쓰레드가 생성될때마다 해쉬맵을 생성하는 것을 방지하기 위함이다.	★	│
		 * ─────────────────────────────────────────────────────────────────────────────────────────────────┘
		 * tls_RefMap을 생성자에 두지 않고 inline static으로 클래스에 박아버리게 되면
		 * 쓰레드 구분없이 모든 타입에 대해서 해쉬맵을 생성해버리게 된다.
		 *
		 * 쓰레드가 수십개다?
		 * 사용하지도 않는데 각 타입별로 해쉬맵이 수천개가 생성되어버릴 수 있다.
		 * 따라서 실제로 사용될때에만 해쉬맵이 생성되도록 하자.
		 *
		 * 	ThreadLocal<int> a;
		 *	Thread th{[](void*) {
		 *		Thread::Sleep(1000);
		 *		ThreadLocal<double> b;
		 *	}};
		 *	th.AutoJoin(true);
		 *
		 * =======================================================
		 * [수정 전]
		 *
		 * 쓰레드1(메인)								| 쓰레드2
		 * ThreadLocal<int>::TlsRefMap 생성			|
		 * ThreadLocal<double>::TlsRefMap 생성		|
		 * ThreadLocal<int> a; 						| [쓰레드 생성]
		 *											| ThreadLocal<int>::TlsRefMap 생성
		 *											| ThreadLocal<double>::TlsRefMap 생성
		 *											| ThreadLocal<double> a;
		 *
		 *
		 *  [23608] ThreadLocal<int> 해쉬맵 초기화
		 *  [23608] ThreadLocal<double> 해쉬맵 초기화
		 *	[11892] ThreadLocal<int> 해쉬맵 초기화
		 *	[11892] ThreadLocal<double> 해쉬맵 초기화
		 * =======================================================
		 * [수정 후]
		 * 쓰레드1(메인)						 		| 쓰레드2
		 * ... 바로 생성안함.						 	|
		 * ThreadLocal<int> a; -> <int>TlsRefMap 생성| [쓰레드 생성]
		 *											|  ... 바로 생성안함.
		 *											|
		 *											| ThreadLocal<double> a;  --> <double>::TlsRefMap 생성
		 *
		 *	[ 7348] ThreadLocal<int> 해쉬맵 초기화
		 *	[11096] ThreadLocal<double> 해쉬맵 초기화
		 * ================================================
		 */

		NormalLockGuard guard(m_Lock);
		m_pRefMap = &tls_RefMap;
		m_pRefMap->Insert(m_uiObjectId, T(Forward<Args>(args)...));
		m_ValueMaps = TAllocator::template Allocate<TlsValueMaps>();
		Memory::PlacementNew(m_ValueMaps);
	}
	~ThreadLocal() {
		NormalLockGuard guard(m_Lock);
		if (m_pRefMap && m_pRefMap->Size() > 0) {
			m_pRefMap->Remove(m_uiObjectId);
		}

		auto it = m_ValueMaps->Begin();
		while (it->HasNext()) {
			auto wp = it->Next();
			if (wp.Exist()) {
				wp->Remove(m_uiObjectId);
			} else {
				// 글로벌 변수로 ThreadLocal<T>를 둔 경우 혹은 쓰레드가 소멸될 때
				// tls_valueMap이 먼저 소멸될 수 있다.
			}
		}
		Memory::PlacementDelete(m_ValueMaps);
		TAllocator::template Deallocate<TlsValueMaps>(m_ValueMaps);
	}
public:
	// ThreadLocal<T>와 생명주기가 같음
	T& Ref() {
		NormalLockGuard guard(m_Lock);
		DebugAssertMsg(m_pRefMap && m_pRefMap->Exist(m_uiObjectId), "생성자에서 초기화되지 않은 상태이거나 소멸된 객체입니다.");

		/* 계속 쓰레기 데이터 쌓이는데 어떻게 처리하지.. 흠
		 *
		 * 1. {
		 *       ThreadLocal<T> tls;
		 *		 tls.Ref();
		 *    }
		 * TlsRefMap보다 ThreadLocal<T>의 소멸자가 먼저 호출되는 경우
		 *   =>
		 * 2. {
		 *       ThreadLocal<T> tls;
		 *       Thread{[](void*)
		 *       {
		 *			tls.Ref()
		 *       }}
		 *    }
		 *	TlsRefMap가 ThreadLocal<T>의 소멸자보다 먼저 호출되는 경우
		 *	 => 그냥 나두면 됨.
		 *
		 */
		// 데이터가 계속 쌓일 경우 처리를 위해 동적할당하여 소멸 시기를 내가 수동으로 결정해줘야한다.
		// 쓰레드가 순식간에 50개가 되어버린 후 1개로 줄어버리는 상황이 발생하면
		// 메모리릭이 발생하게 되므로 미리 복사본을 가지고 있어야한다.
		//
		// 현재 치명적인 문제점은
		// 쓰레드1                    | 쓰레드2
		// ThreadLocal<int> a;       |
		//                           | a 사용
		//                           | 쓰레드2 소멸
		
		thread_local TlsRefMapPtr tls_valueMap = MakeShared<TlsRefMap, TAllocator>();

		if (!tls_valueMap->Exist(m_uiObjectId)) {
			tls_valueMap->Insert(m_uiObjectId, m_pRefMap->Get(m_uiObjectId));
			m_ValueMaps->PushBack(WeakPtr<TlsRefMap>(tls_valueMap));
		}
		return tls_valueMap->Get(m_uiObjectId);
	}
private:
	TlsRefMap* m_pRefMap;
	TlsValueMaps* m_ValueMaps;
	AtomicInt64U m_uiObjectId{};
	

	// tls_RefMap이 thread_local이지만 m_pRefMap이 이를 참조해서 다른 쓰레드에서 사용할 수 있기 때문에
	// 락의 범위는 락이 필요 없는 ThreadLocal<T>::thread_local에서 ThreadLocal<T>로 확장되어버린다. ㅠㅠ
	inline static NormalLock m_Lock{};

	// https://stackoverflow.com/questions/8102125/is-local-static-variable-initialization-thread-safe-in-c11
	// static 변수의 initialization은 thread-safe하다.
	// 근데 이게 다른곳에서 쓰이면 thread-safe하지 않으니까..
	inline static thread_local AtomicInt64U ms_uiObjectUniuqeIdSeq{};
};


NS_JC_END