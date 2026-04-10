#include <cstdio>
#include <cstdint>
// ============================================================
// 테스트 헬퍼
// ============================================================

static int g_pass = 0;
static int g_fail = 0;

#define CHECK(expr, expected, actual)                              \
    do {                                                           \
        auto _exp = (expected);                                    \
        auto _act = (actual);                                      \
        if (_exp == _act) {                                        \
            std::printf("[PASS] %s\n", expr);                      \
            ++g_pass;                                              \
        } else {                                                   \
            std::printf("[FAIL] %s  expected=%lld  got=%lld\n",    \
                       expr, (long long)_exp, (long long)_act);    \
            ++g_fail;                                              \
        }                                                          \
    } while (0)

extern "C"
{
	_s64 abs_i64(_s64 x);
	_s64 max_i64(_s64 a, _s64 b);
	_s64 min_i64(_s64 a, _s64 b);
	_s64 clamp_i64(_s64 x, _s64 lo, _s64 hi);
	_u64 factorial_u64(_u64 n);
	_u64 count_bits_u64(_u64 x);
	_u64 reverse_bits_u64(_u64 x);
	_s64 sum_array_i64(_s64* arr, _s64 n);
	_s64 max_array_i64(_s64* arr, _s64 n);
	_s64 linear_search_i64(_s64* arr, _s64 n, _s64 value);
	_s64 dot_product_i64(_s64* a, _s64* b, _s64 n);
	_u64 strlen_ascii(const char* s);
	_s64 strcmp_ascii(const char* a, const char* b);
	void to_upper_ascii(char* s);
	void* memcpy_u8(void* dst, const void* src, _u64 n);
}

static _s64 sign64(_s64 v) {
	return v < 0 ? -1 : (v > 0 ? 1 : 0);
}

// ============================================================
// 1. abs_i64
// ============================================================

void test_abs_i64() {
	std::puts("--- abs_i64 ---");
	CHECK("abs(0)", 0LL, abs_i64(0));
	CHECK("abs(1)", 1LL, abs_i64(1));
	CHECK("abs(-1)", 1LL, abs_i64(-1));
	CHECK("abs(100)", 100LL, abs_i64(100));
	CHECK("abs(-100)", 100LL, abs_i64(-100));
	CHECK("abs(42)", 42LL, abs_i64(42));
	CHECK("abs(-42)", 42LL, abs_i64(-42));
	CHECK("abs(999999)", 999999LL, abs_i64(999999));
	CHECK("abs(-999999)", 999999LL, abs_i64(-999999));
	CHECK("abs(INT64_MAX)", (std::numeric_limits<_s64>::max)(),
		abs_i64((std::numeric_limits<_s64>::max)()));
}

// ============================================================
// 2. max_i64
// ============================================================

void test_max_i64() {
	std::puts("--- max_i64 ---");
	CHECK("max(1,2)", 2LL, max_i64(1, 2));
	CHECK("max(2,1)", 2LL, max_i64(2, 1));
	CHECK("max(0,0)", 0LL, max_i64(0, 0));
	CHECK("max(-1,0)", 0LL, max_i64(-1, 0));
	CHECK("max(-5,-3)", -3LL, max_i64(-5, -3));
	CHECK("max(100,100)", 100LL, max_i64(100, 100));
	CHECK("max(-100,100)", 100LL, max_i64(-100, 100));
	CHECK("max(0,-1)", 0LL, max_i64(0, -1));
	CHECK("max(7,3)", 7LL, max_i64(7, 3));
	CHECK("max(min,max)", (std::numeric_limits<_s64>::max)(),
		max_i64((std::numeric_limits<_s64>::min)(),
			(std::numeric_limits<_s64>::max)()));
}

// ============================================================
// 3. min_i64
// ============================================================

void test_min_i64() {
	std::puts("--- min_i64 ---");
	CHECK("min(1,2)", 1LL, min_i64(1, 2));
	CHECK("min(2,1)", 1LL, min_i64(2, 1));
	CHECK("min(0,0)", 0LL, min_i64(0, 0));
	CHECK("min(-1,0)", -1LL, min_i64(-1, 0));
	CHECK("min(-5,-3)", -5LL, min_i64(-5, -3));
	CHECK("min(100,100)", 100LL, min_i64(100, 100));
	CHECK("min(-100,100)", -100LL, min_i64(-100, 100));
	CHECK("min(0,-1)", -1LL, min_i64(0, -1));
	CHECK("min(7,3)", 3LL, min_i64(7, 3));
	CHECK("min(min,max)", (std::numeric_limits<_s64>::min)(),
		min_i64((std::numeric_limits<_s64>::min)(),
			(std::numeric_limits<_s64>::max)()));
}

// ============================================================
// 4. clamp_i64
// ============================================================

void test_clamp_i64() {
	std::puts("--- clamp_i64 ---");
	CHECK("clamp(5,1,10)", 5LL, clamp_i64(5, 1, 10));
	CHECK("clamp(0,1,10)", 1LL, clamp_i64(0, 1, 10));
	CHECK("clamp(11,1,10)", 10LL, clamp_i64(11, 1, 10));
	CHECK("clamp(1,1,10)", 1LL, clamp_i64(1, 1, 10));
	CHECK("clamp(10,1,10)", 10LL, clamp_i64(10, 1, 10));
	CHECK("clamp(-5,-3,3)", -3LL, clamp_i64(-5, -3, 3));
	CHECK("clamp(5,-3,3)", 3LL, clamp_i64(5, -3, 3));
	CHECK("clamp(0,-3,3)", 0LL, clamp_i64(0, -3, 3));
	CHECK("clamp(100,0,50)", 50LL, clamp_i64(100, 0, 50));
	CHECK("clamp(-100,0,50)", 0LL, clamp_i64(-100, 0, 50));
}

// ============================================================
// 5. factorial_u64
// ============================================================

void test_factorial_u64() {
	std::puts("--- factorial_u64 ---");
	CHECK("0!", 1ULL, factorial_u64(0));
	CHECK("1!", 1ULL, factorial_u64(1));
	CHECK("2!", 2ULL, factorial_u64(2));
	CHECK("3!", 6ULL, factorial_u64(3));
	CHECK("4!", 24ULL, factorial_u64(4));
	CHECK("5!", 120ULL, factorial_u64(5));
	CHECK("6!", 720ULL, factorial_u64(6));
	CHECK("7!", 5040ULL, factorial_u64(7));
	CHECK("8!", 40320ULL, factorial_u64(8));
	CHECK("10!", 3628800ULL, factorial_u64(10));
}

// ============================================================
// 6. count_bits_u64
// ============================================================

void test_count_bits_u64() {
	std::puts("--- count_bits_u64 ---");
	CHECK("bits(0)", 0ULL, count_bits_u64(0));
	CHECK("bits(1)", 1ULL, count_bits_u64(1));
	CHECK("bits(0xFF)", 8ULL, count_bits_u64(0xFF));
	CHECK("bits(~0ULL)", 64ULL, count_bits_u64(~0ULL));
	CHECK("bits(0x5555..)", 32ULL, count_bits_u64(0x5555555555555555ULL));
	CHECK("bits(0xAAAA..)", 32ULL, count_bits_u64(0xAAAAAAAAAAAAAAAAULL));
	CHECK("bits(0x100)", 1ULL, count_bits_u64(0x100ULL));
	CHECK("bits(7)", 3ULL, count_bits_u64(7ULL));
	CHECK("bits(15)", 4ULL, count_bits_u64(15ULL));
	CHECK("bits(0x0F0F0F0F)", 16ULL, count_bits_u64(0x0F0F0F0FULL));
}

// ============================================================
// 7. reverse_bits_u64
// ============================================================

void test_reverse_bits_u64() {
	std::puts("--- reverse_bits_u64 ---");

	 CHECK("rev(0)", 0ULL, reverse_bits_u64(0ULL));
	 CHECK("rev(~0ULL)", ~0ULL, reverse_bits_u64(~0ULL));
	 CHECK("rev(1)", 0x8000000000000000ULL, reverse_bits_u64(1ULL));
	 CHECK("rev(MSB)", 1ULL, reverse_bits_u64(0x8000000000000000ULL));
	 CHECK("rev(0x0F00000000000000)",
	 	0x00000000000000F0ULL, reverse_bits_u64(0x0F00000000000000ULL));
	 CHECK("rev(0x00000000000000F0)",
	 	0x0F00000000000000ULL, reverse_bits_u64(0x00000000000000F0ULL));
	 CHECK("rev(0xF0F0F0F0F0F0F0F0)",
	 	0x0F0F0F0F0F0F0F0FULL, reverse_bits_u64(0xF0F0F0F0F0F0F0F0ULL));
	 CHECK("rev(0x0F0F0F0F0F0F0F0F)",
	 	0xF0F0F0F0F0F0F0F0ULL, reverse_bits_u64(0x0F0F0F0F0F0F0F0FULL));
	 CHECK("rev(0xAAAAAAAAAAAAAAAA)",
	 	0x5555555555555555ULL, reverse_bits_u64(0xAAAAAAAAAAAAAAAAULL));
	 CHECK("rev(0x5555555555555555)",
	 	0xAAAAAAAAAAAAAAAAULL, reverse_bits_u64(0x5555555555555555ULL));
}

// ============================================================
// 8. sum_array_i64
// ============================================================

void test_sum_array_i64() {
	std::puts("--- sum_array_i64 ---");
	_s64 a1[] = { 1, 2, 3, 4, 5 };
	_s64 a2[] = { -1, -2, -3 };
	_s64 a3[] = { 0 };
	_s64* a4 = nullptr;
	_s64 a5[] = { 100, -100 };
	_s64 a6[] = { (std::numeric_limits<_s64>::max)(), 0 };
	_s64 a7[] = { 1 };
	_s64 a8[] = { 10, 20, 30 };
	_s64 a9[] = { -5, 5, -5, 5 };
	_s64 a10[] = { 1000, 2000, 3000, 4000 };

	CHECK("sum {1..5}", 15LL, sum_array_i64(a1, 5));
	CHECK("sum {-1,-2,-3}", -6LL, sum_array_i64(a2, 3));
	CHECK("sum {0}", 0LL, sum_array_i64(a3, 1));
	CHECK("sum n=0", 0LL, sum_array_i64(a4, 0));
	CHECK("sum {100,-100}", 0LL, sum_array_i64(a5, 2));
	CHECK("sum {INT64_MAX,0}", (std::numeric_limits<_s64>::max)(),
		sum_array_i64(a6, 2));
	CHECK("sum {1}", 1LL, sum_array_i64(a7, 1));
	CHECK("sum {10,20,30}", 60LL, sum_array_i64(a8, 3));
	CHECK("sum {-5,5,-5,5}", 0LL, sum_array_i64(a9, 4));
	CHECK("sum {1k,2k,3k,4k}", 10000LL, sum_array_i64(a10, 4));
}

// ============================================================
// 9. max_array_i64
// ============================================================

void test_max_array_i64() {
	std::puts("--- max_array_i64 ---");
	_s64 a1[] = { 3, 1, 4, 1, 5, 9, 2, 6 };
	_s64 a2[] = { -3, -1, -4 };
	_s64 a3[] = { 42 };
	_s64 a4[] = { 0, 0, 0 };
	_s64 a5[] = { (std::numeric_limits<_s64>::max)(), 0, -1 };
	_s64 a6[] = { 5, 5, 5 };
	_s64 a7[] = { -100, 100 };
	_s64 a8[] = { 100, -100 };
	_s64 a9[] = { 1, 2, 3, 4, 5 };
	_s64 a10[] = { 5, 4, 3, 2, 1 };

	CHECK("max {3,1,4,1,5,9,2,6}", 9LL, max_array_i64(a1, 8));
	CHECK("max all neg", -1LL, max_array_i64(a2, 3));
	CHECK("max single", 42LL, max_array_i64(a3, 1));
	CHECK("max all zero", 0LL, max_array_i64(a4, 3));
	CHECK("max with INT64_MAX",
		(std::numeric_limits<_s64>::max)(),
		max_array_i64(a5, 3));
	CHECK("max all same", 5LL, max_array_i64(a6, 3));
	CHECK("max {-100,100}", 100LL, max_array_i64(a7, 2));
	CHECK("max {100,-100}", 100LL, max_array_i64(a8, 2));
	CHECK("max ascending", 5LL, max_array_i64(a9, 5));
	CHECK("max descending", 5LL, max_array_i64(a10, 5));
}

// ============================================================
// 10. linear_search_i64
// ============================================================

void test_linear_search_i64() {
	std::puts("--- linear_search_i64 ---");
	_s64 a[] = { 10, 20, 30, 40, 50 };
	_s64 b[] = { -1, -2, -3 };
	_s64 c[] = { 7 };
	_s64 d[] = { 5, 5, 5 };
	_s64* e = nullptr;

	CHECK("find 10 -> 0", 0LL, linear_search_i64(a, 5, 10));
	CHECK("find 50 -> 4", 4LL, linear_search_i64(a, 5, 50));
	CHECK("find 30 -> 2", 2LL, linear_search_i64(a, 5, 30));
	CHECK("find 99 -> -1", -1LL, linear_search_i64(a, 5, 99));
	CHECK("n=0 -> -1", -1LL, linear_search_i64(a, 0, 10));
	CHECK("find -2 -> 1", 1LL, linear_search_i64(b, 3, -2));
	CHECK("find 0 -> -1", -1LL, linear_search_i64(b, 3, 0));
	CHECK("single match", 0LL, linear_search_i64(c, 1, 7));
	CHECK("single no match", -1LL, linear_search_i64(c, 1, 8));
	CHECK("dup -> first 0", 0LL, linear_search_i64(d, 3, 5));
}

// ============================================================
// 11. dot_product_i64
// ============================================================

void test_dot_product_i64() {
	std::puts("--- dot_product_i64 ---");
	_s64 a1[] = { 1, 2, 3 }, b1[] = { 4, 5, 6 };      // 1*4+2*5+3*6 = 32
	_s64 a2[] = { 0, 0 }, b2[] = { 1, 1 };
	_s64 a3[] = { -1, -2 }, b3[] = { 3, 4 };         // -3-8=-11
	_s64 a4[] = { 2 }, b4[] = { 3 };
	_s64 a5[] = { 1, 0, -1 }, b5[] = { -1, 99, 1 };    // -1+0-1=-2
	_s64 a6[] = { 10, 10 }, b6[] = { 10, 10 };       // 100+100=200
	_s64 a7[] = { -1, -1 }, b7[] = { -1, -1 };       // 1+1=2
	_s64 a8[] = { 100,200,300 }, b8[] = { 1,2,3 };    // 100+400+900=1400
	_s64 a9[] = { 1,2,3,4,5 }, b9[] = { 5,4,3,2,1 };  // 35
	_s64 a10[] = { 1,1,1 }, b10[] = { 1,-1,1 };     // 1-1+1=1

	CHECK("dot {1,2,3}·{4,5,6}", 32LL, dot_product_i64(a1, b1, 3));
	CHECK("dot zeros", 0LL, dot_product_i64(a2, b2, 2));
	CHECK("dot neg·pos", -11LL, dot_product_i64(a3, b3, 2));
	CHECK("dot single", 6LL, dot_product_i64(a4, b4, 1));
	CHECK("dot n=0", 0LL, dot_product_i64(a1, b1, 0));
	CHECK("dot {1,0,-1}·{-1,99,1}", -2LL, dot_product_i64(a5, b5, 3));
	CHECK("dot {10,10}·{10,10}", 200LL, dot_product_i64(a6, b6, 2));
	CHECK("dot neg·neg", 2LL, dot_product_i64(a7, b7, 2));
	CHECK("dot {100,200,300}·{1,2,3}",
		1400LL, dot_product_i64(a8, b8, 3));
	CHECK("dot {1..5}·{5..1}", 35LL, dot_product_i64(a9, b9, 5));
}

// ============================================================
// 12. strlen_ascii
// ============================================================

void test_strlen_ascii() {
	std::puts("--- strlen_ascii ---");
	CHECK("strlen \"\"", 0ULL, strlen_ascii(""));
	CHECK("strlen \"a\"", 1ULL, strlen_ascii("a"));
	CHECK("strlen \"hello\"", 5ULL, strlen_ascii("hello"));
	CHECK("strlen \"0123456789\"", 10ULL, strlen_ascii("0123456789"));
	CHECK("strlen nullptr", 0ULL, strlen_ascii(nullptr)); // 구현이 0 처리 시
	CHECK("strlen \" \"", 1ULL, strlen_ascii(" "));
	CHECK("strlen \"abc\"", 3ULL, strlen_ascii("abc"));
	CHECK("strlen \"\\n\"", 1ULL, strlen_ascii("\n"));
	CHECK("strlen \"A\"", 1ULL, strlen_ascii("A"));
	CHECK("strlen 26chars", 26ULL, strlen_ascii("abcdefghijklmnopqrstuvwxyz"));
}

// ============================================================
// 13. strcmp_ascii
// ============================================================

void test_strcmp_ascii() {
	std::puts("--- strcmp_ascii ---");
	CHECK("cmp \"aa\"==\"aa\"", 0LL, sign64(strcmp_ascii("aa", "aa")));
	CHECK("cmp \"a\"<\"b\"", -1LL, sign64(strcmp_ascii("a", "b")));
	CHECK("cmp \"b\">\"a\"", 1LL, sign64(strcmp_ascii("b", "a")));
	CHECK("cmp \"\"==\"\"", 0LL, sign64(strcmp_ascii("", "")));
	CHECK("cmp \"a\">\"\"", 1LL, sign64(strcmp_ascii("a", "")));
	CHECK("cmp \"\"<\"a\"", -1LL, sign64(strcmp_ascii("", "a")));
	CHECK("cmp \"abc\"==\"abc\"", 0LL, sign64(strcmp_ascii("abc", "abc")));
	CHECK("cmp \"abd\">\"abc\"", 1LL, sign64(strcmp_ascii("abd", "abc")));
	CHECK("cmp \"ab\"<\"abc\"", -1LL, sign64(strcmp_ascii("ab", "abc")));
	CHECK("cmp \"z\">\"a\"", 1LL, sign64(strcmp_ascii("z", "a")));
}

// ============================================================
// 14. to_upper_ascii
// ============================================================

void test_to_upper_ascii() {
	std::puts("--- to_upper_ascii ---");

	auto check_str = [](const char* label,
		const char* expected,
		char* buf) {
		bool ok = std::strcmp(buf, expected) == 0;
		std::printf("[%s] %s  expected=\"%s\"  got=\"%s\"\n",
			ok ? "PASS" : "FAIL", label, expected, buf);
		ok ? ++g_pass : ++g_fail;
	};

	char s1[] = "hello";    to_upper_ascii(s1);  check_str("hello->HELLO", "HELLO", s1);
	char s2[] = "WORLD";    to_upper_ascii(s2);  check_str("WORLD->WORLD", "WORLD", s2);
	char s3[] = "Hello";    to_upper_ascii(s3);  check_str("Hello->HELLO", "HELLO", s3);
	char s4[] = "";         to_upper_ascii(s4);  check_str("empty->empty", "", s4);
	char s5[] = "abc123";   to_upper_ascii(s5);  check_str("abc123->ABC123", "ABC123", s5);
	char s6[] = "a";        to_upper_ascii(s6);  check_str("a->A", "A", s6);
	char s7[] = "z";        to_upper_ascii(s7);  check_str("z->Z", "Z", s7);
	char s8[] = "aZ";       to_upper_ascii(s8);  check_str("aZ->AZ", "AZ", s8);
	char s9[] = "!@#";      to_upper_ascii(s9);  check_str("!@# unchanged", "!@#", s9);
	char s10[] = "abcDEF";   to_upper_ascii(s10); check_str("abcDEF->ABCDEF", "ABCDEF", s10);
}

// ============================================================
// 15. memcpy_u8
// ============================================================

void test_memcpy_u8() {
	std::puts("--- memcpy_u8 ---");

	auto check_mem = [](const char* label,
		const uint8_t* expected,
		const uint8_t* got,
		size_t n) {
		bool ok = std::memcmp(expected, got, n) == 0;
		std::printf("[%s] %s\n", ok ? "PASS" : "FAIL", label);
		ok ? ++g_pass : ++g_fail;
	};

	uint8_t src1[] = { 1,2,3,4,5 }, dst1[5] = {};
	memcpy_u8(dst1, src1, 5);  check_mem("copy 5 bytes", src1, dst1, 5);

	uint8_t dst2[1] = {};
	memcpy_u8(dst2, src1, 1);  check_mem("copy 1 byte", src1, dst2, 1);

	memcpy_u8(dst1, src1, 0);  check_mem("copy 0 bytes (unchanged)", src1, dst1, 5);

	uint8_t src3[] = { 0xFF, 0x00, 0xAB }, dst3[3] = {};
	memcpy_u8(dst3, src3, 3);  check_mem("copy {FF,00,AB}", src3, dst3, 3);

	uint8_t src4[10]; for (int i = 0; i < 10; ++i) src4[i] = (uint8_t)i;
	uint8_t dst4[10] = {};
	memcpy_u8(dst4, src4, 10); check_mem("copy 10 bytes", src4, dst4, 10);

	uint8_t src5[] = { 42 }, dst5[1] = {};
	memcpy_u8(dst5, src5, 1);  check_mem("copy {42}", src5, dst5, 1);

	uint8_t src6[] = { 0,0,0 }, dst6[3] = { 1,1,1 };
	memcpy_u8(dst6, src6, 3);  check_mem("copy zeros", src6, dst6, 3);

	uint8_t src7[] = { 1,2 }, dst7[3] = { 0,0,99 };
	memcpy_u8(dst7, src7, 2);  check_mem("partial copy 2/3", src7, dst7, 2);

	uint8_t src8[] = { 255,128,64,32,16,8,4,2 };
	uint8_t dst8[8] = {};
	memcpy_u8(dst8, src8, 8);  check_mem("copy 8 bytes", src8, dst8, 8);

	// 반환값 검증 (dst 포인터)
	uint8_t src9[] = { 7 }, dst9[1] = {};
	void* ret = memcpy_u8(dst9, src9, 1);
	bool ok_ret = (ret == (void*)dst9);
	std::printf("[%s] memcpy_u8 return==dst\n", ok_ret ? "PASS" : "FAIL");
	ok_ret ? ++g_pass : ++g_fail;
}

// ============================================================
// Main Test Function
// ============================================================

int call_01_AsmStudy(int _argc, char** _argv)
{
	test_abs_i64();
	test_max_i64();
	test_min_i64();
	test_clamp_i64();
	test_factorial_u64();
	test_count_bits_u64();
	test_reverse_bits_u64();
	test_sum_array_i64();
	test_max_array_i64();
	test_linear_search_i64();
	test_dot_product_i64();
	test_strlen_ascii();
	test_strcmp_ascii();
	test_to_upper_ascii();
	test_memcpy_u8();

	return 0;
}
