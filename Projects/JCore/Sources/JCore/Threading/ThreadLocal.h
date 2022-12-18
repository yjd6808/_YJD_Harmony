/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 6:41:35 PM
 * =====================
 * 정도식 TLS 구현
 * 일단... 꼭 필요한 기능이라서 구현함.
 * 기존 thread_local은 멤버 변수로 둘 수가 없는데, 난 이 기능을 추가해줬다.
 * 이 기능때문에 실제 thread_local 성능과는 비교조차 불가능하다.
 * 따라서 성능이 별로 중요하지 않는 상황에서만 사용할 것
 *
 */


#pragma once

#include <JCore/Threading/Thread.h>
#include <JCore/Threading/ThreadLocalManager.h>



namespace JCore {
	namespace Detail {
		inline static AtomicInt64U uiTypeUniqueIdSeq{};
	}

	template <typename T>
	class ThreadLocal
	{
		using LoadedTypeList = Vector<Int64U>;
		using TlsRefMap = HashMap<TlsObjectId, T*>;

	public:
		template <typename... Args>
		explicit ThreadLocal(Args&&... args): m_uiObjectId(++ms_uiObjectUniuqeIdSeq) {
			ThreadLocalManager_v->Register(ms_uiTypeUniqueId, m_uiObjectId, new T(Forward<Args>(args)...));
		}
		~ThreadLocal() {
			ThreadLocalManager_v->Unregister<T>(ms_uiTypeUniqueId, m_uiObjectId);
		}
	public:
		T& Get() {
			thread_local struct InlineLoader {
				InlineLoader(TlsTypeId typeId) : TlsCopyMap(ThreadLocalManager_v->GetTlsCopyMap(typeId)) {
					SafeConsole::WriteLine("[%05d] 인라인쓰레드 로딩완료 (LoaderMap 사이즈: %d)", Thread::GetThreadId(), TlsCopyMap.Size());
				}

				T& Load(Int64U objectId) {
					SafeConsole::WriteLine("[%05d] TLS 오브젝트 (오브젝트 ID: %llu) Get", Thread::GetThreadId(), objectId);
					return *TlsVarMap[objectId];
				}

				void Initialize(TlsObjectId objectId) {
					ThreadLocalCopy& localCopy = TlsCopyMap[objectId];
					DebugAssertMessage(localCopy.OriginalOwner, "복사본이 존재하지 않습니다.");
					Vector<void*>& copies = localCopy.Copies;
					T* copy = new T(*static_cast<T*>(localCopy.OriginalOwner));
					copies.PushBack(copy);	// 원본 복사본을 저장해놓고
					TlsVarMap.Insert(objectId, copy); // 참조본은 바로바로 가져올 수 있도록 참조로 저장
				}

				bool Initialized(TlsObjectId objectId) {
					return TlsVarMap.Exist(objectId);
				}

				TlsRefMap TlsVarMap;
				TlsCopyMap& TlsCopyMap;
			} Loader{ ms_uiTypeUniqueId };

			if (!Loader.Initialized(m_uiObjectId))
				Loader.Initialize(m_uiObjectId);

			return Loader.Load(m_uiObjectId);
		}
	private:
		Int64U m_uiObjectId;

		JCoreInlineClassMessage("%s 초기화", typeid(T).name());

		// https://stackoverflow.com/questions/8102125/is-local-static-variable-initialization-thread-safe-in-c11
		// static 변수의 initialization은 thread-safe하다.
		inline static AtomicInt64U ms_uiObjectUniuqeIdSeq{};
		inline static AtomicInt64U ms_uiTypeUniqueId = ++Detail::uiTypeUniqueIdSeq;
		
		inline static bool ms_bInitialized = false;
	};


} // namespace JCore