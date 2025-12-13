/*
 * 작성자: 윤정도
 * 생성일: 4/16/2023 2:21:27 PM
 * =====================
 * 연결리스트 기반 오브젝트풀
 * 다중상속시 베이스 클래스는 버철 소멸자 선언해줄것
 *
 * class Legend : public MakeSharedFromThis<Legend>, public ObjectPool<Legend>
 * {
 * public:
 *     Legend(int _1, int _2) : a(_1), b(_2) {}
 *     int a;
 *     int b;
 * };
 *
 *
 * [오브젝트 생성]
 * new Legend()
 * new Legend[10] 사용불가능 (배열생성을 구현할려면 연결리스트 기반으로하면 안되고 배열기반으로 구현 변경해야할듯..)
 *
 * [스마트 포인터 오브젝트 생성]
 * Legend::MakeShared()로 스마트포인터 생성
 * MakeShared<Legend>() 사용불가능
 *
 *
 */


#pragma once

#include <JCore/Config.h>
#include <JCore/Memory.h>
#include <JCore/Primitives/SmartPtr.h>
#include <JCore/Sync/NormalLock.h>

NS_JC_BEGIN

template <typename T>
class ObjectPool
{
public:
	using TPool = ObjectPool<T>;
	using TLock = NormalLock;

	struct LockGuard;
	struct AtExitCallback;

	ObjectPool()
		: pNext_(nullptr)
	{
	}

	virtual ~ObjectPool() = default;

	template <typename... Args>
	static SharedPtr<T> MakeShared(Args&&... _args)
	{
		return { dbg_new T(Forward<Args>(_args)...) };
	}

	static T* PopObject()
	{
		return static_cast<T*>(TPool::operator new(sizeof(T)));
	}

	static void PushObject(T* _pObject)
	{
		TPool::operator delete(_pObject);
	}

	template <typename... Args>
	static T* PopObjectWithConstruct(Args&&... _args)
	{
		T* pInst = static_cast<T*>(TPool::operator new(sizeof(T)));
		Memory::PlacementNew(pInst, Forward<Args>(_args)...);
		return pInst;
	}

	static void PushObjectWithDestroy(T* _pObject)
	{
		Memory::PlacementDelete(_pObject);
		TPool::operator delete(_pObject);
	}

	static void FreeAllObjects()
	{
		JCORE_LIB_LOCK_GUARD(Lock);

		if (AllocatedCount != 0)
		{
			_LogWarn_("아직 반환되지 않은 데이터가 존재합니다.");
		}

		T* pCur = nullptr;
		T* pNext = Head;

		int totalDeletedCount = 0;

		while (pNext != nullptr)
		{
			pCur = pNext;
			pNext = pNext->pNext_;

			Memory::Deallocate(pCur);
			++totalDeletedCount;
		}

		Head = nullptr;

		if (totalDeletedCount != TotalCount)
		{
			_LogWarn_("오브젝트풀의 데이터가 모두 제대로 삭제되지 않았습니다.");
		}
	}

	void* operator new[](size_t _size) = delete;
	void operator delete[](void* _pObject) = delete;

	void* operator new(size_t _size, int _blockUse, char const* _pFileName, int _lineNumber)
	{
		JCORE_LIB_LOCK_GUARD(Lock);

		T* pInst;
		if (Head != nullptr)
		{
			pInst = Head;
			Head = Head->pNext_;
		}
		else
		{
			pInst = static_cast<T*>(::operator new(_size, _blockUse, _pFileName, _lineNumber));
			++TotalCount;
		}

		pInst->pNext_ = nullptr;
		++AllocatedCount;
		return pInst;
	}

	void* operator new(size_t _size)
	{
		JCORE_LIB_LOCK_GUARD(Lock);

		T* pInst;
		if (Head != nullptr)
		{
			pInst = Head;
			Head = Head->pNext_;
		}
		else
		{
			pInst = Memory::Allocate<T*>(_size);
			++TotalCount;
		}

		pInst->pNext_ = nullptr;
		++AllocatedCount;
		return pInst;
	}

	void operator delete(void* _pObject)
	{
		if (_pObject == nullptr)
		{
			_LogWarn_("삭제할려는 %s 오브젝트 풀 객체가 nullptr입니다.", typeid(T).name());
			return;
		}

		T* pInst = static_cast<T*>(_pObject);

		JCORE_LIB_LOCK_GUARD(Lock);
		if (pInst->pNext_)
		{
			_LogWarn_("풀에서 관리중인 객체를 삭제할려고 시도했습니다.");
			return;
		}

		pInst->pNext_ = Head;
		Head = pInst;
		--AllocatedCount;
	}

private:
	T* pNext_;

	inline static T* Head = nullptr;
	inline static Int32U TotalCount = 0;
	inline static Int32U AllocatedCount = 0;
	inline static TLock Lock;
};

NS_JC_END
