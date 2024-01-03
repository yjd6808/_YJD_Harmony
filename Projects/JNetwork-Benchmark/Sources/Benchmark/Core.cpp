/*
 * 작성자: 윤정도
 * 생성일: 01/02/2024 10:13:00 AM
 * =====================
 * PCH 파일
 */

#include <Benchmark/Core.h>

#include <JNetwork/Winsock.h>
#include <JNetwork/Packet/PacketParser.h>

USING_NS_JNET;

int main(int argc, char** argv) {
    char arg0_default[] = "benchmark";                                 
    char* args_default = arg0_default;                                 
    if (!argv) {
        argc = 1;                                                         
        argv = &args_default;                                             
    }

    ::benchmark::Initialize(&argc, argv);

    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) 
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