// ReSharper disable CppClangTidyCppcoreguidelinesProTypeStaticCastDowncast
#include <JNetwork/Network.h>
#include <JNetwork/Winsock.h>
#include <TF/Worker/BackgroundWorker.h>

#include "TF/Util/Console.h"

using namespace JCore;

BackgroundWorker::BackgroundWorker() : Worker() {}
BackgroundWorker::~BackgroundWorker() = default;

void BackgroundWorker::Run(void* param) {
	// std::thread ��� �Լ��� �����ϴ� ��
	// @���� : https://stackoverflow.com/questions/10673585/start-thread-with-member-function

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
	Console::WriteLine("BackgroundWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());

	for (;;) {
		
	}

THREAD_END:
	Console::WriteLine("BackgroundWorker �����尡 ����Ǿ����ϴ�. (%d)", std::this_thread::get_id());
	m_eState = State::JoinWait;
}