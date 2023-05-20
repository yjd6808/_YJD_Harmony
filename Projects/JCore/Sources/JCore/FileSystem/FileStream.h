/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:18:00 AM
 * =====================
 *
 */


#pragma once


#include <JCore/Stream.h>

#include <JCore/FileSystem/FileAccess.h>
#include <JCore/FileSystem/FileMode.h>

NS_JC_BEGIN

template <typename T>
class SharedPtr;
class FileStream : public Stream
{
public:
	FileStream(const String& path, FileAccess access, FileMode mode);
	~FileStream() override;

	FileMode GetMode() { return m_eMode;  }
	FileAccess GetAccess() { return m_eAccess; }

	int Read(JCORE_OUT Byte* bytes, int offset, int len) override;
	void Write(const Byte* bytes, int offset, int len) override;
	void Seek(int offset, Origin origin = Origin::eBegin) override;
	bool Flush() override;
	void Close() override;
	bool IsClosed() override;
protected:
	FileAccess m_eAccess;
	FileMode m_eMode;
	IoHandle m_hHandle;
};

using FileStreamPtr = SharedPtr<FileStream>;

NS_JC_END