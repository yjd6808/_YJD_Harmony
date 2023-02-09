/*
 * 작성자 : 윤정도
 *
 * 	 ======================================================================
 *	   ※※※※※※※※※※※※ □□□□□□□□□□□□□□□□□□□□□□□□□
 *	 ======================================================================
 *	   0               ↑                         ↑
 *	                ReadPos                  WritePos
 *				       <--  ReadableBufferSize --> <--- RemainBufferSize --->
 *
 *	   ( ※ ) : 이미 읽은 영역
 *	   ( □ ) : 아직 읽지 않은 영역
 *	   (   ) : 아직 축적된 데이터가 없음
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
	BufferAbstract(char* buffer, int bufferSize)
		: m_pBuffer(buffer)
		, m_iBufferSize(bufferSize)
		, m_iReadPos(0)
		, m_iWritePos(0)
	{}
	virtual ~BufferAbstract() = default;



	template <typename T>
	T Read() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");

		if (!IsReadable(sizeof(T))) {
			return nullptr;
		}

		T pRet = reinterpret_cast<T>(m_pBuffer + m_iReadPos);
		m_iReadPos += sizeof(JCore::RemovePointer_t<T>);
		return pRet;
	}

	template <typename T>
	bool Write(const T& data) {
		if (!IsWriteable(sizeof(T))) {
			return false;
		}

		JCore::Memory::CopyUnsafe(m_pBuffer + m_iWritePos, &data, sizeof(T));
		m_iWritePos += sizeof(T);
		return true;
	}

	template <typename T>
	T Peek() {
		static_assert(JCore::IsPointerType_v<T>, "... T must be pointer type");
		return reinterpret_cast<T>(m_pBuffer + m_iReadPos);
	}

	bool MoveReadPos(int size) {
		if (!IsReadable(size)) {
			return false;
		}

		m_iReadPos += size;
		return true;
	}

	bool MoveWritePos(int size) {
		if (!IsWriteable(size)) {
			return false;
		}

		m_iWritePos += size;
		return true;
	}

	// m_iReadPos와 m_iWritePos 사이의 데이터를 len만큼 앞으로 당김
	void Pop(int len, bool forceMove = false) {
		if (!forceMove && m_iReadPos - len < 0) {
			DebugAssertMsg(false, "len > m_iReadPos");
			return;
		}

		int iBeforeReadPos = m_iReadPos;

		m_iReadPos -= len;
		m_iWritePos -= len;

		if (forceMove && m_iReadPos < 0) {
			const int iOverFlow = JCore::Math::Abs(m_iReadPos);

			m_iWritePos += iOverFlow;
			m_iReadPos = 0;
		}

		JCore::Memory::CopyUnsafe(
			m_pBuffer + m_iReadPos,
			m_pBuffer + iBeforeReadPos,
			m_iWritePos - m_iReadPos);
	}


	WSABUF GetRemainBuffer() {
		return { (Int32U)m_iBufferSize - m_iWritePos, m_pBuffer + m_iWritePos };
	}

	int GetRemainBufferSize() const {
		return m_iBufferSize - m_iWritePos;
	}

	int GetReadableBufferSize() const {
		return m_iWritePos - m_iReadPos;
	}

	
	int GetReadPos() const { return m_iReadPos; }
	int GetWritePos() const { return m_iWritePos; }
	int GetBufferCapacity() { return m_iBufferSize; }
	

	void Clear() {
		m_iWritePos = 0;
		m_iReadPos = 0;
	}
private:
	bool IsReadable(int size) const {
		if (m_iReadPos + size > m_iWritePos) {
			return false;
		}
	
		return true;
	}

	bool IsWriteable(int size) const {
		if (m_iWritePos + size > m_iBufferSize) {
			return false;
		}

		return true;
	}
private:
	char* m_pBuffer;
	int m_iReadPos;
	int m_iWritePos;
	int m_iBufferSize;
};


using BufferAbstractPtr = JCore::SharedPtr<BufferAbstract>;

NS_JNET_END



