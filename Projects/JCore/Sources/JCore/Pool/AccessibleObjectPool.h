/*
 * 작성자: 윤정도
 * 생성일: 10/14/2023 3:58:21 PM
 * =====================
 * [만든 이유]
 * 기존 ObjectPool<T>는 풀에 접근해서 객체를 가져오는게 불가능하기 때문에
 * 멀티쓰레드 환경에서 접근성이 중요시되는 객체를 손쉽게 풀링하기 위함이다.
 *
 * 1. 사용전 InitPool()을 호출하여 풀 초기화를 수행해줄것
 * 2. 프로그램 종료전 풀에 모든 데이터가 반환 된후에 FreeAllObjects()를 호출할 것
 * 3. 상속받은 클래스는 인자 없는 생성자 private으로 두고 friend class TObjectPool선언을 해줄 것
 * 4. Pop 수행시 OnPopped 가상함수가 호출되므로 이 함수에서 초기화와 같은 작업을 수행할 것
 * 5. 소멸자는 public으로 둬야함.
 *
 * ex) class Data : public AccessibleObjectPool<Data> {
 *	   private:
 *			friend class TObjectPool;
 *			Data() {}
 *	   public:
 *		    ~Data() override {}
 *		    void OnPopped() override {}
 *	   }
 *
 */


#pragma once

#include <JCore/Container/Vector.h>
#include <JCore/Container/LinkedList.h>

#include <JCore/Sync/RecursiveLock.h>

NS_JC_BEGIN

template <typename T>
class AccessibleObjectPool
{
protected:
	using TAccessTable = Vector<T*>;
	using TObjectPool = AccessibleObjectPool<T>;
public:
	AccessibleObjectPool() : m_iAccessId(-1) {}
	virtual ~AccessibleObjectPool() = default;

	static void InitPool(int capacity, int size, int startId = 0) {
		DebugAssertMsg(ms_vAccessTable == nullptr, "이미 풀이 초기화되어 있습니다.");

		ms_vAccessTable = dbg_new Vector<T*>(capacity, nullptr);
		ms_iStartId = startId;
		ms_iCapacity = capacity;
		ms_iCount = 0;

		for (int i = 0; i < size; ++i) {
			T* obj = dbg_new T();
			obj->m_iAccessId = startId + ms_iCount;
			ms_vAccessTable->At(i) = obj;
			ms_lPool.PushBack(obj);
			ms_iCount++;
		}
	}

	static void Expand(int capacity) {
		const int iCurSize = ms_vAccessTable->Size();
		const bool bShirink = capacity < iCurSize;

		if (bShirink) {
			DebugAssertMsg(false, "엑세스 오브젝트 풀의 사이즈를 줄이는 것은 불가능합니다.");
			return;
		}

		if (iCurSize == capacity) 
			return;

		JCORE_LOCK_GUARD(ms_Sync);
		JCORE_DELETE_SAFE(ms_vAccessTableForSwap);

		ms_vAccessTableForSwap = dbg_new TAccessTable(capacity, nullptr);

		for (int i = 0; i < iCurSize; ++i) {
			ms_vAccessTableForSwap->At(i) = ms_vAccessTable->At(i);
		}

		ms_iCapacity = capacity;
		JCORE_SWAP(ms_vAccessTable, ms_vAccessTableForSwap, TAccessTable*);
	}

	static T* Pop() {
		JCORE_LOCK_GUARD(ms_Sync);

		if (ms_iCount == ms_iCapacity) {
			Expand(ms_iCapacity * 4);
		}

		T* pObject;
		if (!ms_lPool.IsEmpty()) {
			pObject = ms_lPool.Front();
			ms_lPool.PopFront();
		} else {
			const int iIndex = ms_iCount;
			pObject = dbg_new T();
			pObject->m_iAccessId = ms_iStartId + iIndex;
			DebugAssert(ms_vAccessTable->At(iIndex) == nullptr);
			ms_vAccessTable->At(iIndex) = pObject;
			ms_iCount++;
		}
		pObject->OnPopped();
		return pObject;
	}

	static void Push(T* obj) {
		JCORE_LOCK_GUARD(ms_Sync);
		ms_lPool.PushBack(obj);
	}

	static T* GetByAccessId(int accessId) {
		const int iIndex = accessId - ms_iStartId;
		if (iIndex < 0 || iIndex >= ms_vAccessTable->Size()) {
			_LogWarn_("올바르지 않은 AccessibleObjectId(%d)", accessId);
			return nullptr;
		}

		return ms_vAccessTable->At(iIndex);
	}

	static void FreeAllObjects() {
		JCORE_LOCK_GUARD(ms_Sync);
		DebugAssertMsg(ms_iCount == ms_lPool.Size(), "아직 반환되지 않은 오브젝트가 존재합니다.");

		for (int i = 0; i < ms_iCount; ++i) {
			delete ms_vAccessTable->At(i);
		}

		JCORE_DELETE_SAFE(ms_vAccessTable);
		JCORE_DELETE_SAFE(ms_vAccessTableForSwap);
	}


	// 메모리 할당된 모든 객체수
	static int GetTotalCount() {
		JCORE_LOCK_GUARD(ms_Sync);
		return ms_iCount;
	}

	// 메모리 할당된 모든 객체들 중 실제 사용중인 객체 수
	static int GetActiveCount() {
		JCORE_LOCK_GUARD(ms_Sync);
		return ms_iCount - ms_lPool.Size();
	}

	// 메모리 할당된 모든 객체들 중 사용이 끝나고 반환된 객체 수
	static int GetRelasedCount() {
		JCORE_LOCK_GUARD(ms_Sync);
		return ms_lPool.Size();
	}

	virtual void OnPopped() = 0;		// 풀에서 팝될때 실행할 작업들 처리

	int GetAccessId() const { return m_iAccessId; }

protected:
	int m_iAccessId;
private:
	inline static int ms_iStartId;
	inline static int ms_iCapacity;
	inline static int ms_iCount;
	inline static bool ms_bInitialized;
	inline static RecursiveLock ms_Sync;
	inline static TAccessTable* ms_vAccessTable;
	inline static TAccessTable* ms_vAccessTableForSwap;
	inline static LinkedList<T*> ms_lPool;
};

NS_JC_END

