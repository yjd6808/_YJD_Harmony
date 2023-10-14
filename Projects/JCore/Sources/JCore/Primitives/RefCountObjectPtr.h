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
 *		JCORE_RELEASE_SAFE(pRef) // RefCount == 0 -> ReleaseAction 호출
 *	}
 */


#pragma once


#include <JCore/Primitives/RefCountObject.h>
#include <JCore/TypeCast.h>

NS_JC_BEGIN

template <typename T>
class RefCountObjectPtr
{
	static_assert(JCore::IsBaseOf_v<RefCountObject, T>, "... T is not base of RefCountObject");
	static_assert(JCore::IsSameType_v<NakedType_t<T>*, T*>, "... T is not RefCountObject*");

	using TThis = RefCountObjectPtr<T>;
	using TField = T;

	#pragma region Validator
	template <typename U>
	struct IsRefCountObjectPtrType : FalseType {};
	template <template <typename> typename Base, typename U>
	struct IsRefCountObjectPtrType<Base<U>> : Conditional_t<IsSameType_v<Base<U>, RefCountObjectPtr<U>>, TrueType, FalseType> {};

	template <typename U>
	static constexpr bool ValidateType() {
		constexpr bool bValidType = IsRefCountObjectPtrType<NakedType_t<U>>::Value;
		static_assert(bValidType, "... U is not RefCountObjectPtr type");
		return bValidType;
	}

	template <typename UField>
	static constexpr bool ValidateField() {
		constexpr bool bFieldConversion = JCore::IsConvertible_v<UField*, TField*>;
		static_assert(bFieldConversion, "... cannot convert! TField = UField");
		return bFieldConversion;
	}

	#pragma endregion
public:
	RefCountObjectPtr() : m_pRefCountObj(nullptr) {}
	RefCountObjectPtr(T* ref, bool addRef = true) : m_pRefCountObj(ref) {
		if (addRef && m_pRefCountObj)
			m_pRefCountObj->AddRef();
	}

	RefCountObjectPtr(const TThis& other) : m_pRefCountObj(nullptr) { CopyFrom(other); }
	RefCountObjectPtr(TThis&& other) noexcept : m_pRefCountObj(nullptr) { MoveFrom(other); }

	template <typename U>
	RefCountObjectPtr(const RefCountObjectPtr<U>& other) : m_pRefCountObj(nullptr) {
		if constexpr (ValidateField<U>())
			CopyFrom(other);
	}

	template <typename U>
	RefCountObjectPtr(RefCountObjectPtr<U>&& other) : m_pRefCountObj(nullptr) {
		if constexpr (ValidateField<U>())
			MoveFrom(other);
	}
	
	~RefCountObjectPtr() {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();
	}

	T* GetPtr() const { return m_pRefCountObj; }

	TThis& operator=(const TThis& other) {
		CopyFrom(other);
		return *this;
	}
	TThis& operator=(TThis&& other) noexcept {
		MoveFrom(other);
		return *this;
	}

	template <typename U>
	TThis& operator=(const RefCountObjectPtr<U>& other) {
		if constexpr (ValidateField<U>())
			CopyFrom(other);
		return *this;
	}
	template <typename U>
	TThis& operator=(RefCountObjectPtr<U>&& other) noexcept {
		if constexpr (ValidateField<U>())
			MoveFrom(other);
		return *this;
	}

	bool operator==(const TThis& other) const { return m_pRefCountObj == other.m_pRefCountObj; }
	bool operator==(RefCountObject* other) const { return m_pRefCountObj == other; }
	bool operator!=(const TThis& other) const { return m_pRefCountObj != other.m_pRefCountObj; }
	bool operator!=(RefCountObject* other) const { return m_pRefCountObj != other; }
	T* operator->() { return m_pRefCountObj; }
	T& operator*() { return *m_pRefCountObj; }

	operator bool() const { return m_pRefCountObj; }	// if statement 사용가능
	operator void*() const { return m_pRefCountObj; }	// void* 대입가능

private:
	template <typename U>
	void CopyFrom(U& other) {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();

		m_pRefCountObj = other.m_pRefCountObj;

		if (m_pRefCountObj)
			m_pRefCountObj->AddRef();
	}

	template <typename U>
	void MoveFrom(U& other) {
		if (m_pRefCountObj)
			m_pRefCountObj->Release();

		m_pRefCountObj = other.m_pRefCountObj;

		if (m_pRefCountObj)
			m_pRefCountObj->AddRef();

		// 나중에 해줘야함. (0되면 ReleaseAction을 수행하기 때문)
		if (other.m_pRefCountObj)
			other.m_pRefCountObj->Release();
	}

	
	T* m_pRefCountObj;

	template <typename> friend class RefCountObjectPtr;
};

NS_JC_END


#define JCORE_REF_COUNT_GUARD(...)				JCORE_EXPAND_1(JCORE_CONCAT_ARGS(JCORE_REF_COUNT_GUARD_, JCORE_ARGS_COUNT(__VA_ARGS__))(__VA_ARGS__)) 
#define JCORE_REF_COUNT_GUARD_1(ref)			JCore::RefCountObjectPtr<JCore::RefCountObject> JCORE_CONCAT_COUNTER(__ref_count_guard__)(ref)
#define JCORE_REF_COUNT_GUARD_2(ref, add_ref)	JCore::RefCountObjectPtr<JCore::RefCountObject> JCORE_CONCAT_COUNTER(__ref_count_guard__)(ref, add_ref)
