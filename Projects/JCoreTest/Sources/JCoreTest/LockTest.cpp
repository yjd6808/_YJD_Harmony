/*
	작성자 : 윤정도
	뮤텍스 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Lock.h>

using namespace JCore;
using namespace std;


#if TEST_LockTest == ON

int number;
CriticalSectionMutex csMtx;
EventMutex evMtx;
SpinLock splock;
Semaphore<1> sm1;
Semaphore<2> sm2;
Semaphore<3> sm3;
Semaphore<4> sm4;

TEST(LockTest, CriticalSectionMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 20000; i++) {
			csMtx.Lock();
			number++;
			number++;
			number++;
			csMtx.Unlock();
		}
	}};

	std::thread decreaser{ []() {
		for (int i = 0; i < 20000; i++) {
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

TEST(LockTest, EventMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 2000; i++) {
			evMtx.Lock();
			number += 1;
			number += 1;
			number += 1;
			evMtx.Unlock();
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 2000; i++) {
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


TEST(LockTest, SpinLock) {
	number = 0;
	std::thread increaser{ []() {
		for (int i = 0; i < 200000; i++) {
			splock.Lock();
			number += 1;
			number += 1;
			number += 1;
			splock.Unlock();
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 200000; i++) {
			splock.Lock();
			number -= 1;
			number -= 1;
			number -= 1;
			splock.Unlock();
		}
	} };

	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}

TEST(LockTest, Semaphore_1) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 200000; i++) {
			sm1.Lock();
			number += 1;
			number += 1;
			number += 1;
			sm1.Unlock();
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 200000; i++) {
			sm1.Lock();
			number -= 1;
			number -= 1;
			number -= 1;
			sm1.Unlock();
		}
	} };


	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}

#endif