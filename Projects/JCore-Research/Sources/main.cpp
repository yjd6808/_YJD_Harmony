#include "header.h"

int main() {
	new char;
	InitializeDefaultLogger();
	Vector<int> vFriendPrimaryKeyList(0);	// null

	{
		vFriendPrimaryKeyList.Resize(20);
	}

	for (int i =0; i < 40; ++i) {
		vFriendPrimaryKeyList.PushBack(i);
	}

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
