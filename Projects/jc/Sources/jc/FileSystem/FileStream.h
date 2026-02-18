/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 4:18:00 AM
 * =====================
 *
 */


#pragma once


#include <jc/Stream.h>

#include <jc/FileSystem/FileAccess.h>
#include <jc/FileSystem/FileMode.h>

NS_JC_BEGIN

template <typename T>
class SharedPtr;

class FileStream : public Stream
{
public:
	FileStream(const String& _path, FileAccess _access, FileMode _mode);
	~FileStream() override;

	FileMode GetMode()
	{
		return m_eMode;
	}

	FileAccess GetAccess()
	{
		return m_eAccess;
	}

	int Read(OUT Byte* _pBytes, int _offset, int _len) override;
	void Write(const Byte* _pBytes, int _offset, int _len) override;
	void Seek(int _offset, Origin _origin = Origin::eBegin) override;
	bool Flush() override;
	void Close() override;
	bool IsClosed() override;

protected:
	FileAccess m_eAccess;
	FileMode m_eMode;
	IoHandle m_hHandle;
};

using FileStreamPtr = SharedPtr<FileStream>;

NS_END
