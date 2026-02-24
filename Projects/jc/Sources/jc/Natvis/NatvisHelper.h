#pragma once

#include <jc/Namespace.h>
#include <jc/Type.h>

// 23년 02월 06일 오전 11시
// https://stackoverflow.com/questions/31689857/natvis-floating-point-format
// 프리시전용 float 추가, 이 생각을 왜 못했을까 ㄷㄷ
NS_JC_BEGIN
NS_DETAIL_BEGIN

struct _u8n		{ _u8 value_{}; };
struct _u16n	{ _u16 value_{}; };
struct _u32n	{ _u32 value_{}; };
struct _u64n	{ _u64 value_{}; };
struct _s8n		{ _s8 value_{}; };
struct _s16n	{ _s16 value_{}; };
struct _s32n	{ _s32 value_{}; };
struct _s64n	{ _s64 value_{}; };
struct _f32n	{ _f32 value_{}; };
struct _f64n	{ _f64 value_{}; };
struct _ptrn	{ void* value_{}; static constexpr _u32 SIZE = sizeof(void*); };

struct CMessage_BinaryLengthText { _u32 value_{}; };
struct CMessage_VariantTraits_s8 { _s8 value_; };
struct CMessage_VariantTraits_s16 { _s16 value_; };
struct CMessage_VariantTraits_s32 { _s32 value_; };
struct CMessage_VariantTraits_s64 { _s64 value_; };
struct CMessage_VariantTraits_u8 { _u8 value_; };
struct CMessage_VariantTraits_u16 { _u16 value_; };
struct CMessage_VariantTraits_u32 { _u32 value_; };
struct CMessage_VariantTraits_u64 { _u64 value_; };
struct CMessage_VariantTraits_f32 { _f32 value_; };
struct CMessage_VariantTraits_f64 { _f64 value_; };
struct CMessage_VariantTraits_ptr { _ptr value_; };


struct NatvisFloat6 { float f{}; };
struct NatvisFloat5 { float f{}; };
struct NatvisFloat4 { float f{}; };
struct NatvisFloat3 { float f{}; };
struct NatvisFloat2 { float f{}; };
struct NatvisFloat1 { float f{}; };

struct NatvisDouble6 { double f{}; };
struct NatvisDouble5 { double f{}; };
struct NatvisDouble4 { double f{}; };
struct NatvisDouble3 { double f{}; };
struct NatvisDouble2 { double f{}; };
struct NatvisDouble1 { double f{}; };

// 일단 포트 변환땜에 하나만 만듬
union NatvisByteOrderInt16U
{
	struct Field
	{
		_u8 seg1; // 0xbb
		_u8 seg2; // 0xaa
	} segs;

	_u16 val; // 0xaabb
};

NS_END
NS_END
