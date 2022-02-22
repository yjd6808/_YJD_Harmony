/*
	�ۼ��� : ������
	�迭��� ���� �׽�Ʈ
*/


#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

#include <JCore/Random.h>

#include <JCore/Container/ListQueue.h>

using namespace JCore;
using namespace std;

#if TEST_ListQueueTest == ON

TEST(ListQueueTest, Regular) {
	MemoryLeakDetector detector;
	ListQueue<int> queue;
	int count = 320;

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

// ť�� ���� ��Ȳ������ �׽�Ʈ
TEST(ListQueueTest, FullCaseTest) {

	// ť�� ���� ���¿��� ���ͷ����Ͱ� ���� ��ȸ�ϴ� ��Ȳ
	ListQueue<int> q;
	for (int i = 0; i < 32; i++) {
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



// �迭 ť ������ �ݺ���, ������ �ݺ���, ���� ���� ��� ��� �׽�Ʈ
TEST(ListQueueTest, TotalTest) {
	Random random;

	int testCount = 5;	// 5000
	while (testCount--) {

		int dataCount = random.GenerateInt(0, 100) + 1;
		int suffleCount = 20;
		MemoryLeakDetector detector;
		ListQueue<int> queue;

		list<int> forward_list;		// ������ ���� - ������ ��
									// ť�� ������ ��� �Ȱ��� ���� �����鼭 ���������� ��ġ�ϴ��� Ȯ��
		list<int> backward_list;

		
		for (int i = 0; i < dataCount; i++) {
			queue.Enqueue(i);
			forward_list.push_back(i);
			backward_list.push_front(i);
		}

		EXPECT_TRUE(queue.Size() == dataCount);
		int r = dataCount;	// ���� ��ô
		
		// 2�� �ְ� 2�� ���� �ٽ� 32���� �־ ��ä���.

		while (suffleCount-- > 0) {
			int popSize = random.GenerateInt(1, queue.Size() + 1);
			r -= popSize;
			
			for (int i = 0; i < popSize; i++) {
				queue.Dequeue();
				forward_list.pop_front();
				backward_list.pop_back();
			}
			int queueSize1 = queue.Size();
			
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
			while (it->HasNext()) {
				v.push_back(it->Next());
			}

			// �ݺ��� ������
			while (rit->HasPrevious()) {
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
				rvIt++;
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
			forward_listit++;
		}

		myqueueit = queue.End();
		while (myqueueit->HasPrevious()) {
			int lval = *backward_listit;
			EXPECT_TRUE(myqueueit->Previous() == lval);
			backward_listit++;
		}
	}	
}

TEST(ListQueueTest, ConstructorTest) {
	MemoryLeakDetector detector;

	ListQueue<int> a{ 1, 2, 3 };

	EXPECT_TRUE(a.Front() == 1); a.Dequeue();
	EXPECT_TRUE(a.Front() == 2); a.Dequeue();

	ListQueue<int> b(a);
	EXPECT_TRUE(b.Front() == 3); b.Dequeue();
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(a.Size() == 1);

	b.Enqueue(1);
	b.Enqueue(5);

	EXPECT_TRUE(b.Front() == 1);
	EXPECT_TRUE(b.Size() == 2);

	ListQueue<int> c(Move(b));
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(c.Front() == 1);
	EXPECT_TRUE(c.Size() == 2);
}



TEST(ListQueueTest, OperatorTest) {
	MemoryLeakDetector detector;

	ListQueue<Model> a{ 1, 2, 3 };

	ListQueue<Model> b{ 6, 5, 4, 3, 2, 1 };

	// ���� ����
	b = a;	// 1, 2, 3�� �ݰ�

	int i = 1;
	while (!b.IsEmpty()) {
		auto k = b.Front();
		EXPECT_TRUE(k.a == i);
		b.Dequeue();
		i++;
	}

	ListQueue<Model> c{ 1, 2, 3 };
	b.EnqueueAll(c);
	EXPECT_TRUE(b.Size() == 3);
	EXPECT_TRUE(b.Front().a == 1); b.Dequeue();
	EXPECT_TRUE(b.Front().a == 2); b.Dequeue();
	EXPECT_TRUE(b.Front().a == 3); b.Dequeue();
	EXPECT_TRUE(b.Size() == 0);


	// �̴ϼȶ����� ���� ����
	b = { 1, 3, 5, 6 };
	EXPECT_TRUE(b.Size() == 4);
	EXPECT_TRUE(b.Front().a == 1); b.Dequeue();
	EXPECT_TRUE(b.Front().a == 3); b.Dequeue();
	EXPECT_TRUE(b.Size() == 2);


	ListQueue<Model> d;
	d = Move(b);
	EXPECT_TRUE(b.Size() == 0);
	EXPECT_TRUE(d.Size() == 2);
	
	auto it = d.Begin();
	EXPECT_TRUE(it->Next().a == 5);
	EXPECT_TRUE(it->Next().a == 6);
	EXPECT_TRUE(it->HasNext() == false);
}

#endif // TEST_ListQueueTest == ON


