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
 *    2. 오브젝트가 프로그램 종료시 자동 소멸된다.
 *    3. 구현이 쉽고 간단하다.
 *
 * 2. 포인터 싱글톤은 메모리 관리에 용이하다.
 *    1. 정적 객체는 프로그램 시작시 메모리에 용량을 차지하고 있기 때문에
 *       늦게 생성된다고 하더라도 무거운 객체의 경우 비효율적일 수 있다.
 *
 * 아래 3가지 케이스 모두 장단점이 존재한다.
 *
 */


#pragma once

#include <jc/Pattern/NonCopyableh.h>
#include <jc/Sync/NormalLock.h>
#include <jc/Debug/New.h>

NS_JC_BEGIN

template <typename T>
class SingletonStatic : private NonCopyableNonMovable
{
protected:
	SingletonStatic() = default;
	~SingletonStatic() = default;

public:
	using TSingleton = SingletonStatic<T>;

	static T* Get()
	{
		static T instance;
		return &instance;
	}
};

template <typename T>
class SingletonStaticPointer : private NonCopyableNonMovable
{
	struct Guard
	{
		~Guard()
		{
			JC_DELETE_SAFE(ms_pInst);
		}
	};

protected:
	SingletonStaticPointer() = default;
	~SingletonStaticPointer() = default;

public:
	using TSingleton = SingletonStaticPointer<T>;

	static T* Get()
	{
		static T* pInst = []
		{
			ms_pInst = dbg_new T;
			return ms_pInst;
		} ();
		static Guard guard;

		if (pInst == nullptr)
		{
			jc_assert_msg(false, "삭제된 객체에 접근을 시도했습니다.");
			return nullptr;
		}

		return pInst;
	}

	static void Free()
	{
		JC_DELETE_SAFE(ms_pInst);
	}

	inline static T* ms_pInst;
};

// novtable사용시에는 사용하지말것
template <typename T>
class SingletonPointer : private NonCopyableNonMovable
{
protected:
	SingletonPointer() = default;
	~SingletonPointer() = default;

public:
	using TSingleton = SingletonPointer<T>;

	static T* Get()
	{
		if (ms_pInst == nullptr)
		{
			// JC_LOCK_GUARD(ms_Lock);
			// TODO: 락가드 사용시 오류난다. 인증/로비/게임 서버 프로젝트에서 jnet::NetGroupMgr 소멸시 넷그룹 해쉬맵 제거되면서 오류가 발생함. 왜 그런지 모르겠다.
			// 해쉬맵 말고 다른 컨테이너를 사용할땐 문제가 없는데..
			// 추정: 느낌이 이 코드실행이전에 가상함수 테이블이 소멸되어서 그런건가?

			ms_Lock.Lock();
			if (ms_bDeleted)
			{
				jc_assert_msg(false, "삭제된 객체에 접근을 시도했습니다.");
				ms_Lock.Unlock();
				return nullptr;
			}

			if (ms_pInst == nullptr)
			{
				ms_pInst = dbg_new T;
			}
			ms_Lock.Unlock();
		}

		return ms_pInst;
	}

	static void Free()
	{
		if (ms_pInst != nullptr)
		{
			// JC_LOCK_GUARD(ms_Lock);

			ms_Lock.Lock();
			if (ms_pInst != nullptr)
			{
				JC_DELETE_SAFE(ms_pInst);
				ms_bDeleted = true;
			}
			ms_Lock.Unlock();
		}
	}

	static bool Singleton_IsDeleted() 
	{ 
		return ms_bDeleted;
	}

public:
	inline static T* __sSingletonInst = nullptr;	// 직접 접근 금지. JC_DECL_SINGLETON_BODY 매크로에서만 사용
private:
	inline static T* ms_pInst;
	inline static NormalLock ms_Lock;
	inline static bool ms_bDeleted;
};


template <typename T>
class SingletonSPointer : private NonCopyableNonMovable
{
protected:
	using ReturnTy = jc::SharedPtr<T>;

	SingletonSPointer() = default;
	~SingletonSPointer() = default;

public:
	using TSingleton = SingletonSPointer<T>;

	static T* Get()
	{
		// 최초 접근 시에만 생성
		if (ms_pInst == nullptr)
		{
			ms_pInst = MakeShared<T>();
		}

		return ms_pInst.GetPtr();
	}

	static jc::SharedPtr<T> GetShared()
	{
		if (ms_pInst == nullptr)
		{
			ms_pInst = MakeShared<T>();
		}

		return ms_pInst;
	}

	static void Free()
	{
		ms_pInst = nullptr;
	}

public:
	inline static T* __sSingletonInst = nullptr;
private:
	inline static SharedPtr<T> ms_pInst;
};


#define JC_DECL_SINGLETON_BODY(type_name) \
	(*(type_name::__sSingletonInst ? type_name::__sSingletonInst : (type_name::__sSingletonInst = type_name::Get())))

NS_END
