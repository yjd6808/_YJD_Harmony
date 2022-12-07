/*
 * 작성자: 윤정도
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Sync/SpinLock.h>

namespace JCore {

	template class LockGuard<SpinLock>;

	SpinLock::SpinLock() : m_bLocked(false) {
	}

	SpinLock::~SpinLock() = default;

	// @참고1 : https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange - compare_exchange_weak
	// @참고2 : https://jacking75.github.io/C++_std_atomic_compare_exchange_strong/ - 한글로 설명된 것(잭킹75)
	// bool compare_exchange_weak( T& expected, T desired )
	//		return		: 현재값이 expected와 일치할 경우 true 일치하지 않을 경우 false를 반환한다.
	//      expected	: 비교 대상
	//      desired		: 바꾸고자하는 값
	// 
	// [동작]
	// 만약 현재값과 expected값이 틀리면 expected를 현재값으로 변경함
	// 만약 현재값과 expected값이 같으면 현재값을 desired값으로 변경함
	/*
	   @궁금증
	   근데 결국 CPU 점유를 계속해서 busy-waiting 상태를 계속 유지하는건데

	   while (m_bLocked) {
		   Sleep(0);
	   }
	   m_bLocked = true;
	   아 이건 안되겠네. while문을 동시에 2개의 쓰레드가 빠져 나와버리는 경우가 있을 수 있을 것 같은데?
	 */

	 // @학습 : https://modoocode.com/271
	 // memory_order_acquire : 해당 명령 이후에 오는 모든 메모리 명령들이 해당 명령 이전으로 재배치 되는 것을 금지한다
	 // memory_order_release : 해당 명령 이전에 오는 모든 메모리 명령들이 해당 명령 이후로   재배치 되는 것을 금지한다.
	 // memory_order_seq_cst : 메모리 명령의 순차적 일관성을 보장한다.
	 // memory_order_acq_rel : acquire 와 release 를 모두 수행하는 것입니다. 이는, 읽기와 쓰기를 모두 수행하는 명령들, 예를 들어서 fetch_add 와 같은 함수에서 사용될 수 있습니다.
	 // 순차적 일관성이란, 메모리 명령 재배치도 없고, 모든 쓰레드에서 모든 시점에 동일한 값을 관찰할 수 있는, 여러분이 생각하는 그대로 CPU 가 작동하는 방식이라 생각하면 됩니다.

	 // compare_exchange_strong 함수는 기본적으로 memory_order_seq_cst를 사용하도록 하고 있는데
	 // 인텔에서는 비용이 그렇게 크지 않은데 ARM CPU는 일관성을 보장하기 위한 비용이 매우 크다고 한다.
	void SpinLock::Lock() {
		bool expected = false;

		while (!m_bLocked.compare_exchange_strong(expected, true)) {


			// 여기 들어온 경우 이미 다른 쓰레드가 락을 획득한 상태이다.
			//   = 기존 값(m_bLocked)이 true이기 때문에 compare_exchange_strong 함수 결과가 false를 반환했다.
			// 
			// 그냥 쌔리마 반복문 계속 돌면서 열때까지 문을 계속 뚜들김

			expected = false;
			// 이걸 해주는 이유가 m_bLocked가 true인 경우 expected가 현재 값(true)로 변경되어 버리기 때문에
			// 다시 compare_exchange_strong을 호출 할 때 그냥 무조건 반복문을 탈출해버려서 여러 쓰레드가 
			// Critical Section에 진입하는 경우가 생길 수 있게 된다.
		}
	}

	void SpinLock::Unlock() {
		m_bLocked = false;
	}

	bool SpinLock::TryLock() {
		bool expected = false;
		return m_bLocked.compare_exchange_strong(expected, true);
	}

	bool SpinLock::IsLocked() {
		return m_bLocked;
	}

} // namespace JCore