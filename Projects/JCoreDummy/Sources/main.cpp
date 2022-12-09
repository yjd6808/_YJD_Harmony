
#include "header.h"


using Console = NormalConsole;

struct A : std::enable_shared_from_this<A>
{
private:
    A();
public:
};

int main() {

    Console::Init();
    Console::SetOutputCodePage(CodePage::UTF8);

    // Thread::Create();

	return 0;
}

