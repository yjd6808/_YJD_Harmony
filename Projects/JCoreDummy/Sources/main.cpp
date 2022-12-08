#include <filesystem>

#include "header.h"


using namespace JCore;
using namespace std;

//template <typename T>
//struct Adder
//{
//    T value = 0;
//    Adder(const T& val) : value(val) {}
//
//    void Print()
//    {
//        std::cout << hex;
//        std::cout << __FUNCSIG__ << "\n";
//
//        for (int i = 0; i < 0x300; i++) {
//            Interlocked<T>::Add(&value, 1);
//            printf("%lld\n", value);
//        }
//
//        for (int i = 0; i < 0x300; i++) {
//            Interlocked<T>::Add(&value, -1);
//            printf("%lld\n", value);
//        }
//
//        std::cout << dec;
//
//        NormalConsole::WriteLine("======================");
//    }
//};


using Console = NormalConsole;

struct A
{
};

struct B : A {};

struct C{};


int main() {
    Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);

    B ff;

    Atomic<A*> k1 = new A();
    Atomic<B*> k2 = new B();
    Atomic<C*> k3;
    k1 = k2;
    Console::WriteLine("%p", k1.Load());
    Console::WriteLine("%p", k2.Load());

    std::atomic<A*> k4;
    std::atomic<B*> k5;
    std::atomic<C*> k6;

    k4 = k5;
    
	return 0;
}

