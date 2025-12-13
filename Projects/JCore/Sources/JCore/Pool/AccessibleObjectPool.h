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
 * 3. 상속받은 클래스는 인자 없는 생성자 private으로 두고 friend class TAccessibleObjectPool 선언을 해줄 것
 * 4. Pop 수행시 OnPopped 가상함수가 호출되므로 이 함수에서 초기화와 같은 작업을 수행할 것
 * 5. 소멸자는 public으로 둬야함.
 *
 * ex) class CData : public CAccessibleObjectPool<CData> {
 *     private:
 *         friend class TAccessibleObjectPool;
 *         CData() {}
 *     public:
 *         ~CData() override {}
 *         void OnPopped() override {}
 *     };
 *
 * // TODO: 버철 기능 제거, OnPushed, OnPopped 함수는 콜백 함수로 처리할 수 있도록 한다. 정적 멤버로 Action<T>를 두고 가져올 때, 넣을 때 설정한 액션을 실행해주도록 한다.
 */

#pragma once

#include <JCore/Container/Vector.h>
#include <JCore/Container/LinkedList.h>

#include <JCore/Sync/RecursiveLock.h>

NS_JC_BEGIN

template <typename T>
class CAccessibleObjectPool
{
public:
	using TAccessTable = Vector<T*>;
	using TObjectPool = CAccessibleObjectPool<T>;

	CAccessibleObjectPool()
		: accessId_(-1)
	{
	}

	virtual ~CAccessibleObjectPool() = default;

	// new를 통한 생성을 금함.
	// Push, Pop으로만 객체 얻을 수 있도록 하기위함.
	void* operator new(size_t _size, int _blockUse, char const* _pFileName, int _lineNumber) = delete;
	void* operator new(size_t _size) = delete;

	static void InitPool(int _capacity, int _size, int _startId = 0)
	{
		DebugAssertMsg(AccessTable == nullptr, "이미 풀이 초기화되어 있습니다.");
		DebugAssert(_capacity >= _size);

		AccessTable = dbg_new Vector<T*>(_capacity, nullptr);
		StartId = _startId;
		Capacity = _capacity;
		Count = 0;

		for (int index = 0; index < _size; ++index)
		{
			T* pObject = CreateObject();
			pObject->accessId_ = _startId + Count;
			AccessTable->At(index) = pObject;
			Pool.PushBack(pObject);
			Count++;
		}
	}

	static void Expand(int _capacity)
	{
		DebugAssertMsg(AccessTable != nullptr, "초기화를 우선 해주세요.");

		const int curSize = AccessTable->Size();
		const bool shrink = _capacity < curSize;

		if (shrink)
		{
			DebugAssertMsg(false, "엑세스 오브젝트 풀의 사이즈를 줄이는 것은 불가능합니다.");
			return;
		}

		if (curSize == _capacity)
		{
			return;
		}

		JCORE_LIB_LOCK_GUARD(Sync);
		JCORE_DELETE_SAFE(AccessTableForSwap);

		AccessTableForSwap = dbg_new TAccessTable(_capacity, nullptr);

		for (int index = 0; index < curSize; ++index)
		{
			AccessTableForSwap->At(index) = AccessTable->At(index);
		}

		Capacity = _capacity;
		JCORE_SWAP(AccessTable, AccessTableForSwap, TAccessTable*);
	}

	static T* Pop()
	{
		DebugAssertMsg(AccessTable != nullptr, "초기화를 우선 해주세요.");

		JCORE_LIB_LOCK_GUARD(Sync);
		if (Count == Capacity)
		{
			Expand(Capacity * 4);
		}

		T* pObject;
		if (!Pool.IsEmpty())
		{
			pObject = Pool.Front();
			Pool.PopFront();
		}
		else
		{
			const int index = Count;
			pObject = CreateObject();
			pObject->accessId_ = StartId + index;
			DebugAssert(AccessTable->At(index) == nullptr);
			AccessTable->At(index) = pObject;
			Count++;
		}

		pObject->OnPopped();
		return pObject;
	}

	static void Push(T* _pObject)
	{
		JCORE_LIB_LOCK_GUARD(Sync);
		DebugAssert(Pool.Exist(_pObject) == false);
		Pool.PushFront(_pObject);
	}

	static TAccessTable* GetTable()
	{
		return AccessTable;
	}

	static T* GetByAccessId(int _accessId)
	{
		const int index = _accessId - StartId;
		if (index < 0 || index >= AccessTable->Size())
		{
			_LogWarn_("올바르지 않은 AccessibleObjectId(%d)", _accessId);
			return nullptr;
		}

		return AccessTable->At(index);
	}

	static void FreeAllObjects()
	{
		JCORE_LIB_LOCK_GUARD(Sync);
		DebugAssertMsg(Count == Pool.Size(), "%s 아직 반환되지 않은 오브젝트가 %d개 존재합니다.", typeid(T).name(), Count - Pool.Size());

		for (int index = 0; index < Count; ++index)
		{
			DeleteObject(AccessTable->At(index));
		}

		JCORE_DELETE_SAFE(AccessTable);
		JCORE_DELETE_SAFE(AccessTableForSwap);
	}

	// 메모리 할당된 모든 객체수
	static int GetTotalCount()
	{
		JCORE_LIB_LOCK_GUARD(Sync);
		return Count;
	}

	// 메모리 할당된 모든 객체들 중 실제 사용중인 객체 수
	static int GetActiveCount()
	{
		JCORE_LIB_LOCK_GUARD(Sync);
		return Count - Pool.Size();
	}

	// 메모리 할당된 모든 객체들 중 사용이 끝나고 반환된 객체 수
	static int GetRelasedCount()
	{
		JCORE_LIB_LOCK_GUARD(Sync);
		return Pool.Size();
	}

	virtual void OnPopped() = 0;   // 가져올때 실행할 작업 처리
	virtual void OnPushed() = 0;   // 넣을때 실행할 작업처리

	int GetAccessId() const
	{
		return accessId_;
	}

private:
	static T* CreateObject()
	{
		T* pObject = (T*)::dbg_operator_new(sizeof(T));
		::new (pObject) T();
		return pObject;
	}

	static void DeleteObject(T* _pObject)
	{
		_pObject->~T();
		::dbg_operator_delete(_pObject);
	}

protected:
	int accessId_;

private:
	inline static int StartId;
	inline static int Capacity;
	inline static int Count;
	inline static bool Initialized;
	inline static RecursiveLock Sync;
	inline static TAccessTable* AccessTable;
	inline static TAccessTable* AccessTableForSwap;
	inline static LinkedList<T*> Pool;
};

NS_JC_END
