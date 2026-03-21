#include "Core.h"

int main(int _argc, char** _argv) 
{
    Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);

    char arg0Default[] = "benchmark";
    char* argsDefault = arg0Default;
    if (!_argv) 
	{
        _argc = 1;
        _argv = &argsDefault;
    }

    ::benchmark::Initialize(&_argc, _argv);
    if (::benchmark::ReportUnrecognizedArguments(_argc, _argv)) 
		return 1;

    ::benchmark::RunSpecifiedBenchmarks();
    ::benchmark::Shutdown();
    return 0;
}
