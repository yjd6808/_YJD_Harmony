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
#include <jc/TypeTraits.h>

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
	virtual int          GetColIndex(const char* _pFieldName) { return -1; }
	virtual _u32         GetColCount() const { return 0; }
	virtual int			 GetColType(int _fieldIndex) { return -1; }

	virtual const char*  GetRawString(const char* _pFieldName)  { return nullptr; }
	virtual const char*  GetRawString(int _fieldIndex)          { return nullptr; }
	virtual jc::DateTime GetDateTime(const char* _pFieldName)   { return 0; }
	virtual jc::DateTime GetDateTime(int _fieldIndex)           { return 0; }
	
	virtual jc::String	 GetString(const char* _pFieldName) { return GetRawString(_pFieldName); }
	virtual jc::String	 GetString(int _fieldIndex) { return GetRawString(_fieldIndex); }

	// ===========================================================================================
	// 타입 읽기 메서드 (SELECT 전용)
	// ===========================================================================================
	virtual _s8 GetS8(int _fieldIdx)      { return 0; }
	virtual _u8 GetU8(int _fieldIdx)      { return 0; }
	virtual _s16 GetS16(int _fieldIdx)    { return 0; }
	virtual _u16 GetU16(int _fieldIdx)    { return 0; }
	virtual _s32 GetS32(int _fieldIdx)    { return 0; }
	virtual _u32 GetU32(int _fieldIdx)    { return 0; }
	virtual _s64 GetS64(int _fieldIdx)    { return 0; }
	virtual _u64 GetU64(int _fieldIdx)    { return 0; }
	virtual _f32 GetFloat(int _fieldIdx)  { return 0.0f; }
	virtual _f64 GetDouble(int _fieldIdx) { return 0.0; }
	virtual jc::Date GetDate(int _fieldIdx) { return {}; }
	virtual jc::Time GetTime(int _fieldIdx) { return {}; }

	template <typename T>
	T GetNumber(const char* _pFieldName)
	{
		int idx = GetColIndex(_pFieldName);
		if (idx == -1) return T{};
		return GetNumber<T>(idx);
	}

	template <typename T>
	T GetNumber(int _fieldIdx)
	{
		if constexpr (jc::IsSameType_v<T, _s8>)           return static_cast<T>(GetS8(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _u8>)      return static_cast<T>(GetU8(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _s16>)     return static_cast<T>(GetS16(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _u16>)     return static_cast<T>(GetU16(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _s32>)     return static_cast<T>(GetS32(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _u32>)     return static_cast<T>(GetU32(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _s64>)     return static_cast<T>(GetS64(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _u64>)     return static_cast<T>(GetU64(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _f32>)     return static_cast<T>(GetFloat(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _f64>)     return static_cast<T>(GetDouble(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, bool>)     return GetU8(_fieldIdx) != 0;
		else if constexpr (jc::IsSameType_v<T, _s32l>)    return static_cast<T>(GetS32(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _u32l>)    return static_cast<T>(GetU32(_fieldIdx));
		else if constexpr (jc::IsSameType_v<T, _s16c>)    return static_cast<T>(GetS16(_fieldIdx));
		else
		{
			static_assert(sizeof(T) == 0, "지원하지 않는 타입입니다.");
			return T{};
		}
	}

	virtual void		   ResetColReadOffset() { }
	virtual jc::StringView ReadRawString() { return jc::StringView{}; }
	virtual jc::String	   ReadString() { return jc::String(); }
	virtual _s8			   ReaS8() { return 0; }
	virtual _u8			   ReadU8() { return 0; }
	virtual _s16		   ReadS16() { return 0; }
	virtual _u16		   ReadU16() { return 0; }
	virtual _s32		   ReadS32() { return 0; }
	virtual _u32		   ReadU32() { return 0; }
	virtual _s64		   ReadS64() { return 0; }
	virtual _u64		   ReadU64() { return 0; }
	virtual _f32		   ReadFloat() { return 0.0f; }
	virtual _f64		   ReadDouble() { return 0.0; }
	virtual jc::DateTime   ReadDateTime() { return 0; }

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
