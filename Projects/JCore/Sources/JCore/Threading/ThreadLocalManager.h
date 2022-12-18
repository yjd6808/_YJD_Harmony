/*
 * 작성자: 윤정도
 * 생성일: 12/18/2022 6:44:34 PM
 * =====================
 * 프로그램의 마지막 ThreadLocal 객체가 소멸될때 이객체가 소멸되도록 해준다.
 *
 */


#pragma once



#include <JCore/Threading/ThreadLocalCopy.h>
#include <JCore/Container/HashMap.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Utils/Console.h>

namespace JCore {

	using TlsTypeId = Int64U;
	using TlsObjectId = Int64U;
	using TlsCopyMap = HashMap<TlsObjectId, ThreadLocalCopy>;

	
	class ThreadLocalManager : public MakeSharedFromThis<ThreadLocalManager>
	{	
	public:
		~ThreadLocalManager() {
			SafeConsole::WriteLine("스레드로컬 매니저 삭제됨");
		}

		template <typename T, DefaultEnableIf_t<IsPointerType_v<T>> = nullptr>
		void Register(Int64U typeId, Int64U objectId, T generated) {
			if (!Maps.Exist(typeId)) {
				SafeConsole::WriteLine("신규 객체: %s 생성 - (타입 ID: %llu, 오브젝트 ID: %llu) (최초 타입 등록)", typeid(T).name(), typeId, objectId);
				Maps.Insert(Move(typeId), TlsCopyMap{});
			}
			else { SafeConsole::WriteLine("신규 객체: %s 생성 - (타입 ID: %llu, 오브젝트 ID: %llu)", typeid(T).name(), typeId, objectId); }
			auto& temp = Maps[typeId];

			temp.Insert(Move(objectId), ThreadLocalCopy(generated));
		}

		template <typename T>
		void Unregister(Int64U typeId, Int64U objectId) {
			SafeConsole::WriteLine("객체: %s 제거 시도 - (타입 ID: %llu, 오브젝트 ID: %llu)", typeid(T*).name(), typeId, objectId);

			auto& localCopyMap = Maps[typeId];
			auto& localCopy = localCopyMap[objectId];
			auto& copies = localCopy.Copies;
			int copiesCount = copies.Size();

			for (int i = 0; i < copiesCount; ++i) {
				delete static_cast<T*>(copies[i]);
			}

			delete localCopy.OriginalOwner;
			localCopyMap.Remove(objectId);
		}

		TlsCopyMap& GetTlsCopyMap(const TlsTypeId typeId) {
			DebugAssertMessage(Maps.Exist(typeId), "헉.. 어떻게?");
			return Maps[typeId];
		}

		HashMap<TlsTypeId, TlsCopyMap> Maps;
	};

	using ThreadLocalManagerPtr = SharedPtr<ThreadLocalManager>;
	inline ThreadLocalManagerPtr ThreadLocalManager_v = MakeShared<ThreadLocalManager>();

} // namespace JCore