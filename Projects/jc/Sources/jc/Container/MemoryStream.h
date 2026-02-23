
/*
 * 작성자: 윤정도
 * 생성일: 5/19/2024 9:17:24 AM
 * =====================
 * 용도는 암거나 마구잡이로 넣음 (상속하기 귀찮아서...)
 * 1. 프로세스 할당 O 멤스트림
 * 1. 프로세스 할당 X 멤스트림
 * 1. 할당 O 멤스트림
 * 1. 할당 X 멤스트림
 * 1. 네트워크 메시지용 멤스트림
 */

/*
#pragma once

#include "Span.h"

class Process;
class RIC_API MemoryStream
{
public:
	enum Origin
	{
		moBegin,
		moCurrent,
		moEnd
	};

	MemoryStream();
	MemoryStream(_byte* _pBytes, _u32 _len, bool _owner = true);
	MemoryStream(_ptr _pAlloc, _u32 _len, Process _memOwner, bool _owner = true);
	MemoryStream(SpanByte _span, bool _owner = true);
	MemoryStream(const MemoryStream& _other);
	MemoryStream(MemoryStream&& _other) noexcept;
	~MemoryStream();

	MemoryStream&	operator=(const MemoryStream& _other);
	MemoryStream&	operator=(MemoryStream&& _other) noexcept;
	_byte&			operator[](_u32 _offset);

	void		Free();
	void		ExpandAllocateIfNeeded(_u32 _minimumSize = 0);
	void		SetData(_byte* _pBytes, _u32 _len);
	void		SetOwner(bool _owner) { owner_ = _owner; }
	void		SetWriteOffset(_u32 _offset, Origin _origin = moBegin) { SetWriteOffset((int)_offset, _origin); }
	void		SetWriteOffset(int _offset, Origin _origin = moBegin);
	void		SetReadOffset(int _offset, Origin _origin = moBegin);
	void		Reset();
	void		PopReads();

	_byte		GetAt(_u32 _offset) const;
	int			GetWriteOffset() const { return writeOffset_; }
	int			GetReadOffset() const { return readOffset_; }
	int			GetReadaleSize() const { return writeOffset_ - readOffset_; }
	int			GetWriteableSize() const { return (int)len_ - writeOffset_; }
	_u32		GetNetworkMessageId(bool _includeStreamLen);
	std::shared_ptr<Process> GetMemOwner() const { return process_; }
	bool		IsNull() const { return pBytes_ == nullptr; }
	bool		IsOwner() const { return owner_; }
	bool		IsTargetLocal() const { return process_ == nullptr; }
	bool		IsTargetProcess() const { return process_ != nullptr; }
	bool		CanReadNetworkMessage();

	_byte*		Data() const { return pBytes_; }
	_byte*		DataFromReadOffset() const { return pBytes_ + readOffset_; }
	_byte*		DataFromWriteOffset() const { return pBytes_ + writeOffset_; }
	_ptr		PtrData() const { return (_ptr)pBytes_; }
	_ptr		PtrDataFromReadOffset() const { return (_ptr)pBytes_ + readOffset_; }
	_ptr		PtrDataFromWriteOffset() const { return (_ptr)pBytes_ + writeOffset_; }

	_u32		Length() const { return len_; }
	SpanByte	Span() const { return { pBytes_, len_ }; }

	int			PeekS32();
	_u32		PeekU32();
	int&		PeekS32Ref();	// CMessage에는 없어서 거슬렸음;
	_u32&		PeekU32Ref();
	void		Peek(OUT _byte* _pBytes, _u32 _offset, _u32 _len);
	void		PeekRef(OUT _byte** _ppBytes, _u32 _offset, _u32 _len);

	_string		ReadString();
	void		ReadStringArray(OUT _char* _pBuf, int _capacity);
	_byte		ReadByte();
	void		ReadBytes(OUT byte* _pBuf, int _count);
	bool		ReadBool();
	_char		ReadChar();
	wchar_t		ReadWChar();
	char		ReadMChar();
	_ptr		ReadPtr();
	float		ReadFloat();
	_s8			ReadS8();
	_s16		ReadS16();
	int			ReadS32();
	_s64		ReadS64();
	_u8			ReadU8();
	_u16		ReadU16();
	_u32		ReadU32(bool _pop = false);
	_u64		ReadU64();
	void		Read(OUT _byte* _pBytes, _u32 _offset, _u32 _len);
	void		ReadStream(OUT MemoryStream& _stream, int _len);
	bool		ReadNetworkMessage(OUT MemoryStream& _stream, OUT int& _readBytes, bool _readOnly);

	// 멤스트림만 다르게 처리하기 위함
	template <typename T>	struct is_write_specilized_type	: std::false_type {};
	template <>				struct is_write_specilized_type<MemoryStream> : std::true_type {};
	template <typename T, std::enable_if_t<!is_write_specilized_type<T>::value, int> = 0>
	void		Write(const T& _value) { WriteInternal((_byte*)&_value, sizeof(T)); }
	void		Write(const MemoryStream& _value);

	void		WriteString(const _string& _value);
	void		WriteString(const _char* _value);
	void		WriteString(const _char* _value, int _len);
	void		WriteByte(_byte _value);
	void		WriteBool(bool _value);
	void		WriteChar(_char _value);
	void		WriteU32(_u32 _value);
	void		WriteU32(_u32 _value, OUT _u32** _ptr);
	void		WriteS32(int _value);
	void		WriteS64(_s64 _value);
	void		WriteU64(_u64 _value);
	void		WritePtr(_ptr _value);
	void		Write(const _byte* _pBytes, _u32 _len);
	void		WriteJmpRip(_ptr _instructionAddr, _ptr _dstAddr);
	void		WriteJmpR15();
	void		WriteCallRip(_ptr _instructionAddr, _ptr _dstAddr);
	bool		WriteInternal(const _byte* _pBytes, _u32 _len);
	void		PlugNetworkMessage(_u32 _cmdId);
	_string		DumpHexText(_u32 _byteGroupSize = 1, _u32 _byteGroupCountInSingleLine = 8, _string _join = _string(), bool _0x = true);

	static std::shared_ptr<MemoryStream> CreateNetworkMessage();
	static MemoryStream Null;
private:
	int readOffset_;
	int writeOffset_;
	_u32 len_;
	_byte* pBytes_;
	bool owner_;
	std::shared_ptr<Process> process_;
};

using MemoryStreamPtr = std::shared_ptr<MemoryStream>;
using RecvPendingMessageQueue = std::queue<MemoryStreamPtr>;
using RecvPendingMessageQueuePtr = std::shared_ptr<std::queue<MemoryStreamPtr>>;
*/