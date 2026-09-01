/*
 * 작성자: 윤정도
 * 생성일: 5/19/2024 9:36:00 AM
 * =====================
 *
 */


#pragma once

NS_JC_BEGIN

template <typename T>
struct JC_DLL Span
{
	Span()
		: pArr_(nullptr)
		, len_(0)
	{}
	Span(T* _pArr, _u32 _len)
	: pArr_(_pArr)
	, len_(_len)
	{}
	Span(const Span& _other) = default;
	Span& operator=(const Span& _other) = default;

	T& operator[](_u32 _offset)
	{
		static T d;
		if (_offset >= len_)
		{
			jc_assert(_offset < len_);
			return d;
		}
		
		return pArr_[_offset];
	}
	const T& operator[](_u32 _offset) const
	{
		static T d;
		if (_offset >= len_)
		{
			jc_assert(_offset < len_);
			return d;
		}
		return pArr_[_offset];
	}

	_u32 Size() const { return len_; }
	bool IsEmpty() const { return len_ == 0; }
	T* Data() const { return pArr_; }

	explicit operator bool() const { return pArr_ != nullptr && len_ != 0; }

	T* pArr_;
	_u32 len_;
};

NS_END