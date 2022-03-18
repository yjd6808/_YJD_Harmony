/*
	작성자 : 윤정도
	뮤텍스 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Mutex.h>

using namespace JCore;
using namespace std;


#if TEST_MutexTest == ON

int number;
CriticalSectionMutex csMtx;
EventMutex evMtx;

TEST(MutexTest, CriticalSectionMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 200000; i++) {
			csMtx.Lock();
			number++;
			number++;
			number++;
			csMtx.Unlock();
		}
	}};

	std::thread decreaser{ []() {
		for (int i = 0; i < 200000; i++) {
			csMtx.Lock();
			number--;
			number--;
			number--;
			csMtx.Unlock();
		}
	} };
	
	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}

TEST(MutexTest, EventMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 20000; i++) {
			evMtx.Lock();
			number += 1;
			number += 1;
			number += 1;
			evMtx.Unlock();
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 20000; i++) {
			evMtx.Lock();
			number -= 1;
			number -= 1;
			number -= 1;
			evMtx.Unlock();
		}
	} };

	
	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}


#endif