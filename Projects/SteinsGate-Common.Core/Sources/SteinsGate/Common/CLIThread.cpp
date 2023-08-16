/*
 * 작성자 : 윤정도
 */

#include "Core.h"
#include "CommonCoreHeader.h"
#include "CLIThread.h"

#include <csignal>

USING_NS_JC;
USING_NS_JNET;

CLIThread::Input::Input()
	: Arguments(0)
{}

CLIThread::Input::Input(SGVector<SGString>&& args)
	: Arguments(Move(args))
{}

CLIThread::Input::Input(Input&& other) noexcept {
	this->operator=(Move(other));
}

CLIThread::Input::~Input() {
	// Arrays::ForEachDelete(Arguments, ArgumentCount);
	// JCORE_DELETE_ARRAY_SAFE(Arguments);
}

CLIThread::Input& CLIThread::Input::operator=(Input&& other) noexcept {
	// this->ArgumentCount = other.ArgumentCount;
	// this->Arguments = other.Arguments;

	// other.ArgumentCount = 0;
	// other.Arguments = nullptr;

	this->Arguments = Move(other.Arguments);
	return *this;
}

CLIThread::CLIThread()
	: m_bRunning(false)
	, m_bHasInput(false)
	, m_pListener(nullptr)
	, m_iMaxInputEventCount(4)
{}

CLIThread::~CLIThread() {
	JCORE_DELETE_SAFE(m_pListener);
}

void CLIThread::SetListener(ICLIListener* listener) {
	JCORE_LOCK_GUARD(m_Lock);
	m_pListener = listener;
}

// std::cin.get을 강제로 
bool CLIThread::PreStart() {
	m_bRunning = true;
	return true;
}

bool CLIThread::PreStop() {
	m_bRunning = false;
	return true;
}


void CLIThread::WorkerThread() {
	while (m_bRunning) {
		String read = Console::ReadLine();
		JCORE_LOCK_GUARD(m_Lock);

		Input input = { read.Split(" ") };

		if (input.Arguments.Size() == 0 || input.Arguments[0].Length() == 0) {
			continue;
		}

		m_qInputs.Enqueue(Move(input));
		m_bHasInput = true;
	}
}


void CLIThread::ProcessInputs() {
	static Vector<Input> s_vProcessingInputs;

	if (!m_bHasInput) {
		return;
	}

	s_vProcessingInputs.Clear();

	{
		JCORE_LOCK_GUARD(m_Lock);
		while (!m_qInputs.IsEmpty()) {
			s_vProcessingInputs.PushBack(Move(m_qInputs.Front()));
			m_qInputs.Dequeue();
		}
		m_bHasInput = false;
	}

	for (int i = 0; i < s_vProcessingInputs.Size(); ++i) {
		Input& input = s_vProcessingInputs[i];

		if (m_pListener) {
			m_pListener->OnInputProcessing(input.Arguments.Size(), &input.Arguments[0]);
		}
	}
}

void CLIThread::SendInterrupt() {

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	const bool bHasConsole = ::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

	if (!bHasConsole) {
		_LogWarn_("콘솔 정보를 찾지 못했습니다.");
		return;
	}

	m_bRunning = false;

	DWORD dwTmp;
	INPUT_RECORD ir[2];
	ir[0].EventType = KEY_EVENT;
	ir[0].Event.KeyEvent.bKeyDown = TRUE;
	ir[0].Event.KeyEvent.dwControlKeyState = 0;
	ir[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
	ir[0].Event.KeyEvent.wRepeatCount = 1;
	ir[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
	ir[0].Event.KeyEvent.wVirtualScanCode = ::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
	ir[1] = ir[0];
	ir[1].Event.KeyEvent.bKeyDown = FALSE;
	::WriteConsoleInput(::GetStdHandle(STD_INPUT_HANDLE), ir, 2, &dwTmp);
	
}
