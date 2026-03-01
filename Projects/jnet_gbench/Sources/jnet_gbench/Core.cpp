/*
 * 작성자: 윤정도
 * 생성일: 01/02/2024 10:13:00 AM
 * =====================
 * PCH 파일
 */

#include <jnet_gbench/Core.h>

#include <jnet/Winsock.h>
#include <jnet/Packet/PacketParser.h>

USING_NS_JNET;

//////////////////////////////////////////////////////////////////////////////////////////
int main(int _argc, char** _argv)
{
    char arg0Default[] = "benchmark";
    char* pArgsDefault = arg0Default;
    if (!_argv)
    {
        _argc = 1;
		_argv = &pArgsDefault;
    }

    ::benchmark::Initialize(&_argc, _argv);

    if (::benchmark::ReportUnrecognizedArguments(_argc, _argv))
        return 1;

    Winsock::Initialize(2, 2);
    InitializeJCore(_argc, _argv);
	InitializeJNet(_argc, _argv);

    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

	FinalizeJNet();
    FinalizeJCore();
    Winsock::Finalize();
    return 0;
}
