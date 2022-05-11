/*
	작성자 : 윤정도
	
	그냥 단순히 서브 작업용으로 사용하는 워커 쓰레드
*/

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <JNetwork/Worker.h>

class SystemWorker
{
public:
	static SystemWorker* GetInstance();
	HANDLE CreateExitHandle();
	void Run();
	void WorkerThread() const;

	~SystemWorker();
protected:
	SystemWorker();
private:
	std::thread m_Thread;
	HANDLE m_hExitHandle = INVALID_HANDLE_VALUE;

	inline static SystemWorker* ms_pInstance;
};



