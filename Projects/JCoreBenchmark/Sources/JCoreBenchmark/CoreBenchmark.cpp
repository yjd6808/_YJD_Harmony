#include <JCoreBenchmark/CoreBenchmark.h>

int main(int argc, char** argv) {
    Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);

    char arg0_default[] = "benchmark";                                 
    char* args_default = arg0_default;                                 
    if (!argv) {
        argc = 1;                                                         
        argv = &args_default;                                             
    }                                                                  
    ::benchmark::Initialize(&argc, argv);                          
    if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
        ::benchmark::RunSpecifiedBenchmarks();                         
        ::benchmark::Shutdown();                                       
        return 0;                                                      
}