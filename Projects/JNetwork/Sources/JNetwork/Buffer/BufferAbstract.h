/*
 * 작성자 : 윤정도
 *
 *   ======================================================================
 *   ※※※※※※※※※※※※ □□□□□□□□□□□□□□□□□□□□□□□□□
 *   ======================================================================
 *   0               ↑                         ↑
 *                ReadPos                  WritePos
 *                   <--  ReadableBufferSize --> <--- RemainBufferSize --->
 *
 *   ( ※ ) : 이미 읽은 영역
 *   ( □ ) : 아직 읽지 않은 영역
 *   (   ) : 아직 축적된 데이터가 없음
 *  
 *
 */

#pragma once

#include <JCore/Memory.h>
#include <JCore/Math.h>
#include <JCore/Primitives/SmartPtr.h>

#include <WinSock2.h>
#include <JNetwork/Namespace.h>


NS_JNET_BEGIN

class BufferAbstract
{
public:
	BufferAbstract()
		: buffer_(nullptr)
		, bufferSize_(0)
		, readPos_(0)
		, writePos_(0)
	{
	}

	BufferAbstract(char* _pBuffer, int _bufferSize)
		: buffer_(_pBuffer)
		, bufferSize_(_bufferSize)
		, readPos_(0)
		, writePos_(0)
	{
	}

	virtual ~BufferAbstract() = default;

	template <typename T>
	bool Read(JCORE_OUT T& _data)
	{
		static_assert(JCore::IsNaturalType_v<T>, "... T must be natural type");

		if (!IsReadable(sizeof(T)))
		{
			return false;
		}

		_data = *reinterpret_cast<T*>(buffer_ + readPos_);
		readPos_ += sizeof(sizeof(T));
		return true;
	}

	template <typename T>
	bool Write(const T& _data)
	{
		if (!IsWriteable(sizeof(T)))
		{
			return false;
		}

		JCore::Memory::CopyUnsafe(buffer_ + writePos_, &_data, sizeof(T));
		writePos_ += sizeof(T);
		return true;
	}

	JCore::String ReadString(int _len)
	{
		if (!IsReadable(_len))
		{
			return {};
		}

		JCore::String s;
		for (int i = 0; i < _len; ++i)
		{
			s += buffer_[readPos_ + i];
		}
		readPos_ += _len;
		return s;
	}

	bool WriteString(const JCore::String& _s)
	{
		const int length = _s.Length();
		if (!IsWriteable(length))
		{
			return false;
		}

		JCore::Memory::CopyUnsafe(buffer_ + writePos_, _s.Source(), length);
		writePos_ += length;
		return true;
	}

	template <typename T>
	T Peek()
	{
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(buffer_ + readPos_);
	}

	bool MoveReadPos(int _size)
	{
		if (!IsReadable(_size))
		{
			return false;
		}

		readPos_ += _size;
		return true;
	}

	bool MoveWritePos(int _size)
	{
		if (!IsWriteable(_size))
		{
			return false;
		}

		writePos_ += _size;
		return true;
	}

	// m_iReadPos와 m_iWritePos 사이의 데이터를 len만큼 앞으로 당김
	void Pop(int _len, bool _forceMove = false)
	{
		if (!_forceMove && readPos_ - _len < 0)
		{
			DebugAssertMsg(false, "len > m_iReadPos");
			return;
		}

		int beforeReadPos = readPos_;

		readPos_ -= _len;
		writePos_ -= _len;

		if (_forceMove && readPos_ < 0)
		{
			const int overflow = JCore::Math::Abs(readPos_);

			writePos_ += overflow;
			readPos_ = 0;
		}

		JCore::Memory::CopyUnsafe(
			buffer_ + readPos_,
			buffer_ + beforeReadPos,
			writePos_ - readPos_);
	}

	void PopReads()
	{
		if (readPos_ == writePos_)
		{
			// 만약 수신한 데이터를 모두 읽었으면 포지션을 그냥 0으로 옮긴다.
			ResetPosition();
		}
		else
		{
			// 읽은 위치만큼은 이제 다시 쓰일일이 없으므로 버퍼를 앞으로 당긴다. 
			// WritePos 이후로 데이터를 쌓을 수 있도록하기 위해
			Pop(readPos_, true);
		}
	}

	WSABUF GetRemainBuffer()
	{
		return { (Int32U)bufferSize_ - writePos_, buffer_ + writePos_ };
	}

	int GetRemainBufferSize() const
	{
		return bufferSize_ - writePos_;
	}

	int GetReadableBufferSize() const
	{
		return writePos_ - readPos_;
	}

	char* GetReadableSource()
	{
		return buffer_ + readPos_;
	}

	char* GetWriteableSource()
	{
		return buffer_ + readPos_;
	}

	void SetReadPos(int _pos)
	{
		readPos_ = _pos;
	}

	void SetWritePos(int _pos)
	{
		writePos_ = _pos;
	}

	int GetReadPos() const
	{
		return readPos_;
	}

	int GetWritePos() const
	{
		return writePos_;
	}

	int GetBufferCapacity() const
	{
		return bufferSize_;
	}

	void ResetPosition()
	{
		writePos_ = 0;
		readPos_ = 0;
	}

	char* Source()
	{
		return buffer_;
	}

private:
	bool IsReadable(int _size) const
	{
		if (readPos_ + _size > writePos_)
		{
			return false;
		}

		return true;
	}

	bool IsWriteable(int _size) const
	{
		if (writePos_ + _size > bufferSize_)
		{
			return false;
		}

		return true;
	}

protected:
	char* buffer_;
	int readPos_;
	int writePos_;
	int bufferSize_;
};


using BufferAbstractPtr = JCore::SharedPtr<BufferAbstract>;

NS_JNET_END


