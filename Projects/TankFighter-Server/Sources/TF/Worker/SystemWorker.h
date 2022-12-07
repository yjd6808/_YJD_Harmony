/*
	작성자 : 윤정도
	
	그냥 단순히 서브 작업용으로 사용하는 워커 쓰레드
*/

#pragma once

#include <JNetwork/Worker.h>

class SystemWorker
{
public:
	static SystemWorker* GetInstance();
	WinHandle CreateExitHandle();
	void Run();
	void WorkerThread() const;

	~SystemWorker();
protected:
	SystemWorker();
private:
	std::thread m_Thread;
	WinHandle m_hExitHandle = INVALID_HANDLE_VALUE;

	inline static SystemWorker* ms_pInstance;
};



