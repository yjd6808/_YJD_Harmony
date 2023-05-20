/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 5:36:01 PM
 * =====================
 *
 * 포인터 싱글톤과 스타릭 싱글톤 특징
 * @참고: https://stackoverflow.com/questions/13047526/difference-between-singleton-implemention-using-pointer-and-using-static-object
 *
 * 1. 정적 싱글톤이 사용이 훨씬 편리하다.
 *    1. 모든 컴파일러에 대해서 쓰레드 세이프하다.
 *	  2. 오브젝트가 프로그램 종료시 자동 소멸된다.
 *	  3. 구현이 쉽고 간단하다.
 *
 * 2. 포인터 싱글톤은 메모리 관리에 용이하다.
 *    1. 정적 객체는 프로그램 시작시 메모리에 용량을 차지하고 있기 때문에
 *	     늦게 생성된다고 하더라도 무거운 객체의 경우 비효율적일 수 있다.
 *
 *
 */


#pragma once

#include <JCore/Pattern/NonCopyableh.h>
#include <JCore/Debug/New.h>
#include <JCore/Sync/NormalLock.h>

NS_JC_BEGIN

template <typename T>
class SingletonStatic : private NonCopyableNonMovable
{
protected:
	SingletonStatic() = default;
	virtual ~SingletonStatic() = default;
public:
	using TSingleton = SingletonStatic<T>;
	static T* Get() {
		static T instance;
		return &instance;
	}
};

template <typename T>
class SingletonPointer : private NonCopyableNonMovable
{
protected:
	SingletonPointer() = default;
	virtual ~SingletonPointer() = default;
public:
	using TSingleton = SingletonPointer<T>;

	static T* Get() {
		if (ms_pInst == nullptr) {
			LOCK_GUARD(ms_Lock);

			if (ms_bDeleted) {
				DebugAssertMsg(false, "삭제된 객체에 접근을 시도했습니다.");
				return nullptr;
			}

			if (ms_pInst == nullptr) {
				ms_pInst = dbg_new T;
			}
		}

		return ms_pInst;
	}

	static void Free() {
		if (ms_pInst != nullptr) {
			LOCK_GUARD(ms_Lock);
			if (ms_pInst != nullptr) {
				JCORE_DELETE_SAFE(ms_pInst);
				ms_bDeleted = true;
			}
		}
	}
private:
	inline static T* ms_pInst;
	inline static NormalLock ms_Lock;
	inline static bool ms_bDeleted;
};

NS_JC_END
