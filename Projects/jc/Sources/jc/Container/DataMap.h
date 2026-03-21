/*
 * 작성자: 윤정도
 * 설명: 다양한 타입의 데이터를 저장할 수 있는 variant 기반 맵 컨테이너
 */

#pragma once

#include "jc/Container/HashMap.h"
#include "jc/Container/TreeMap.h"
#include "jc/Primitives/String.h"
#include "jc/Memory.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
// DataVariantType - 저장할 수 있는 데이터 타입 열거형
//////////////////////////////////////////////////////////////////////////////////////////
enum class DataVariantType : _u8
{
	dvt_none = 0,
	dvt_s32,
	dvt_u32,
	dvt_s64,
	dvt_u64,
	dvt_f32,
	dvt_f64,
	dvt_ptr,
	dvt_string,
	dvt_bool,
	dvt_max
};

//////////////////////////////////////////////////////////////////////////////////////////
// DataVariant - 타입 정보와 실제 값을 저장하는 구조체
//////////////////////////////////////////////////////////////////////////////////////////
struct DataVariant
{
	DataVariantType type_;
	_u32 length_;
	union
	{
		_s32 s32_;
		_u32 u32_;
		_s64 s64_;
		_u64 u64_;
		_f32 f32_;
		_f64 f64_;
		_ptr ptr_;
	};

	DataVariant() : type_(DataVariantType::dvt_none), length_(0), s32_(0) {}
	DataVariant(const DataVariant& _other) : type_(_other.type_), length_(_other.length_)
	{
		Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64)); // 기본적으로 shallow copy임.
	}
	DataVariant(DataVariant&& _other) noexcept : type_(_other.type_), length_(_other.length_)
	{
		Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64)); // 기본적으로 shallow copy임.
	}

	DataVariant& operator=(const DataVariant& _other)
	{
		if (this != &_other)
		{
			type_ = _other.type_;
			length_ = _other.length_;
			Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64));
		}
		return *this;
	}

	DataVariant& operator=(DataVariant&& _other) noexcept
	{
		if (this != &_other)
		{
			type_ = _other.type_;
			length_ = _other.length_;
			Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64));
		}
		return *this;
	}

	template <typename TAllocator>
	void CopyFrom(DataVariant& _other, bool _move = false)
	{
		if (this != &_other)
		{
			DataVariantType otherType = _other.type_; // 기존 타입 저장

			if (type_ == DataVariantType::dvt_string)
			{
				if (_other.type_ == DataVariantType::dvt_string)
				{
					String* pStr = reinterpret_cast<String*>(ptr_);
					jc_assert(pStr); // nullptr 일 수 없다.

					String* pTargetStr = reinterpret_cast<String*>(_other.ptr_);
					jc_assert(pTargetStr); // nullptr 일 수 없다.

					if (_move)
					{
						*pStr = Move(*pTargetStr); // String의 이동 대입 연산자 호출
						pTargetStr = nullptr;
						_other.Free<TAllocator>(); // 이동 대상의 리소스 해제
					}
					else
					{
						*pStr = *pTargetStr; // String의 복사 대입 연산자 호출
					}
				}
				else
				{
					Free<TAllocator>();
					Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64));
				}
			}
			else if (_other.type_ == DataVariantType::dvt_string)
			{
				String* pTargetStr = reinterpret_cast<String*>(_other.ptr_);
				jc_assert(pTargetStr); // nullptr 일 수 없다.

				String* pAllocatedStr = TAllocator::template AllocateInitStatic<String>(_move ? Move(*pTargetStr) : *pTargetStr);
				if (_move)
				{
					pTargetStr = nullptr;
					_other.Free<TAllocator>();
				}
				if (pAllocatedStr == nullptr)
				{
					jc_assert_msg(false, "문자열 할당에 실패했습니다.");
					return;
				}
				ptr_ = reinterpret_cast<_ptr>(pAllocatedStr);
				
			}
			else
			{
				Memory::CopyUnsafe(&u64_, &_other.u64_, sizeof(_u64));
			}

			// 타입과 길이는 맨 마지막에 복사 해야함.
			type_ = otherType;
			length_ = _other.length_;
		}
	}

	template <typename TAllocator>
	void Free()
	{
		if (type_ == DataVariantType::dvt_string)
		{
			String* pStr = reinterpret_cast<String*>(ptr_);
			if (pStr)
			{
				pStr->~String();
				TAllocator::template DeallocateStatic<String>(pStr);
			}
			ptr_ = 0;
			length_ = 0;
			type_ = DataVariantType::dvt_none;
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// CDataMap - 다양한 타입의 데이터를 String 키로 저장하는 컨테이너
//////////////////////////////////////////////////////////////////////////////////////////
template <typename TMapContainer = HashMap<String, DataVariant>, typename TAllocator = CDefaultAllocator>
class CDataMap
{
	using TDataMap = CDataMap<TMapContainer, TAllocator>;

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	CDataMap() {}
	CDataMap(const TDataMap& _other)
	{
		operator=(_other);
	}
	CDataMap(TDataMap&& _other) noexcept
	{
		operator=(Move(_other));
	}

	~CDataMap() noexcept
	{
		Clear();
	}

	TDataMap& operator=(const TDataMap& _other)
	{
		if (this != &_other)
		{
			Clear();
			AddData(_other, false);
		}
		return *this;
	}

	TDataMap& operator=(TDataMap&& _other) noexcept
	{
		if (this != &_other)
		{
			Clear();
			map_ = Move(_other.map_);
		}
		return *this;
	}

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - S32
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetS32(const String& _key, _s32 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_s32;
		variant.length_ = sizeof(_s32);
		variant.s32_ = _value;

		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - U32
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetU32(const String& _key, _u32 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_u32;
		variant.length_ = sizeof(_u32);
		variant.u32_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - S64
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetS64(const String& _key, _s64 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_s64;
		variant.length_ = sizeof(_s64);
		variant.s64_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - U64
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetU64(const String& _key, _u64 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_u64;
		variant.length_ = sizeof(_u64);
		variant.u64_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - Float
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetFloat(const String& _key, _f32 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_f32;
		variant.length_ = sizeof(_f32);
		variant.f32_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - Double
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetDouble(const String& _key, _f64 _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_f64;
		variant.length_ = sizeof(_f64);
		variant.f64_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - Ptr
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetPtr(const String& _key, void* _value)
	{
		return SetPtr(_key, (_ptr)_value);
	}

	bool SetPtr(const String& _key, _ptr _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_ptr;
		variant.length_ = sizeof(_ptr);
		variant.ptr_ = _value;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - String
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetString(const String& _key, const String& _value)
	{
		String* pAllocatedStr = TAllocator::template AllocateInitStatic<String>(_value);
		if (pAllocatedStr == nullptr)
		{
			jc_assert_msg(false, "문자열 할당에 실패했습니다.");
			return false;
		}
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_string;
		variant.length_ = sizeof(_ptr);
		variant.ptr_ = reinterpret_cast<_ptr>(pAllocatedStr);

		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant, true); // 기존 객체로 문자열만 복사해서 넣어주면 되므로. move 시켜준다.
			return true;
		}
		return map_.Insert(_key, variant);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Setter - Bool
	//////////////////////////////////////////////////////////////////////////////////////////
	bool SetBool(const String& _key, bool _value)
	{
		DataVariant variant;
		variant.type_ = DataVariantType::dvt_bool;
		variant.length_ = sizeof(_u32);
		variant.s32_ = _value ? 1 : 0;
		
		DataVariant* pOld = map_.Find(_key);
		if (pOld != nullptr)
		{
			pOld->CopyFrom<TAllocator>(variant);
			return true;
		}
		return map_.Insert(_key, variant);
	}

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - S32
	//////////////////////////////////////////////////////////////////////////////////////////
	_s32 GetS32(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_s32)
			return pVariant->s32_;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - U32
	//////////////////////////////////////////////////////////////////////////////////////////
	_u32 GetU32(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_u32)
			return pVariant->u32_;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - S64
	//////////////////////////////////////////////////////////////////////////////////////////
	_s64 GetS64(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_s64)
			return pVariant->s64_;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - U64
	//////////////////////////////////////////////////////////////////////////////////////////
	_u64 GetU64(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_u64)
			return pVariant->u64_;
		return 0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - Float
	//////////////////////////////////////////////////////////////////////////////////////////
	_f32 GetFloat(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_f32)
			return pVariant->f32_;
		return 0.0f;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - Double
	//////////////////////////////////////////////////////////////////////////////////////////
	_f64 GetDouble(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_f64)
			return pVariant->f64_;
		return 0.0;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - Ptr
	//////////////////////////////////////////////////////////////////////////////////////////
	void* GetPtr(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_ptr)
			return (void*)pVariant->ptr_;
		return nullptr;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - String
	//////////////////////////////////////////////////////////////////////////////////////////
	String GetString(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_string)
		{
			String* pStr = reinterpret_cast<String*>(pVariant->ptr_);
			if (pStr)
				return *pStr;
		}
		return String();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// Getter - Bool
	//////////////////////////////////////////////////////////////////////////////////////////
	bool GetBool(const String& _key) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_bool)
			return pVariant->s32_ != 0;
		return false;
	}

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - S32
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetS32(const String& _key, _s32& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_s32)
		{
			_outValue = pVariant->s32_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - U32
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetU32(const String& _key, _u32& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_u32)
		{
			_outValue = pVariant->u32_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - S64
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetS64(const String& _key, _s64& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_s64)
		{
			_outValue = pVariant->s64_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - U64
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetU64(const String& _key, _u64& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_u64)
		{
			_outValue = pVariant->u64_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - Float
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetFloat(const String& _key, _f32& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_f32)
		{
			_outValue = pVariant->f32_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - Double
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetDouble(const String& _key, _f64& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_f64)
		{
			_outValue = pVariant->f64_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - Ptr
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetPtr(const String& _key, OUT _ptr& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_ptr)
		{
			_outValue = pVariant->ptr_;
			return true;
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - String
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetString(const String& _key, String& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_string)
		{
			String* pStr = reinterpret_cast<String*>(pVariant->ptr_);
			if (pStr)
			{
				_outValue = *pStr;
				return true;
			}
		}
		return false;
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// TryGet 메서드 - Bool
	//////////////////////////////////////////////////////////////////////////////////////////
	bool TryGetBool(const String& _key, bool& _outValue) const
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant && pVariant->type_ == DataVariantType::dvt_bool)
		{
			_outValue = pVariant->s32_ != 0;
			return true;
		}
		return false;
	}

public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// AddData - 다른 CDataMap의 데이터를 병합
	//////////////////////////////////////////////////////////////////////////////////////////
	void AddData(const TDataMap& _data, bool _overrideExist)
	{
		_data.map_.ForEach([this, _overrideExist](typename TMapContainer::TKeyValuePair& _pair)
		{
			DataVariant* pExistingVariant = map_.Find(_pair.key_);

			if (_overrideExist || pExistingVariant == nullptr)
			{
				if (pExistingVariant)
				{
					pExistingVariant->CopyFrom<TAllocator>(_pair.value_);
				}
				else
				{
					DataVariant variant;
					variant.CopyFrom<TAllocator>(_pair.value_);
					map_.Insert(_pair.key_, variant);
				}
			}
		});
	}


public:
	//////////////////////////////////////////////////////////////////////////////////////////
	// 유틸리티 메서드
	//////////////////////////////////////////////////////////////////////////////////////////
	bool Exist(const String& _key) const
	{
		return map_.Exist(_key);
	}

	bool Remove(const String& _key)
	{
		DataVariant* pVariant = map_.Find(_key);
		if (pVariant)
		{
			pVariant->Free<TAllocator>();
			return map_.Remove(_key);
		}
		return false;
	}

	void Clear()
	{
		map_.ForEachValue([this](DataVariant& _variant)
		{
			if (_variant.type_ == DataVariantType::dvt_string)
			{
				_variant.Free<TAllocator>();
			}
		});
		map_.Clear();
	}

	int Size() const
	{
		return map_.Size();
	}

	bool IsEmpty() const
	{
		return map_.IsEmpty();
	}

private:
	TMapContainer map_;
};

NS_END
