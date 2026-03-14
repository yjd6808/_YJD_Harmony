/*
 * 작성자: 윤정도
 * 생성일: 3/13/2026
 * =====================
 *
 * MySQL/SQLServer 공통 쿼리 클래스
 * MysqlQuery, SqlServerQuery가 상속한다.
 */

#pragma once

#include <jc/Assert.h>
#include <jc/Time.h>
#include <jc/Primitives/String.h>
#include <jc/Primitives/SmartPtr.h>
#include <jc/Type.h>

#include <jdb/Namespace.h>
#include <jdb/DLLExport.h>
#include <jdb/StatementType.h>

NS_JDB_BEGIN

class IConnection;

class JDB_DLL IQuery
{
public:
	virtual ~IQuery() = default;

	bool IsSuccess() const { return errorCode_ == 0; }
	bool IsFailed()  const { return errorCode_ != 0; }
	int  GetErrorCode() const { return errorCode_; }

	// select의 경우 Execute이후 Next로 커서가 끝까지 도달한 경우에 rowCount_가 유효하다.
	// update, delete, insert의 경우 Execute가 성공한 경우 유효하다.
	_u32 GetRowCount() const { return rowCount_; }

	StatementType GetStatementType() const { return stmtType_; }

	static StatementType ParseStatement(const jc::String& _statement);

	void	SetId(int _id) { id_ = _id; }
	int		GetId() const { return id_; }

	virtual bool Execute() = 0;

	// SELECT 전용 메서드 (기본 구현: 무동작)
	virtual bool         HasNext() const                        { return false; }
	virtual bool         Next()                                 { return false; }
	virtual int          GetFieldIndex(const char* _pFieldName) { return -1; }
	virtual const char*  GetRawString(const char* _pFieldName)  { return nullptr; }
	virtual const char*  GetRawString(int _fieldIndex)          { return nullptr; }
	virtual jc::DateTime GetDateTime(const char* _pFieldName)   { return 0; }
	virtual jc::DateTime GetDateTime(int _fieldIndex)           { return 0; }
	virtual _u32         GetFieldCount() const                  { return 0; }

	jc::String GetString(const char* _pFieldName) { return GetRawString(_pFieldName); }
	jc::String GetString(int _fieldIndex)          { return GetRawString(_fieldIndex); }



	template <typename T>
	T GetNumber(const char* _pFieldName)
	{
		const char* pRawString = GetRawString(_pFieldName);
		if (pRawString == nullptr)
			return 0;
		return jc::StringUtil::ToNumber<T>(pRawString);
	}

	template <typename T>
	T GetNumber(int _fieldIndex)
	{
		const char* pRawString = GetRawString(_fieldIndex);
		if (pRawString == nullptr)
			return 0;
		return jc::StringUtil::ToNumber<T>(pRawString);
	}

	template <typename T>
	bool TryGetNumber(const char* _pFieldName, T& _val, T _defaultValue = 0)
	{
		const char* pRawString = GetRawString(_pFieldName);
		if (pRawString == nullptr)
		{
			_val = _defaultValue;
			return false;
		}
		_val = jc::StringUtil::ToNumber<T>(pRawString);
		return true;
	}

	template <typename T>
	bool TryGetNumber(int _fieldIndex, T& _val, T _defaultValue = 0)
	{
		const char* pRawString = GetRawString(_fieldIndex);
		if (pRawString == nullptr)
		{
			_val = _defaultValue;
			return false;
		}
		_val = jc::StringUtil::ToNumber<T>(pRawString);
		return true;
	}

		// ===========================================================================================
	// 구체적 타입별 GetXXX 메서드
	// ===========================================================================================

	// _s8 (signed char)
	_s8 GetS8(const char* _pFieldName) { return GetNumber<_s8>(_pFieldName); }
	_s8 GetS8(int _fieldIndex) { return GetNumber<_s8>(_fieldIndex); }
	bool TryGetS8(const char* _pFieldName, _s8& _val, _s8 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetS8(int _fieldIndex, _s8& _val, _s8 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _u8 (unsigned char)
	_u8 GetU8(const char* _pFieldName) { return GetNumber<_u8>(_pFieldName); }
	_u8 GetU8(int _fieldIndex) { return GetNumber<_u8>(_fieldIndex); }
	bool TryGetU8(const char* _pFieldName, _u8& _val, _u8 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetU8(int _fieldIndex, _u8& _val, _u8 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _s16 (short)
	_s16 GetS16(const char* _pFieldName) { return GetNumber<_s16>(_pFieldName); }
	_s16 GetS16(int _fieldIndex) { return GetNumber<_s16>(_fieldIndex); }
	bool TryGetS16(const char* _pFieldName, _s16& _val, _s16 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetS16(int _fieldIndex, _s16& _val, _s16 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _u16 (unsigned short)
	_u16 GetU16(const char* _pFieldName) { return GetNumber<_u16>(_pFieldName); }
	_u16 GetU16(int _fieldIndex) { return GetNumber<_u16>(_fieldIndex); }
	bool TryGetU16(const char* _pFieldName, _u16& _val, _u16 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetU16(int _fieldIndex, _u16& _val, _u16 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _s32 (int)
	_s32 GetS32(const char* _pFieldName) { return GetNumber<_s32>(_pFieldName); }
	_s32 GetS32(int _fieldIndex) { return GetNumber<_s32>(_fieldIndex); }
	bool TryGetS32(const char* _pFieldName, _s32& _val, _s32 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetS32(int _fieldIndex, _s32& _val, _s32 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _u32 (unsigned int)
	_u32 GetU32(const char* _pFieldName) { return GetNumber<_u32>(_pFieldName); }
	_u32 GetU32(int _fieldIndex) { return GetNumber<_u32>(_fieldIndex); }
	bool TryGetU32(const char* _pFieldName, _u32& _val, _u32 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetU32(int _fieldIndex, _u32& _val, _u32 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _s64 (long long)
	_s64 GetS64(const char* _pFieldName) { return GetNumber<_s64>(_pFieldName); }
	_s64 GetS64(int _fieldIndex) { return GetNumber<_s64>(_fieldIndex); }
	bool TryGetS64(const char* _pFieldName, _s64& _val, _s64 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetS64(int _fieldIndex, _s64& _val, _s64 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _u64 (unsigned long long)
	_u64 GetU64(const char* _pFieldName) { return GetNumber<_u64>(_pFieldName); }
	_u64 GetU64(int _fieldIndex) { return GetNumber<_u64>(_fieldIndex); }
	bool TryGetU64(const char* _pFieldName, _u64& _val, _u64 _defaultValue = 0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetU64(int _fieldIndex, _u64& _val, _u64 _defaultValue = 0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _f32 (float)
	_f32 GetFloat(const char* _pFieldName) { return GetNumber<_f32>(_pFieldName); }
	_f32 GetFloat(int _fieldIndex) { return GetNumber<_f32>(_fieldIndex); }
	bool TryGetFloat(const char* _pFieldName, _f32& _val, _f32 _defaultValue = 0.0f) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetFloat(int _fieldIndex, _f32& _val, _f32 _defaultValue = 0.0f) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// _f64 (double)
	_f64 GetDouble(const char* _pFieldName) { return GetNumber<_f64>(_pFieldName); }
	_f64 GetDouble(int _fieldIndex) { return GetNumber<_f64>(_fieldIndex); }
	bool TryGetDouble(const char* _pFieldName, _f64& _val, _f64 _defaultValue = 0.0) { return TryGetNumber(_pFieldName, _val, _defaultValue); }
	bool TryGetDouble(int _fieldIndex, _f64& _val, _f64 _defaultValue = 0.0) { return TryGetNumber(_fieldIndex, _val, _defaultValue); }

	// INSERT 전용 메서드 (기본 구현: 무동작)
	virtual _u64 GetInsertId() const { return 0; }

protected:
	int			  id_ = 0;

	IConnection*  pConn_              = nullptr;
	StatementType stmtType_           = StatementType::None;
	jc::String    ptmt_;
	int           errorCode_          = 0;
	jc::String	  errorMsg_			  = jc::String(0);
	_u32          rowCount_           = 0;

	friend class MysqlDatabase;
	friend class SqlServerDatabase;
};

// ===========================================================================================
// 공통 타입 별칭
// ===========================================================================================

using IQueryPtr = jc::SharedPtr<IQuery>;

NS_END
