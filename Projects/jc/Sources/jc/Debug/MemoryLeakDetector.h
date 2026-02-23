/*
 * 작성자: 윤정도
 * 생성일: 12/14/2022 11:47:01 AM
 * =====================
 *
 */


#pragma once

#include <crtdbg.h>

#include <jc/Type.h>
#include <jc/TypeCast.h>
#include <jc/Functional.h>


NS_JC_BEGIN

// CrtMemBlockHeader
struct MemHeader
{
	_CrtMemBlockHeader* block_header_next;
	_CrtMemBlockHeader* block_header_prev;
	char const* file_name;
	int line_number;

	int block_use;
	size_t data_size;

	long request_number;
	unsigned char gap[4];

	// unsigned char    _data[_data_size];
	// unsigned char    _another_gap[no_mans_land_size];
};


// 범위 메모리릭 체크
// @코드 획득 주소 : https://stackoverflow.com/questions/29174938/googletest-and-memory-leaks
class MemoryLeakDetector
{
public:
	void StartDetect();
	bool Detecting()
	{
		return m_bDetecting;
	}
	int StopDetect();

protected:
	_CrtMemState m_State{};
	bool m_bDetecting{};
};

class AutoMemoryLeakDetector : public MemoryLeakDetector
{
	using TAction = Action<_u32>;

public:
	AutoMemoryLeakDetector();
	AutoMemoryLeakDetector(const TAction& _action);
	AutoMemoryLeakDetector(TAction&& _action);
	~AutoMemoryLeakDetector();

private:
	TAction m_Callback;
};

NS_END
