#include "JPCH.h"
#include <JCore.h>
#include <JUniquePointer.h>
#include <JString.h>

#include <cstddef>
#include <stdexcept>
#include <cstring>
#include <ostream>

class Node {
	static const int MAX = 100;
public:
	Node() {
		cout << "\t노드 디폴트 생성자(" << 0 << ")\n";
		m_piValues = new int[MAX];
		std::fill(m_piValues, m_piValues + MAX, 0);
	}
	Node(int data) {
		cout << "\t노드 생성자(" << data << ")\n";
		m_piValues = new int[MAX];
		std::fill(m_piValues, m_piValues + MAX, data);
	}

	Node(const Node& node) {
		cout << "\t노드 복사 생성자(" << node.Val() << ")\n";
		m_piValues = new int[MAX];
		memcpy_s(m_piValues, sizeof(int) * MAX, node.m_piValues, sizeof(int) * MAX);
	}
	~Node() {
		JSAFE_DELETE_ARRAY(m_piValues);
	}

	Node operator=(const Node& other) {
		cout << "\t노드 복사 대입(" << other.Val() << ")\n";
		return Node(other.Val());
	}

	Node& operator=(Node&& other) {
		cout << "\t노드 이동 대입(" << other.Val() << ")\n";
		m_piValues = other.m_piValues;
		other.m_piValues = nullptr;
		return *this;
	}

	int Val() const {
		return m_piValues[99];
	}


	friend std::ostream& operator<<(std::ostream& os, const Node& other);
private:
	int* m_piValues;
};

#ifdef _MSC_VER
#    define CONSTEXPR11_TN constexpr
#    define CONSTEXPR14_TN
#    define NOEXCEPT_TN noexcept
#endif  // _MSC_VER

class static_string
{
	const char* const p_;
	const std::size_t sz_;

public:
	typedef const char* const_iterator;

	template <std::size_t N>
	CONSTEXPR11_TN static_string(const char(&a)[N]) NOEXCEPT_TN
		: p_(a)
		, sz_(N - 1)
	{}

	CONSTEXPR11_TN static_string(const char* p, std::size_t N) NOEXCEPT_TN
		: p_(p)
		, sz_(N)
	{}

	CONSTEXPR11_TN const char* data() const NOEXCEPT_TN { return p_; }
	CONSTEXPR11_TN std::size_t size() const NOEXCEPT_TN { return sz_; }

	CONSTEXPR11_TN const_iterator begin() const NOEXCEPT_TN { return p_; }
	CONSTEXPR11_TN const_iterator end()   const NOEXCEPT_TN { return p_ + sz_; }

	CONSTEXPR11_TN char operator[](std::size_t n) const
	{
		return n < sz_ ? p_[n] : throw std::out_of_range("static_string");
	}
};

inline std::ostream& operator<<(std::ostream& os, static_string const& s)
{
	return os.write(s.data(), s.size());
}

template <class T>
CONSTEXPR14_TN static_string type_name()
{
	static_string p = __FUNCSIG__;
	return static_string(p.data() + 38, p.size() - 38 - 7);
}




TEST(JUnqiuePointerTest, Creation) {
	
}

std::ostream& operator<<(std::ostream& os, const Node& other)
{
	os << other.Val();
	return os;
}
