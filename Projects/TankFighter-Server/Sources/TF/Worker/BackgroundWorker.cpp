// ReSharper disable CppClangTidyCppcoreguidelinesProTypeStaticCastDowncast
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <TF/Worker/BackgroundWorker.h>

#include "TF/Util/Console.h"

using namespace JCore;

BackgroundWorker::BackgroundWorker() : Worker() {}
BackgroundWorker::~BackgroundWorker() = default;

void BackgroundWorker::Run(void* param) {
	// std::thread 멤버 함수로 실행하는 법
	// @참고 : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

	m_eState = State::Running;
	m_Thread = std::thread{ [this, param]() { WorkerThread(param); } };
}

void BackgroundWorker::JoinWait(HANDLE waitHandle) {
}

void BackgroundWorker::Join() {
}

void BackgroundWorker::Pause(HANDLE waitHandle) {
}

void BackgroundWorker::Resume() {
}


void BackgroundWorker::WorkerThread(void* param) {
	Console::WriteLine("BackgroundWorker 쓰레드가 실행되었습니다. (%d)", std::this_thread::get_id());

	for (;;) {
		
	}

THREAD_END:
	Console::WriteLine("BackgroundWorker 쓰레드가 종료되었습니다. (%d)", std::this_thread::get_id());
	m_eState = State::JoinWait;
}