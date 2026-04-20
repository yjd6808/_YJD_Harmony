/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 StringUtil API를 호출하기 위한 extern "C" 래퍼 구현
 * created by AI.
 */

#include "Extern_StringUtil.h"
#include "jc/Primitives/StringUtil.h"

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_U32(_s8* _pBuff, _s32 _capacity, _u32 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_S32(_s8* _pBuff, _s32 _capacity, _s32 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_U64(_s8* _pBuff, _s32 _capacity, _u64 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_S64(_s8* _pBuff, _s32 _capacity, _s64 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_Float(_s8* _pBuff, _s32 _capacity, _f32 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToStringBuffered_Double(_s8* _pBuff, _s32 _capacity, _f64 _value)
{
    return StringUtil::ToStringBuffered(_pBuff, _capacity, _value);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToNumber_S32(const _s8* _pStr)
{
    return StringUtil::ToNumber<_s32>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_ToNumber_S32_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_s32>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 StringUtil_ToNumber_U32(const _s8* _pStr)
{
    return StringUtil::ToNumber<_u32>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u32 StringUtil_ToNumber_U32_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_u32>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 StringUtil_ToNumber_S64(const _s8* _pStr)
{
    return StringUtil::ToNumber<_s64>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s64 StringUtil_ToNumber_S64_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_s64>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 StringUtil_ToNumber_U64(const _s8* _pStr)
{
    return StringUtil::ToNumber<_u64>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_u64 StringUtil_ToNumber_U64_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_u64>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 StringUtil_ToNumber_Float(const _s8* _pStr)
{
    return StringUtil::ToNumber<_f32>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f32 StringUtil_ToNumber_Float_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_f32>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 StringUtil_ToNumber_Double(const _s8* _pStr)
{
    return StringUtil::ToNumber<_f64>(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_f64 StringUtil_ToNumber_Double_Ext(const _s8* _pStr, _s8** _ppEndptr, _s32 _ignoreLeadingZero)
{
    return StringUtil::ToNumber<_f64>(_pStr, reinterpret_cast<char**>(_ppEndptr), _ignoreLeadingZero != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_S32(_s32* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_s32>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_S32_Ext(_s32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_s32>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_U32(_u32* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_u32>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_U32_Ext(_u32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_u32>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_S64(_s64* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_s64>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_S64_Ext(_s64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_s64>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_U64(_u64* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_u64>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_U64_Ext(_u64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_u64>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_Float(_f32* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_f32>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_Float_Ext(_f32* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_f32>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_Double(_f64* _pVal, const _s8* _pStr)
{
    return StringUtil::TryToNumber<_f64>(*_pVal, _pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_TryToNumber_Double_Ext(_f64* _pVal, const _s8* _pStr, _s32 _ignoreLeadingZero)
{
    return StringUtil::TryToNumber<_f64>(*_pVal, _pStr, _ignoreLeadingZero != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_Length(const _s8* _pStr)
{
    return StringUtil::Length(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_LengthWithNull(const _s8* _pStr)
{
    return StringUtil::LengthWithNull(_pStr);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_IsNullOrEmpty(const _s8* _pStr)
{
    return StringUtil::IsNullOrEmpty(_pStr) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_Copy(_s8* _pBuffer, _s32 _bufferSize, const _s8* _pCopy)
{
    return StringUtil::Copy(_pBuffer, _bufferSize, _pCopy);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_CopyUnsafe(_s8* _pBuffer, const _s8* _pCopy)
{
    return StringUtil::CopyUnsafe(_pBuffer, _pCopy);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_IsEqual(const _s8* _pSrc, const _s8* _pDst, _s32 _bCompareCase)
{
    return StringUtil::IsEqual(_pSrc, _pDst, _bCompareCase != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_IsEqualLen(const _s8* _pSrc, _s32 _srcLen, const _s8* _pDst, _s32 _dstLen, _s32 _bCompareCase)
{
    return StringUtil::IsEqual(_pSrc, _srcLen, _pDst, _dstLen, _bCompareCase != 0) ? 1 : 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_Find(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive)
{
    return StringUtil::Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindLen(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _strLen, _s32 _caseSensitive)
{
    return StringUtil::Find(_pSource, _sourceLen, _startIdx, _endIdx, _pStr, _strLen, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindFrom(const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, const _s8* _pStr, _s32 _caseSensitive)
{
    return StringUtil::Find(_pSource, _sourceLen, _startIdx, _pStr, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindAll(_s32* _pPositionArray, const _s8* _pSource, const _s8* _pStr, _s32 _caseSensitive)
{
    return StringUtil::FindAll(_pPositionArray, _pSource, _pStr, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindAllLen(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, const _s8* _pStr, _s32 _caseSensitive)
{
    return StringUtil::FindAll(_pPositionArray, _pSource, _sourceLen, _pStr, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindAllRange(_s32* _pPositionArray, const _s8* _pSource, _s32 _sourceLen, _s32 _startIdx, _s32 _endIdx, const _s8* _pStr, _s32 _caseSensitive)
{
    return StringUtil::FindAll(_pPositionArray, _pSource, _sourceLen, _startIdx, _endIdx, _pStr, _caseSensitive != 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindChar(const _s8* _pSource, _s8 _ch)
{
    return StringUtil::FindChar(_pSource, _ch);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindCharReverse(const _s8* _pSource, _s8 _ch)
{
    return StringUtil::FindCharReverse(_pSource, _ch);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindCharReverseLen(const _s8* _pSource, _s32 _len, _s8 _ch)
{
    return StringUtil::FindCharReverse(_pSource, _len, _ch);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_FindCharUncontained(const _s8* _pSource, _s8 _ch)
{
    return StringUtil::FindCharUncontained(_pSource, _ch);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_Compare(const _s8* _pStr1, const _s8* _pStr2)
{
    return StringUtil::Compare(_pStr1, _pStr2);
}

//////////////////////////////////////////////////////////////////////////////////////////
_s32 StringUtil_CompareLen(const _s8* _pStr1, _s32 _str1Len, const _s8* _pStr2, _s32 _str2Len)
{
    return StringUtil::Compare(_pStr1, _str1Len, _pStr2, _str2Len);
}
