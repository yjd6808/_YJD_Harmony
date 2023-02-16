#include "header.h"
#include <stacktrace>

#include "JCore/Natvis/NatvisByteOrder.h"


USING_NS_JC;

int main() {
	constexpr const char* a = CSI_GRAPHIC_RENDITION(1;2);

	Console::Init();
	Console::SetEnableVTMode(false);
	Console::WriteLine(Yellow, "fsefe");
	Console::SetEnableVTMode(false);
	Console::WriteLine(White, "fsefe");
	Console::SetEnableVTMode(true);
	Console::WriteLine(Yellow, "fsefe");
	Console::SetEnableVTMode(true);
	Console::WriteLine(White, "fsefe");
	return 0;
}

