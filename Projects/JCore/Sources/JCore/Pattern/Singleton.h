/*
 * 작성자: 윤정도
 * 생성일: 4/19/2023 5:36:01 PM
 * =====================
 * 나만의 슁글톤
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
 *  ==========================================================================
 *  OnDestroy()함수는 소멸자 호출 대용이다. (소멸자는 신경쓰지 않고 매크로로 처리하기 위함.)
 *
 *  [예시코드]
 *	struct A final : SingletonStatic<A>
 *	{
 *	private:
 *		JCoreSingletonNoConstructor(A) : p(1, 2) {}
 *	protected:
 *		void OnDestroy() override {
 *			std::cout << "객체 소멸시 수행할 작업은 여기서 처리\n";
 *		}
 *	private:
 *		Point p;	// 만약 Point가 생성자를 필수로 요구한다면 JCoreSingletonNoConstructor 매크로를 사용
 *	};
 *
 *  [예시코드 - 생성자로 초기화 해줄게 없는 경우]
 *	struct A final : SingletonStatic<A>
 *	{
 *	private:
 *		JCoreSingletonSimple(A);
 *	protected:
 *		void OnDestroy() override {
 *			std::cout << "객체 소멸시 수행할 작업은 여기서 처리\n";
 *		}
 *	private:
 *		Point p;	// 만약 Point가 디폴트 생성자가 있다면 JCoreSingletonSimple 매크로로 생략해서 구현
 *	};
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

	virtual ~SingletonStatic() {
		((T*)this)->OnSingletonDestroy();
	}
public:
	using TSingleton = SingletonStatic<T>;
	virtual void OnSingletonDestroy() = 0;

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

	virtual ~SingletonPointer() {
		((T*)this)->OnSingletonDestroy();
	}
public:
	using TSingleton = SingletonPointer<T>;
	virtual void OnSingletonDestroy() = 0;

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
				DeleteSafe(ms_pInst);
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

#define JCoreSingletonSimple(class_name)								   \
friend class TSingleton;												   \
private:																   \
	~class_name() = default;											   \
	class_name() = default											   

#define JCoreSingletonNoConstructor(class_name)							   \
friend class TSingleton;												   \
private:																   \
	~class_name() = default;											   \
	class_name()														  

								   

