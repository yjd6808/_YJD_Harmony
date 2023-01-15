#include <header.h>


SpinLock l;


void fn(const std::shared_ptr<int>& fsefes) {
	int l = 0;
}

void fn(const SharedPtr<int>& fsefes) {
	int l = 0;
}

struct A {

	A(const std::shared_ptr<int>& f) : m(f) {}

	std::shared_ptr<int> m;
};

struct aaa : public std::enable_shared_from_this<aaa>{
};

struct bbb : aaa
{
	void fn() {
		auto k = shared_from_this();
	}
};

struct ccc : MakeSharedFromThis<ccc>{
};

struct ddd : ccc
{
	void fn() {
		auto k = Shared();
	}
};

// template <typename T>
   // struct A
   // Derived = struct C : A<C>
   // IsBaseOf<C, A> 이렇게 비교 할 수 있을까?

template <typename T, DefaultEnableIf_t<!IsVoidType_v<T>> = nullptr>
struct a
{
	using Type = a; // -> a<b, 0>
	void fn() {
	}

	static constexpr bool IsMakeSharedFromThis = true;
};

struct b : a<b> {};
struct c : b{};



int main() {
	//auto fd = std::is_base_of_v<c, a<c>>;
	c c;
	c.fn();

	auto rdsf = make_shared<bbb>();
	rdsf->fn();

	auto s2 = MakeShared<ddd>();
	s2->fn();

	AutoMemoryLeakDetector detector;
	{
		
		HashMap<int, String> aq;
		aq.Insert(1, "fsefesfesfesf");
		aq.Insert(2, "fsefesfesfesf");
		aq.Remove(1);
	}
	int g2 = detector.StopDetect();

	SafeConsole::WriteLine("=================================================");
	return 0;
}

