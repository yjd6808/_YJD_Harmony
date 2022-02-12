/*
	�ۼ��� : ������
	�迭��� ���� �׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCore/Random.h>
#include <JCore/Container/ArrayQueue.h>

using namespace JCore;
using namespace std;

#if TEST_ArrayQueueTest == ON

TEST(ArrayQueueTest, Regular) {
	MemoryLeakDetector detector;
	ArrayQueue<int> queue;
	int count = 320;

	for (int i = 0; i < count; i++) {
		queue.Enqueue(i);
	}

	for (int i = 0; i < count; i++) {
		EXPECT_TRUE(queue.Front() == i);
		queue.Dequeue();
	}

	EXPECT_TRUE(queue.GetSize() == 0);

	for (int i = 0; i < count; i++) {
		queue.Enqueue(i);
	}

	queue.Clear();
	EXPECT_TRUE(queue.GetSize() == 0);
}

// ť�� ���� ��Ȳ������ �׽�Ʈ
TEST(ArrayQueueTest, FullCaseTest) {

	// ť�� ���� ���¿��� ���ͷ����Ͱ� ���� ��ȸ�ϴ� ��Ȳ
	ArrayQueue<int> q;
	int z = q.GetCapacity();

	for (int i = 0; i < z; i++) {
		q.Enqueue(i);
	}

	auto it = q.Begin();
	int i = 0;
	while (it->HasValue()) {
		it->Next();
		i++;
	}

	it = q.End();

	while (it->HasValue()) {
		it->Previous();
		i--;
	}

	EXPECT_TRUE(i == 0);
}



// �迭 ť ������ �ݺ���, ������ �ݺ���, ���� ���� ��� ��� �׽�Ʈ
TEST(ArrayQueueTest, TotalTest) {
	Random random;

	int testCount = 5;	// 5000
	while (testCount--) {

		int dataCount = random.GenerateInt(0, 100) + 1;
		int suffleCount = 20;
		MemoryLeakDetector detector;
		ArrayQueue<int> queue;

		list<int> forward_list;		// ������ ���� - ������ ��
									// ť�� ������ ��� �Ȱ��� ���� �����鼭 ���������� ��ġ�ϴ��� Ȯ��
		list<int> backward_list;

		
		for (int i = 0; i < dataCount; i++) {
			queue.Enqueue(i);
			forward_list.push_back(i);
			backward_list.push_front(i);
		}

		EXPECT_TRUE(queue.GetSize() == dataCount);
		int r = dataCount;	// ���� ��ô
		
		// 2�� �ְ� 2�� ���� �ٽ� 32���� �־ ��ä���.

		while (suffleCount-- > 0) {
			int popSize = random.GenerateInt(1, queue.GetSize() + 1);
			r -= popSize;
			
			for (int i = 0; i < popSize; i++) {
				queue.Dequeue();
				forward_list.pop_front();
				backward_list.pop_back();
			}
			int queueSize1 = queue.GetSize();
			
			vector<int> v;	// ������ ����
							// �̹� ���� �ܰ迡�� �����Ͱ� ��ġ�ϴ���
			list<int> rv; // �ݴ�������� ������ ����
			
			
			//PrintFormat("����/ť ������ �� : %d/%d\n", suffleCount, queueSize1);
			//���� �����ִ� �͵��� �־���
			Enumerator<int> it = queue.Begin();
			Enumerator<int> rit = queue.End();

			if (queueSize1 == 0) {
				int a = 40;
			}

			// �ݺ��� ������
			while (it->HasValue()) {
				v.push_back(it->Next());
			}

			// �ݺ��� ������
			while (rit->HasValue()) {
				rv.push_back(rit->Previous());
			}
			
			int enqueueSize = random.GenerateInt(1, 50);
			r += enqueueSize;

			// ť�� 1 ~ �����ִ� �͵� ������ ���� ������ŭ �ٽ� �־���
			for (int i = 0; i < enqueueSize; i++) {
				queue.Enqueue(i);

				v.push_back(i);
				forward_list.push_back(i);

				rv.push_front(i);
				backward_list.push_front(i);
			}

			int queueSize2 = queue.GetSize();

			it = queue.Begin();
			int k = 0;
			while (it->HasValue()) {
				EXPECT_TRUE(v[k++] == it->Next());
			}

			auto rvIt = rv.begin();
			rit = queue.End();
			k = 0;
			while (rit->HasValue()) {
				int value = rit->Previous();
				int vv = *rvIt;
				EXPECT_TRUE(vv == value);
				rvIt++;
			}

			EXPECT_TRUE(forward_list.size() == queue.GetSize());
			EXPECT_TRUE(backward_list.size() == queue.GetSize());
		}

		EXPECT_TRUE(queue.GetSize() == r);

		auto forward_listit = forward_list.begin();
		auto backward_listit = backward_list.begin();
		auto myqueueit = queue.Begin();

		while (myqueueit->HasValue()) {
			int lval = *forward_listit;
			EXPECT_TRUE(myqueueit->Next() == lval);
			forward_listit++;
		}

		myqueueit = queue.End();
		while (myqueueit->HasValue()) {
			int lval = *backward_listit;
			EXPECT_TRUE(myqueueit->Previous() == lval);
			backward_listit++;
		}
	}
	
}

#endif // TEST_ArrayQueueTest == ON


