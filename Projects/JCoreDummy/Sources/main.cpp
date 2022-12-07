#include "header.h"


using namespace JCore;
using namespace std;

template <typename T>
struct Adder
{
    T value = 0;
    Adder(const T& val) : value(val) {}

    void Print()
    {
        std::cout << hex;
        std::cout << __FUNCSIG__ << "\n";

        for (int i = 0; i < 0x300; i++) {
            Interlocked64Api<T>::Add(&value, 1);
            printf("%lld\n", value);
        }

        for (int i = 0; i < 0x300; i++) {
            Interlocked64Api<T>::Add(&value, -1);
            printf("%lld\n", value);
        }

        std::cout << dec;

        NormalConsole::WriteLine("======================");
    }
};

int main() {
	AutoMemoryLeakDetector detector;
    NormalConsole::Init();
    NormalConsole::SetOutputCodePage(CodePage::UTF8);
    
    vector<thread> threads;
    //for (int i = 0; i < 4; i++)
    //    threads.emplace_back([] {
    //        for (int i = 0; i < 1'100'000; i++) {
    //            Interlocked32Api<Int32>::Add(&k, 1);
    //        }
    //    });

    //for (int i = 0; i < 4; i++)
    //    threads.emplace_back([] {
    //        for (int i = 0; i < 1'100'000; i++) {
    //            Interlocked32Api<Int32>::Add(&k, -1);
    //        }
    //    });


    Atomic<int*> s;

    Adder<Int64> k { MaxInt64_v - 0x100 };
    k.Print();
   /* for (int i = 0; i < 8; i++ )
        threads[i].join();*/
    //NormalConsole::WriteLine("k: %d(%d)", k, k == result);
    this_thread::sleep_for(10s);
	return 0;
} 