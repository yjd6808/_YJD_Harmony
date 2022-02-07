#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>
#include <atomic>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/Deletor.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/SmartPointer.h>
#include <JCore/Exception.h>
#include <JCore/Container/Arrays.h>

using namespace JCore;
using namespace std;




struct Model
{
	Model() {
		cout << "model default cons\n";
	}
	Model(int da) {
		a = da;
		cout << "model cons\n";
	}
	~Model() {
		cout << "model dest\n";
	}

	int a = 3;
	int b = 3;
};


struct LegendBlock
{
	int a;
	int b;

	LegendBlock() {
		cout << "cons\n";
	}
	~LegendBlock() {
		cout << "dest\n";
	}
};



/*
void* operator new(std::size_t blockSize) {
	std::cout << "allocate bytes: " << blockSize << std::endl;
	return malloc(blockSize);
}

void* operator new[](std::size_t blockSize) {
	std::cout << "[] allocate: " << blockSize << std::endl;
	return malloc(blockSize);
}

void operator delete(void* block) throw() {
	int* blockSize = static_cast<int*>(block);
	blockSize = blockSize - sizeof(int);
	std::cout << "deallocate bytes: " << *blockSize << std::endl;
	free(block);
}

void operator delete[](void* block) throw() {
	int* blockSize = static_cast<int*>(block);
	blockSize = blockSize - sizeof(int);
	std::cout << "[] deallocate bytes: " << *blockSize << std::endl;
	free(block);
}
*/




int main() {

	SharedPointer<Model[12]> a = MakeShared<Model[12]>(1);	// 배열 길이 12,		값 1로 초기화
	SharedPointer<Model[12]> b = MakeShared<Model[12]>();	// 배열 길이 12,		디폴트 초기화

	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

