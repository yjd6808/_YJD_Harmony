#include "header.h"

class Data : public AccessibleObjectPool<Data>
{
public:
	void OnPopped() override {}
	void OnPushed() override {}

	char a[300];
};

constexpr int ElementCount = 100;
constexpr int ThreadSize = 16;

Tuple<bool, Data*> maps[20000]{};
int pushCount[ThreadSize]{};

int main() {
	new char;
	InitializeDefaultLogger();
	Random::EngineInitialize();
	Data::InitPool(ElementCount, 10);
	

	for (int i = 0; i < ThreadSize; ++i) {
		for (int j = 0; j < ElementCount; ++j) {
			Data* data = Data::Pop();
			const int id = data->GetAccessId();
			DebugAssert(maps[id].item1 == false);		// 중복된 ID 발급 확인

			// 40%확률로 데이터를 도로 집어넣는다.
			if (Random::Chance(40.0f)) {
				Data::Push(data);
				++pushCount[i];
			} else {
				maps[id].item1 = true;
				maps[id].item2 = data;
			}
		}
	}

	for (int i = 0; i < ThreadSize; ++i) {
		const int start = ElementCount * i;
		const int end = ElementCount * (i + 1);

		for (int j = start; j < end; ++j) {
			if (maps[j].item1) {
				Data::Push(maps[j].item2);
				maps[j].item1 = false;
				maps[j].item2 = nullptr;
			}
		}
	}


	// 전부 다시 풀로 복귀
	Data::FreeAllObjects();

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
