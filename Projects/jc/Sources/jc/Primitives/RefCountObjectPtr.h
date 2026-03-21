/*
 * 작성자: 윤정도
 * 생성일: 7/30/2023 6:35:01 PM
 * =====================
 * 사용법
 *
 *	{
 *		RefCountObjectPtr<Nice> sp{ dbg_new Nice{2}, false };
 *		// RefCount == 1
 *	} // RefCount == 0 -> ReleaseAction 호출
 *
 *  {
 *		RefCountObject* pRef = dbg_new Nice{2};
 *		// RefCount == 1
 *		{
 *			RefCountObjectPtr<Nice> sp{ pRef };
 *			// RefCount == 2
 *		}
 *		// RefCount == 1
 *		JC_RELEASE_SAFE(pRef) // RefCount == 0 -> ReleaseAction 호출
 *	}
 */


#pragma once


#include "jc/Primitives/RefCountObject.h"
#include "jc/TypeCast.h"

NS_JC_BEGIN

template <typename T>
class RefCountObjectPtr
{
	static_assert(jc::IsBaseOf_v<RefCountObject, T>, "... T is not base of RefCountObject");
	static_assert(jc::IsSameType_v<NakedType_t<T>*, T*>, "... T is not RefCountObject*");

	using TThis = RefCountObjectPtr<T>;
	using TField = T;

	#pragma region Validator
	template <typename U>
	struct IsRefCountObjectPtrType : FalseType {};
	template <template <typename> typename Base, typename U>
	struct IsRefCountObjectPtrType<Base<U>> : Conditional_t<IsSameType_v<Base<U>, RefCountObjectPtr<U>>, TrueType, FalseType> {};

	template <typename U>
	static constexpr bool ValidateType() {
		constexpr bool VALID_TYPE = IsRefCountObjectPtrType<NakedType_t<U>>::Value;
		static_assert(VALID_TYPE, "... U is not RefCountObjectPtr type");
		return VALID_TYPE;
	}

	template <typename UField>
	static constexpr bool ValidateField() {
		constexpr bool FIELD_CONVERSION = jc::IsConvertible_v<UField*, TField*>;
		static_assert(FIELD_CONVERSION, "... cannot convert! TField = UField");
		return FIELD_CONVERSION;
	}

	#pragma endregion
public:
	RefCountObjectPtr() : m_pRefCountObj(nullptr) {}
	RefCountObjectPtr(T* _pRef, bool _addRef = true) : m_pRefCountObj(_pRef) {
		if (_addRef && m_pRefCountObj)
			m_pRefCountObj->AddRef();
	}

	RefCountObjectPtr(const TThis& _other) : m_pRefCountObj(nullptr) { CopyFrom(_other); }
	RefCountObjectPtr(TThis&& _other) noexcept : m_pRefCountObj(nullptr) { MoveFrom(_other); }

	template <typename U>
	RefCountObjectPtr(const RefCountObjectPtr<U>& _other) : m_pRefCountObj(nullptr) {
		if constexpr (ValidateField<U>())
			CopyFrom(_other);
	}

	template <typename U>
	RefCountObjectPtr(RefCountObjectPtr<U>&& _other) : m_pRefCountObj(nullptr) {
		if constexpr (ValidateField<U>())
			MoveFrom(_other);
	}
	
	~RefCountObjectPtr() {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();
	}

	T* GetPtr() const { return m_pRefCountObj; }

	TThis& operator=(const TThis& _other) {
		CopyFrom(_other);
		return *this;
	}
	TThis& operator=(TThis&& _other) noexcept {
		MoveFrom(_other);
		return *this;
	}

	template <typename U>
	TThis& operator=(const RefCountObjectPtr<U>& _other) {
		if constexpr (ValidateField<U>())
			CopyFrom(_other);
		return *this;
	}
	template <typename U>
	TThis& operator=(RefCountObjectPtr<U>&& _other) noexcept {
		if constexpr (ValidateField<U>())
			MoveFrom(_other);
		return *this;
	}

	bool operator==(const TThis& _other) const { return m_pRefCountObj == _other.m_pRefCountObj; }
	bool operator==(RefCountObject* _pOther) const { return m_pRefCountObj == _pOther; }
	bool operator!=(const TThis& _other) const { return m_pRefCountObj != _other.m_pRefCountObj; }
	bool operator!=(RefCountObject* _pOther) const { return m_pRefCountObj != _pOther; }
	T* operator->() { return m_pRefCountObj; }
	T& operator*() { return *m_pRefCountObj; }

	operator bool() const { return m_pRefCountObj; }	// if statement 사용가능
	operator void*() const { return m_pRefCountObj; }	// void* 대입가능

private:
	template <typename U>
	void CopyFrom(U& _other) {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();

		m_pRefCountObj = _other.m_pRefCountObj;

		if (m_pRefCountObj)
			m_pRefCountObj->AddRef();
	}

	template <typename U>
	void MoveFrom(U& _other) {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();

		m_pRefCountObj = _other.m_pRefCountObj;

		if (m_pRefCountObj)
			m_pRefCountObj->AddRef();

		// 나중에 해줘야함. (0되면 ReleaseAction을 수행하기 때문)
		if (_other.m_pRefCountObj)
			_other.m_pRefCountObj->Release();
	}

	
	T* m_pRefCountObj;

	template <typename> friend class RefCountObjectPtr;
};

NS_END


#define JC_REF_COUNT_GUARD(...)				JC_EXPAND_1(JC_CONCAT_ARGS(JC_REF_COUNT_GUARD_, JC_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)) 
#define JC_REF_COUNT_GUARD_1(ref)			jc::RefCountObjectPtr<jc::RefCountObject> JC_CONCAT_COUNTER(__ref_count_guard__)(ref)
#define JC_REF_COUNT_GUARD_2(ref, add_ref)	jc::RefCountObjectPtr<jc::RefCountObject> JC_CONCAT_COUNTER(__ref_count_guard__)(ref, add_ref)
