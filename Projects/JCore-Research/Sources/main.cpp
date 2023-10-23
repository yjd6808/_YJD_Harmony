#include "header.h"


int main() {
	InitializeDefaultLogger();

	class Data : public AccessibleObjectPool<Data>
	{
	public:
		void OnPopped() override {
			A = 100;
			B = 200;
		}
		void OnPushed() override {}

		int A;
		int B;
	};


	Data::InitPool(1000, 10);	// 용량, 초기 생성된 객체 수 설정
	Data* pNewData = Data::Pop();
	const int iAccessId = pNewData->GetAccessId();
	Data* pAccessData = Data::GetByAccessId(iAccessId);
	if (pAccessData == pNewData) {
		Console::WriteLine("동일한 데이터입니다. A=%d, B=%d", pNewData->A, pNewData->B);
	}
	Data::Push(pNewData);
	Data::FreeAllObjects();		// 반환된 객체들 모두 메모리 정리

	Console::WriteLine("");

	FinalizeDefaultLogger();
	return Console::ReadKeyWhile("X키 입력시 종료", ConsoleKey::X) ? 0 : -1;
}
