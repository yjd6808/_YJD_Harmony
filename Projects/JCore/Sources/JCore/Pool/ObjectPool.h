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
 *	   Legend(int _1, int _2) : a(_1), b(_2) {}
 *	   int a;
 *	   int b;
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

#include <JCore/Memory.h>
#include <JCore/Primitives/SmartPtr.h>

#include <JCore/Sync/RecursiveLock.h>
#include <JCore/Sync/LockGuard.h>

NS_JC_BEGIN

template <typename T>
class ObjectPool
{
public:
	using TPool = ObjectPool<T>;
	using TLock = RecursiveLock;

	ObjectPool() : m_pNext(nullptr) {}
	virtual	~ObjectPool() = default;

	template <typename... Args>
	static SharedPtr<T> MakeShared(Args&&... args) {
		return { dbg_new T(Forward<Args>(args)...) };
	}

	static T* PopObject() {
		return (T*)TPool::operator new(sizeof(T));
	}

	static void PushObject(T* obj) {
		TPool::operator delete(obj);
	}

	template <typename... Args>
	static T* PopObjectWithConstruct(Args&&... args) {
		T* pInst = (T*)TPool::operator new(sizeof(T));
		Memory::PlacementNew(pInst, Forward<Args>(args)...);
		return pInst;
	}

	static void PushObjectWithDestroy(T* obj) {
		Memory::PlacementDelete(obj);
		TPool::operator delete(obj);
	}


	static void	FreeAllObjects() {
		LOCK_GUARD(ms_Lock);

		if (ms_uiAllocatedCount != 0) {
			_LogWarn_("아직 반환되지 않은 데이터가 존재합니다.");
		}

		T* pCur = nullptr;
		T* pNext = ms_pHead;

		int iTotalDeletedCount = 0;

		while (pNext != nullptr) {
			pCur = pNext;
			pNext = pNext->m_pNext;

			Memory::Deallocate(pCur);
			++iTotalDeletedCount;
		}

		ms_pHead = nullptr;

		if (iTotalDeletedCount != ms_uiTotalCount) {
			_LogWarn_("오브젝트풀의 데이터가 모두 제대로 삭제되지 않았습니다.");
		}
	}

	void* operator new[](size_t size) = delete;
	void operator delete[](void* obj) = delete;
	

	void* operator new(size_t size, int blockUse, char const* fileName, int lineNumber) {
		LOCK_GUARD(ms_Lock);

		T* pInst;
		if (ms_pHead != nullptr) {
			pInst = ms_pHead;
			ms_pHead = ms_pHead->m_pNext;
		} else {
			pInst = (T*)::operator new(size, blockUse, fileName, lineNumber);
			++ms_uiTotalCount;
		}

		pInst->m_pNext = NULL;
		++ms_uiAllocatedCount;
		return pInst;
		
	}

	void* operator new(size_t size) {
		LOCK_GUARD(ms_Lock);

		T* pInst;
		if (ms_pHead != nullptr) {
			pInst = ms_pHead;
			ms_pHead = ms_pHead->m_pNext;
		} else {
			pInst = Memory::Allocate<T*>(size);
			++ms_uiTotalCount;
		}

		pInst->m_pNext = NULL;
		++ms_uiAllocatedCount;
		return pInst;
	}

	void operator delete(void* obj) {
		if (obj == nullptr) {
			_LogWarn_("삭제할려는 %s 오브젝트 풀 객체가 nullptr입니다.", typeid(T).name());
			return;
		}

		LOCK_GUARD(ms_Lock);

		T* pInst = (T*)obj;

		if (pInst->m_pNext) {
			_LogWarn_("풀에서 관리중인 객체를 삭제할려고 시도했습니다.");
			return;
		}

		pInst->m_pNext = ms_pHead;
		ms_pHead = pInst;
		--ms_uiAllocatedCount;
	}

private:
	T* m_pNext;
	inline static T* ms_pHead = nullptr;
	inline static Int32U ms_uiTotalCount = 0;
	inline static Int32U ms_uiAllocatedCount = 0;
	inline static TLock	ms_Lock;
};

NS_JC_END