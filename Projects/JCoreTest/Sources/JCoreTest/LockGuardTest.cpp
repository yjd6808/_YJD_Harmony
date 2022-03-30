/*
	작성자 : 윤정도
	뮤텍스 테스트
*/


#include <JCoreTest/CoreTest.h>

using namespace JCore;
using namespace std;


#if TEST_LockGuardTest == ON

namespace LockGuardTest {

int number;
CriticalSectionMutex csMtx;
EventMutex evMtx;

TEST(LockGuardTest, CriticalSectionMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 2000000; i++) {
			CriticalSectionLockGuard guard(csMtx);
			number++;
			number++;
			number++;
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 2000000; i++) {
			CriticalSectionLockGuard guard(csMtx);
			number--;
			number--;
			number--;
		}
	} };

	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}

TEST(LockGuardTest, EventMutex) {
	number = 0;

	std::thread increaser{ []() {
		for (int i = 0; i < 200000; i++) {
			EventLockGuard guard(evMtx);
			number += 1;
			number += 1;
			number += 1;
		}
	} };

	std::thread decreaser{ []() {
		for (int i = 0; i < 200000; i++) {
			EventLockGuard guard(evMtx);
			number -= 1;
			number -= 1;
			number -= 1;
		}
	} };


	increaser.join();
	decreaser.join();

	EXPECT_TRUE(number == 0);
}

} // namespace LockGuardTest

#endif