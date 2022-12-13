/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 4:01:33 PM
 * =====================
 */

namespace JCore {

	template <typename, typename>
	class HashMap;
	class IMemoryPool
	{
	public:
		virtual ~IMemoryPool() = 0;

		virtual void Initialize(const HashMap<int, int>& Counter) = 0;
		virtual void Finalize() = 0;
		virtual void* DynamicPop(int size) = 0;
		virtual void DynamicPush(void* memory, int size) = 0;
	};

	inline IMemoryPool::~IMemoryPool() = default;

} // namespace JCore