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
int main(int _argc, char** _pArgv)
{
    char arg0Default[] = "benchmark";
    char* pArgsDefault = arg0Default;
    if (!_pArgv)
    {
        _argc = 1;
        _pArgv = &pArgsDefault;
    }

    ::benchmark::Initialize(&_argc, _pArgv);

    if (::benchmark::ReportUnrecognizedArguments(_argc, _pArgv))
        return 1;

    Winsock::Initialize(2, 2);
    InitializeJCore();

    DefaultParserType = PacketParser::Command;
    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();

    FinalizeJCore();
    Winsock::Finalize();
    return 0;
}
