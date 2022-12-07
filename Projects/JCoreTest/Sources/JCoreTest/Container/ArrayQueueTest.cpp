/*
	작성자 : 윤정도
	배열기반 스택 테스트
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Container/ArrayQueue.h>


using namespace std;

#if TEST_ArrayQueueTest == ON

TEST(ArrayQueueTest, Regular) {
	AutoMemoryLeakDetector detector;
	ArrayQueue<int> queue;
	const int count = 320;

	for (int i = 0; i < count; i++) {
		queue.Enqueue(i);
	}

	for (int i = 0; i < count; i++) {
		EXPECT_TRUE(queue.Front() == i);
		queue.Dequeue();
	}

	EXPECT_TRUE(queue.Size() == 0);

	for (int i = 0; i < count; i++) {
		queue.Enqueue(i);
	}

	queue.Clear();
	EXPECT_TRUE(queue.Size() == 0);
}

// 큐가 꽉찬 상황에서의 테스트
TEST(ArrayQueueTest, FullCaseTest) {

	// 큐가 꽉찬 상태에서 이터레이터가 무한 순회하는 상황
	ArrayQueue<int> q;
	const int z = q.Capacity();

	for (int i = 0; i < z; i++) {
		q.Enqueue(i);
	}

	auto it = q.Begin();
	int i = 0;
	while (it->HasNext()) {
		it->Next();
		i++;
	}

	it = q.End();

	while (it->HasPrevious()) {
		it->Previous();
		i--;
	}

	EXPECT_TRUE(i == 0);
}



// 배열 큐 역방향 반복자, 정방향 반복자, 삽입 삭제 등등 모두 테스트
TEST(ArrayQueueTest, TotalTest) {
	Random random;

	int testCount = 5;	// 5000
	while (testCount--) {

		int dataCount = random.GenerateInt(0, 100) + 1;
		int suffleCount = 20;
		AutoMemoryLeakDetector detector;
		ArrayQueue<int> queue;

		list<int> forward_list;		// 데이터 추적 - 정방향 용
									// 큐와 데이터 계속 똑같이 빼고 넣으면서 최종적으로 일치하는지 확인
		list<int> backward_list;

		
		for (int i = 0; i < dataCount; i++) {
			queue.Enqueue(i);
			forward_list.push_back(i);
			backward_list.push_front(i);
		}

		EXPECT_TRUE(queue.Size() == dataCount);
		int r = dataCount;	// 갯수 추척
		
		// 2개 넣고 2개 빼고 다시 32개를 넣어서 꽉채운다.

		while (suffleCount-- > 0) {
			int popSize = random.GenerateInt(1, queue.Size() + 1);
			r -= popSize;
			
			for (int i = 0; i < popSize; i++) {
				queue.Dequeue();
				forward_list.pop_front();
				backward_list.pop_back();
			}
			int queueSize1 = queue.Size();
			
			vector<int> v;	// 데이터 추적
							// 이번 셔플 단계에서 데이터가 일치하는지
			list<int> rv; // 반대방향으로 데이터 추적
			
			
			//PrintFormat("셔플/큐 데이터 수 : %d/%d\n", suffleCount, queueSize1);
			//빼고 남아있는 것들을 넣어줌
			Enumerator<int> it = queue.Begin();
			Enumerator<int> rit = queue.End();

			if (queueSize1 == 0) {
				int a = 40;
			}

			// 반복자 정방향
			while (it->HasNext()) {
				v.push_back(it->Next());
			}

			// 반복자 역방향
			while (rit->HasPrevious()) {
				rv.push_back(rit->Previous());
			}
			
			int enqueueSize = random.GenerateInt(1, 50);
			r += enqueueSize;

			// 큐에 1 ~ 남아있는 것들 수사이 랜덤 갯수만큼 다시 넣어줌
			for (int i = 0; i < enqueueSize; i++) {
				queue.Enqueue(i);

				v.push_back(i);
				forward_list.push_back(i);

				rv.push_front(i);
				backward_list.push_front(i);
			}

			int queueSize2 = queue.Size();

			it = queue.Begin();
			int k = 0;
			while (it->HasNext()) {
				EXPECT_TRUE(v[k++] == it->Next());
			}

			auto rvIt = rv.begin();
			rit = queue.End();
			k = 0;
			while (rit->HasPrevious()) {
				int value = rit->Previous();
				int vv = *rvIt;
				EXPECT_TRUE(vv == value);
				++rvIt;
			}

			EXPECT_TRUE(forward_list.size() == queue.Size());
			EXPECT_TRUE(backward_list.size() == queue.Size());
		}

		EXPECT_TRUE(queue.Size() == r);

		auto forward_listit = forward_list.begin();
		auto backward_listit = backward_list.begin();
		auto myqueueit = queue.Begin();

		while (myqueueit->HasNext()) {
			int lval = *forward_listit;
			EXPECT_TRUE(myqueueit->Next() == lval);
			++forward_listit;
		}

		myqueueit = queue.End();
		while (myqueueit->HasPrevious()) {
			int lval = *backward_listit;
			EXPECT_TRUE(myqueueit->Previous() == lval);
			++backward_listit;
		}
	}
}


// 생성자 테스트
TEST(ArrayQueueTest, ConstructorTest) {
	AutoMemoryLeakDetector detector;

	// 이니셜라이저 테스트
	ArrayQueue<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a.Front() == 1); a.Dequeue();
	EXPECT_TRUE(a.Front() == 2); a.Dequeue();

	// 복사 생성자 테스트
	ArrayQueue<int> b(a);
	for (int i = 0; i < 30; i++) {
		b.Enqueue(i);
	}

	ArrayQueue<int> c(a);
	EXPECT_TRUE(b.Front() == 3); b.Dequeue();
	EXPECT_TRUE(b.Front() == 0); b.Dequeue();
	EXPECT_TRUE(b.Size() == 29);
	EXPECT_TRUE(c.Size() == 1);

	// 이동 생성자 테스트
	ArrayQueue<int> d(Move(c));
	EXPECT_TRUE(c.Size() == 0);
	EXPECT_TRUE(d.Size() == 1);
	EXPECT_TRUE(d.Front() == 3);
}


template <typename T>
void SetQueuePos(ArrayQueue<T>& q, int pos) {
	q.Clear();

	for (int i = 0; i < pos; i++) {
		q.Enqueue(i);
	}
	while (!q.IsEmpty()) {
		q.Dequeue();
	}
}

// 연산자 테스트
TEST(ArrayQueueTest, OperatorTest) {
	AutoMemoryLeakDetector detector;
	// 꼬리가 머리보다 뒤에 있는 경우
	{
		ArrayQueue<int> a{ 1, 2 };
		const ArrayQueue<int> b{ 1, 2 };

		// 이니셜라이저 테스트
		a = { 3, 4 };

		EXPECT_TRUE(a.Front() == 3); a.Dequeue();
		EXPECT_TRUE(a.Front() == 4); a.Dequeue();
		a.Enqueue(5);
		EXPECT_TRUE(a.Front() == 5); a.Dequeue();

		// 복사 대입 연산자 테스트
		a = b;

		EXPECT_TRUE(a.Front() == 1); a.Dequeue();
		EXPECT_TRUE(a.Front() == 2); a.Dequeue();
		a.Enqueue(5);
		EXPECT_TRUE(a.Front() == 5); a.Dequeue();
	}
	// 머리가 꼬리보다 뒤에있는 경우
	{
		auto borderlineTest = [](int dataCount) {
			ArrayQueue<int> a;
			// 일부러 데이터를 넣어주고 뺌으로써 머리와 꼬리를 10에 위치시킨다.
			SetQueuePos(a, 10);

			for (int i = 0; i < dataCount; i++) {
				a.Enqueue(i);
			}

			ArrayQueue<int> b;
			b = a;
			int i = 0;
			while (!b.IsEmpty()) {
				EXPECT_TRUE(b.Front() == i++);
				b.Dequeue();
			}
			EXPECT_TRUE(i == dataCount);
			ArrayQueue<int> c;
			c = Move(a);
			i = 0;
			while (!c.IsEmpty()) {
				EXPECT_TRUE(c.Front() == i++);
				c.Dequeue();
			}
			EXPECT_TRUE(i == dataCount);
		};

		// Head를 10 Tail을 0으로 맞춰보자.
		// 경계에서 제대로 복사가 되는지

		borderlineTest(22);	// Head 10 : Tail 0
		borderlineTest(23); // Head 10 : Tail 1
	}
}

#endif // TEST_ArrayQueueTest == ON


