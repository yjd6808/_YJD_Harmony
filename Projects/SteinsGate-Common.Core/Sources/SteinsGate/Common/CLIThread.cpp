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
: arguments_(0)
{
}

CLIThread::Input::Input(SGVector<SGString>&& _arguments)
: arguments_(Move(_arguments))
{
}

CLIThread::Input::Input(Input&& _other) noexcept
{
	this->operator=(Move(_other));
}

CLIThread::Input::~Input()
{
}

CLIThread::Input& CLIThread::Input::operator=(Input&& _other) noexcept
{
	this->arguments_ = Move(_other.arguments_);
	return *this;
}

CLIThread::CLIThread()
: isRunning_(false)
, hasInput_(false)
, pListener_(nullptr)
, maxInputEventCount_(4)
{
}

CLIThread::~CLIThread()
{
	JCORE_DELETE_SAFE(pListener_);
}

void CLIThread::SetListener(ICLIListener* _pListener)
{
	JCORE_LOCK_GUARD(lock_);
	pListener_ = _pListener;
}

// std::cin.get을 강제로 
bool CLIThread::PreStart()
{
	isRunning_ = true;
	return true;
}

bool CLIThread::PreStop()
{
	isRunning_ = false;
	return true;
}

void CLIThread::WorkerThread()
{
	while (isRunning_)
	{
		String inputLine = Console::ReadLine();
		JCORE_LOCK_GUARD(lock_);

		Input input = { inputLine.Split(" ") };

		if (input.arguments_.Size() == 0 || input.arguments_[0].Length() == 0)
		{
			continue;
		}

		inputQueue_.Enqueue(Move(input));
		hasInput_ = true;
	}
}

void CLIThread::ProcessInputs()
{
	static Vector<Input> processingInputs;

	if (!hasInput_)
	{
		return;
	}

	processingInputs.Clear();

	{
		JCORE_LOCK_GUARD(lock_);
		while (!inputQueue_.IsEmpty())
		{
			processingInputs.PushBack(Move(inputQueue_.Front()));
			inputQueue_.Dequeue();
		}
		hasInput_ = false;
	}

	for (int i = 0; i < processingInputs.Size(); ++i)
	{
		Input& input = processingInputs[i];

		if (pListener_)
		{
			pListener_->OnInputProcessing(input.arguments_.Size(), &input.arguments_[0]);
		}
	}
}

void CLIThread::SendInterrupt()
{
	CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
	const bool hasConsole = ::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE), &consoleScreenBufferInfo);

	if (!hasConsole)
	{
		_LogWarn_("콘솔 정보를 찾지 못했습니다.");
		return;
	}

	isRunning_ = false;

	DWORD tmpWritten;
	INPUT_RECORD inputRecords[2];
	inputRecords[0].EventType = KEY_EVENT;
	inputRecords[0].Event.KeyEvent.bKeyDown = TRUE;
	inputRecords[0].Event.KeyEvent.dwControlKeyState = 0;
	inputRecords[0].Event.KeyEvent.uChar.UnicodeChar = VK_RETURN;
	inputRecords[0].Event.KeyEvent.wRepeatCount = 1;
	inputRecords[0].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
	inputRecords[0].Event.KeyEvent.wVirtualScanCode = ::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC);
	inputRecords[1] = inputRecords[0];
	inputRecords[1].Event.KeyEvent.bKeyDown = FALSE;
	::WriteConsoleInput(::GetStdHandle(STD_INPUT_HANDLE), inputRecords, 2, &tmpWritten);
}
