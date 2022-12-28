/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 6:44:34 PM
 * ====================================================================================
 * [! WARNING !]이 파일은 어디서도 쓰이지 않는 헤더파일이다.
 * 이번에 구현한 ThreadLocal 기능을 처음에 구현했을 때 작성한 코드이다.
 * 내가 원하는 ThreadLocal 기능을 구현해냈지만
 *
 * 1. 불안전한 성능
 * 2. 기분나쁜 2중 해쉬맵 구조
 * 3. 구조적 결함
 * 때문에 폐기한 코드이다.
 * 다음부턴 이렇게 만들지 않기 위해 박제한다.
 * ====================================================================================
 * [당시 내가한 생각]
 *	 이 생각을 한 당시 커밋해쉬는 : 59da4310de242cdb3451fa4be3907f07995c88d4 임
 *	 우선 쓰레드 세이프하지 않고 내가 원하는 대로 동작하는 상태로만 모두 구현해놓은 상태.
 *	 -ThreadLocal.h
 *	 -ThreadLocalCopy.h
 *	 -ThreadLocalManager.h
 *	 이렇게 3가지 파일로 구성해놨음
 *	
 *	
 *	1. 모든 ThreadLocal을 관리하는 m_Maps는 쓰레드 세이프해야한다.
 *	 Register 함수를 여러 쓰레드에서 동시에 접근하기 때문이다.
 *	 2. m_Maps에 자식으로 포함되는 TlsCopyMap도 마찬가지로 쓰레드 세이프해야한다.
 *	 Register() 함수에서 Insert로 등록을 진행하기 때문이다.
 *	 그리고 ThreadLocal<T>에서 이녀석을 접근하여 사용하기 때문이다.
 *	 3. TlsCopyMap을 SharedPtr로 처리해야할 것인가 ?
 *	이는 ThreadLocalManager보다 오래 생존하고 있을 수 있는
 *	 ThreadLocal<T> 객체가 존재할 수 있는것인가에 대한 답이 필요하다.
 *	 메인 쓰레드만 고려하면 ThreadLocal<T> 객체는 ThreadLocalManager보다 오래 생존할 수 없다.
 *	 ThreadLocal<T> 객체를 생성했다는 말은 이 헤더파일을 포함시켰다는 말이고 이건 ThreadLocalManager가 생성되었다는 말과 동일하기 때문이다.
 *	 그렇다면 메인 쓰레드보다 오래 생존할 수 있는 쓰레드가 있을 수 있는가 ?
 *	가능하다.
 *	 쓰레드를 detach시키게 된다면 프로세스가 종료되더라도 OS가 이녀석이 완료될때까지 관리하게 된다.
 *	 -https ://stackoverflow.com/questions/18756836/how-to-keep-a-thread-alive-after-the-main-process-exited
 *	4. 그럼 메인쓰레드가 종료된 후 다른 쓰레드가 다시 다른 타입의 ThraedLocal<T> 객체를 생성한다면 ?
 *	메인쓰레드가 종료되면서 ThreadLocalManager도 삭제가 되어버려서 오류가 발생할 것 같은데 ?
 *	ThreadLocalManager를 thread_local로 처리하는건 어떨까 ?
 *	= > 이렇게 만들면 메모리를 조금 더 먹겠지만 락이 필요 없어질 것 같다.
 *	 ㅇㅋ 이렇게 한번 구현해보자.
 */


#pragma once

#if 0

#include <JCore/Threading/ThreadLocalCopy.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Utils/Console.h>

namespace JCore {

	using TlsTypeId = Int64U;
	using TlsObjectId = Int64U;
	using TlsCopyMap	= HashMap<TlsObjectId, ThreadLocalCopy>;
	using TlsCopyMapPtr = SharedPtr<TlsCopyMap>;


	

	class ThreadLocalManager : public MakeSharedFromThis<ThreadLocalManager>
	{	
	public:
		~ThreadLocalManager() {
			SafeConsole::WriteLine("스레드로컬 매니저 삭제됨");
		}

		template <typename T, DefaultEnableIf_t<IsPointerType_v<T>> = nullptr>
		void Register(Int64U typeId, Int64U objectId, T generated) {
			NormalLockGuard guard(m_Lock);
			if (!m_Maps.Exist(typeId)) {
				SafeConsole::WriteLine("신규 객체: %s 생성 - (타입 ID: %llu, 오브젝트 ID: %llu) (최초 타입 등록)", typeid(T).name(), typeId, objectId);
				m_Maps.Insert(Move(typeId), MakeShared<TlsCopyMap>());
			}
			else { SafeConsole::WriteLine("신규 객체: %s 생성 - (타입 ID: %llu, 오브젝트 ID: %llu)", typeid(T).name(), typeId, objectId); }

			m_Maps[typeId]->Insert(Move(objectId), ThreadLocalCopy(generated));
		}

		template <typename T>
		void Unregister(Int64U typeId, Int64U objectId) {
			SafeConsole::WriteLine("객체: %s 제거 시도 - (타입 ID: %llu, 오브젝트 ID: %llu)", typeid(T*).name(), typeId, objectId);

			NormalLockGuard guard(m_Lock);
			TlsCopyMapPtr localCopyMap = m_Maps[typeId];
			auto& localCopy = localCopyMap->Get(objectId);
			auto& copies = localCopy.Copies;
			int copiesCount = copies.Size();

			for (int i = 0; i < copiesCount; ++i) {
				delete static_cast<T*>(copies[i]);
			}

			delete static_cast<T*>(localCopy.OriginalOwner);
			localCopyMap->Remove(objectId);
		}

		TlsCopyMapPtr GetTlsCopyMap(const TlsTypeId typeId) {
			NormalLockGuard guard(m_Lock);
			DebugAssertMessage(m_Maps.Exist(typeId), "헉.. 어떻게?");
			return m_Maps[typeId];
		}

	private:
		NormalLock m_Lock;
		HashMap<TlsTypeId, TlsCopyMapPtr> m_Maps;
	};

	using ThreadLocalManagerPtr = SharedPtr<ThreadLocalManager>;
	inline ThreadLocalManagerPtr ThreadLocalManager_v = MakeShared<ThreadLocalManager>();

} // namespace JCore
#endif