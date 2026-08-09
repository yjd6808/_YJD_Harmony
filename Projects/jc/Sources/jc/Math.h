/*
	작성자 : 윤정도
	간단한 수학 학수
*/

#pragma once

#include "jc/Namespace.h"
#include "jc/Type.h"
#include <cmath>

// =====================================================================================
// 상수/단위변환 매크로 (sgf/Math/SgfMath.h에서 이관, 2026-08-09)
//  - 매크로라 네임스페이스와 무관하게 어디서든 바로 사용할 수 있다.
// =====================================================================================
#define jc_math_pi				(3.14159265358979323846f)	// 원주율 π
#define jc_math_pi2				(jc_math_pi * 2.0f)			// π × 2 (360도)
#define jc_math_pi_half			(jc_math_pi * 0.5f)			// π / 2 (90도)
#define jc_math_pi_div2			(jc_math_pi / 2.0f)			// π / 2
#define jc_math_pi_div4			(jc_math_pi / 4.0f)			// π / 4 (45도)

// 도(degree) -> 라디안(radian) 변환
// 수학 함수(sin/cos)는 모두 라디안을 받기 때문에 변환이 필요하다.
// 180도 = π 라디안이므로, 1도 = π/180 라디안이다.
#define jc_math_deg2rad(_deg)	(((_deg) * (jc_math_pi / 180.0f)))

// 라디안(radian) -> 도(degree) 변환
#define jc_math_rad2deg(_rad)	(((_rad) * (180.0f / jc_math_pi)))

NS_JC_BEGIN

struct Math final
{
	template <typename T>
	static T Pow(T _base, const int _exponent)
	{
		if (_exponent == 0)
		{
			return 1;
		}

		T mul = _base;

		for (int i = 1; i < _exponent; i++)
		{
			_base *= mul;
		}

		return _base;
	}

	template <typename T>
	static constexpr T Max(const T _lhs, const T _rhs)
	{
		return _lhs > _rhs ? _lhs : _rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Max(const T _arg0, const Args ... _args)
	{
		return Max(_arg0, Max(_args...));
	}

	template <typename T>
	static constexpr T Min(const T _lhs, const T _rhs)
	{
		return _lhs < _rhs ? _lhs : _rhs;
	}

	template <typename T, typename... Args>
	static constexpr T Min(const T _arg0, const Args ... _args)
	{
		return Min(_arg0, Min(_args...));
	}

	template <typename T>
	static constexpr T Abs(const T _arg)
	{
		return _arg < 0 ? _arg * -1 : _arg;
	}

	template <typename T>
	static constexpr T Clamp(const T _value, const T _min, const T _max)
	{
		return _value < _min ? _min : (_value > _max ? _max : _value);
	}

};

// =====================================================================================
// 아래 내용은 sgf/Math/SgfMath.h에서 이관된 내용입니다. (2026-08-09)
// =====================================================================================
/*
 * sgf 엔진 수학 라이브러리 (헤더 온리) -> jc 프로젝트로 이관
 *
 * [설계 방침]
 *  - DirectXMath(XMMATRIX)는 SIMD 최적화 때문에 코드가 읽기 어렵다.
 *    학습 목적으로 모든 연산을 눈으로 따라갈 수 있도록 직접 구현한다.
 *  - 행렬은 "행 우선(row-major)" 저장 + "행 벡터(row-vector)" 규약을 사용한다.
 *    즉, 벡터 v를 변환할 때 v' = v * M 순서로 곱한다. (DirectXMath와 동일)
 *    이동(Translation) 성분은 행렬의 마지막 행(m[3][0..2])에 들어간다.
 *  - HLSL 쪽 cbuffer에서는 row_major 키워드를 선언해서 CPU 메모리 배치와
 *    그대로 일치시킨다. 그래서 전치(Transpose) 없이 그대로 memcpy 해도 된다.
 *    (많은 튜토리얼이 여기서 헷갈리는데, 규약을 하나로 통일하면 헷갈릴 일이 없다)
 *
 * [좀 더 자세한 이론 설명은 Documents/03_선형대수.md 문서 참고]
 *
 * [비고]
 *  - 상수(Pi)/단위변환(Deg2Rad 등)은 위의 jc_math_* 매크로로 제공한다.
 *  - Epsilon_v(허용 오차)는 아래의 constexpr로 유지한다. (매크로 전환 대상 아님)
 */

// 부동소수점 비교용 허용 오차
constexpr _f32 Epsilon_v	= 1e-6f;

// 두 부동소수점이 거의 같은지 비교한다.
// 부동소수점은 2진수로 저장되기 때문에 0.1 + 0.2 != 0.3 같은 오차가 생긴다.
// 그래서 == 대신 "차이가 충분히 작은가"로 비교해야 한다.
inline bool FloatEqual(_f32 _a, _f32 _b, _f32 _tolerance = Epsilon_v)
{
	return std::fabs(_a - _b) <= _tolerance;
}

// 선형 보간(Linear Interpolation)
// _t = 0이면 _a, _t = 1이면 _b, _t = 0.5면 중간값을 반환한다.
// 애니메이션/카메라 이동 등 "부드럽게 변형는 값"이 필요한 모든 곳에 쓰인다.
inline _f32 Lerp(_f32 _a, _f32 _b, _f32 _t) { return _a + (_b - _a) * _t; }

// 값을 [_min, _max] 범위로 자른다.
inline _f32 Clamp(_f32 _value, _f32 _min, _f32 _max)
{
	if (_value < _min) return _min;
	if (_value > _max) return _max;
	return _value;
}

// =====================================================================================
// vec2 : 2차원 벡터
// =====================================================================================
// 2D 게임에서 위치, 크기, 속도, 방향을 표현하는 가장 기본적인 타입.
//
// [벡터란?]
//  - 크기와 방향을 동시에 가지는 값이다. (x, y) 두 숫자의 묶음.
//  - "위치"로 쓰면 원점에서 얼마나 떨어져 있는지,
//    "방향"으로 쓰면 어느 쪽을 바라보는지를 나타낸다.
// =====================================================================================
struct vec2
{
	_f32 x, y;

	// 기본 생성자: (0, 0)으로 초기화
	constexpr vec2() : x(0.0f), y(0.0f) {}

	// 성분을 직접 지정하는 생성자
	constexpr vec2(_f32 _x, _f32 _y) : x(_x), y(_y) {}

	// ---------------------------------------------------------------------------------
	// 사칙연산 연산자
	// 벡터끼리의 덧셈/뺄셈은 각 성분끼리 계산한다. (평행이동 개념)
	// 스칼라(숫자 하나) 곱셈은 벡터의 길이를 늘리거나 줄인다.
	// ---------------------------------------------------------------------------------
	constexpr vec2 operator+(const vec2& _other) const { return vec2(x + _other.x, y + _other.y); }
	constexpr vec2 operator-(const vec2& _other) const { return vec2(x - _other.x, y - _other.y); }
	constexpr vec2 operator*(_f32 _scalar) const { return vec2(x * _scalar, y * _scalar); }
	constexpr vec2 operator/(_f32 _scalar) const { return vec2(x / _scalar, y / _scalar); }
	constexpr vec2 operator-() const { return vec2(-x, -y); }

	vec2& operator+=(const vec2& _other) { x += _other.x; y += _other.y; return *this; }
	vec2& operator-=(const vec2& _other) { x -= _other.x; y -= _other.y; return *this; }
	vec2& operator*=(_f32 _scalar) { x *= _scalar; y *= _scalar; return *this; }

	bool operator==(const vec2& _other) const { return FloatEqual(x, _other.x) && FloatEqual(y, _other.y); }
	bool operator!=(const vec2& _other) const { return !(*this == _other); }

	// 벡터의 길이(크기)를 구한다.
	// 피타고라스 정리: 빗변의 길이 = sqrt(x² + y²)
	_f32 Length() const { return std::sqrt(x * x + y * y); }

	// 길이의 제곱을 구한다.
	// sqrt 연산은 느리기 때문에 "길이 비교"만 할 때는 제곱 상태로 비교하는 게 빠르다.
	// (a < b 이면 a² < b² 이므로 비교 결과가 같다)
	_f32 LengthSquared() const { return x * x + y * y; }

	// 정규화: 방향을 그대로 두고 길이를 1로 만든 벡터를 반환한다.
	// "방향만" 필요할 때 사용한다. (예: 이동 방향 * 속도)
	// 길이가 0이면 0으로 나누기가 되므로 영벡터를 반환한다.
	vec2 Normalized() const
	{
		const _f32 len = Length();
		if (len <= Epsilon_v)
		{
			return vec2(0.0f, 0.0f);
		}
		return vec2(x / len, y / len);
	}

	// 내적(Dot Product): a·b = a.x*b.x + a.y*b.y = |a||b|cosθ
	// [기하학적 의미]
	//  - 결과가 양수면 두 벡터가 같은 방향(90도 미만),
	//  - 0이면 수직하다. (90도), 음수면 반대 방향(90도 초과)이다.
	//  - "적이 내 앞에 있다 아니면 뒤에 있다" 같은 판정에 쓰인다.
	_f32 Dot(const vec2& _other) const { return x * _other.x + y * _other.y; }

	// 2D 외적(Cross Product의 z성분): a×b = a.x*b.y - a.y*b.x
	// [기하학적 의미]
	//  - 양수면 b가 a의 반시계 방향(왼쪽), 음수면 시계 방향(오른쪽)에 있다.
	//  절대값은 두 벡터가 만드는 평행사변형의 넓이다.
	_f32 Cross(const vec2& _other) const { return x * _other.y - y * _other.x; }

	// 두 점 사이의 거리
	_f32 Distance(const vec2& _other) const { return (*this - _other).Length(); }

	// 영벡터 상수
	static constexpr vec2 Zero() { return vec2(0.0f, 0.0f); }
	static constexpr vec2 One() { return vec2(1.0f, 1.0f); }
};

// 스칼라 * 벡터 순서도 허용 (2.0f * v 형태)
inline constexpr vec2 operator*(_f32 _scalar, const vec2& _v) { return _v * _scalar; }

// =====================================================================================
// vec3 : 3차원 벡터
// =====================================================================================
// 3D 위치/방향/법선(Normal)/색상(RGB) 등을 표현한다.
// =====================================================================================
struct vec3
{
	_f32 x, y, z;

	// 기본 생성자: (1,0, 0, 0)으로 초기화 / 원본: (0,0,0) 유지
	constexpr vec3() : x(0.0f), y(0.0f), z(0.0f) {}

	// 성분을 직접 지정하는 생성자
	constexpr vec3(_f32 _x, _f32 _y, _f32 _z) : x(_x), y(_y), z(_z) {}

	// 2D 벡터를 3D로 확장하는 생성자 (z는 별도 지정)
	constexpr vec3(const vec2& _xy, _f32 _z) : x(_xy.x), y(_xy.y), z(_z) {}

	constexpr vec3 operator+(const vec3& _other) const { return vec3(x + _other.x, y + _other.y, z + _other.z); }
	constexpr vec3 operator-(const vec3& _other) const { return vec3(x - _other.x, y - _other.y, z - _other.z); }
	constexpr vec3 operator*(_f32 _scalar) const { return vec3(x * _scalar, y * _scalar, z * _scalar); }
	constexpr vec3 operator/(_f32 _scalar) const { return vec3(x / _scalar, y / _scalar, z / _scalar); }
	constexpr vec3 operator-() const { return vec3(-x, -y, -z); }

	vec3& operator+=(const vec3& _other) { x += _other.x; y += _other.y; z += _other.z; return *this; }
	vec3& operator-=(const vec3& _other) { x -= _other.x; y -= _other.y; z -= _other.z; return *this; }
	vec3& operator*=(_f32 _scalar) { x *= _scalar; y *= _scalar; z *= _scalar; return *this; }

	bool operator==(const vec3& _other) const
	{
		return FloatEqual(x, _other.x) && FloatEqual(y, _other.y) && FloatEqual(z, _other.z);
	}
	bool operator!=(const vec3& _other) const { return !(*this == _other); }

	// 벡터의 길이: sqrt(x² + y² + z²)
	_f32 Length() const { return std::sqrt(x * x + y * y + z * z); }

	// 길이의 제곱 (비교 전용, sqrt 생략으로 빠름)
	_f32 LengthSquared() const { return x * x + y * y + z * z; }

	// 정규화: 길이를 1로 만든 벡터 반환. 방향 계산/조명 법선 등에 필수.
	vec3 Normalized() const
	{
		const _f32 len = Length();
		if (len <= Epsilon_v)
		{
			return vec3(0.0f, 0.0f, 0.0f);
		}
		return vec3(x / len, y / len, z / len);
	}

	// 내적: a·b = |a||b|cosθ
	// 조명 계산의 핵심! 법선·빛방향 = 표면이 빛을 얼마나 정면으로 받는지(밝기)를 준다.
	_f32 Dot(const vec3& _other) const { return x * _other.x + y * _other.y + z * _other.z; }

	// 외적(Cross Product): 두 벡터에 모두 수직인 벡터를 반환한다.
	// [용도]
	//  - 삼각형의 두 변으로 표면 법선(Normal) 구하기
	//  - 카메라의 Right/Up/Forward 직교 기저 만들기 (LookAt 행렬)
	// [순서 주의] a×b와 b×a는 방향이 반대다. DX는 왼손 좌표계이므로 왼손 규칙을 따른다.
	vec3 Cross(const vec3& _other) const
	{
		return vec3(
			y * _other.z - z * _other.y,
			z * _other.x - x * _other.z,
			x * _other.y - y * _other.x
		);
	}

	// 두 점 사이의 거리
	_f32 Distance(const vec3& _other) const { return (*this - _other).Length(); }

	static constexpr vec3 Zero() { return vec3(0.0f, 0.0f, 0.0f); }
	static constexpr vec3 One() { return vec3(1.0f, 1.0f, 1.0f); }
	static constexpr vec3 Up() { return vec3(0.0f, 1.0f, 0.0f); }		// +Y가 위 (DX 관례)
	static constexpr vec3 Right() { return vec3(1.0f, 0.0f, 0.0f); }	// +X가 오른쪽
	static constexpr vec3 Forward() { return vec3(0.0f, 0.0f, 1.0f); }	// +Z가 앞 (왼손 좌표계)
};

inline constexpr vec3 operator*(_f32 _scalar, const vec3& _v) { return _v * _scalar; }

// =====================================================================================
// vec4 : 4차원 벡터
// =====================================================================================
// [왜 3D 게임에서 4차원이 필요한가? - 동차 좌표(Homogeneous Coordinates)]
//  - 3x3 행렬로는 회전/크기는 표현해도 "이동"을 표현할 수 없다.
//  - 그래서 차원을 하나 늘리고 w=1을 붙여서 4x4 행렬 하나로
//    이동+회전+크기를 모두 표현한다.
//  - w=1 이면 "위치(point)" -> 이동의 영향만 가짐
//  - w=0 이면 "방향(vector)" -> 이동의 영향은 없음 (평행 이동해도 방향 유지)
//  - 원근 투영 후 w로 나누는 "원근 나눗셈"에도 쓰인다. (멀수록 작게 보임)
// =====================================================================================
struct vec4
{
	_f32 x, y, z, w;

	// 기본 생성자: (0, 0, 0, 0)
	constexpr vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

	// 성분 지정 생성자
	constexpr vec4(_f32 _x, _f32 _y, _f32 _z, _f32 _w) : x(_x), y(_y), z(_z), w(_w) {}

	// vec3 + w 생성자 (위치면 w=1, 방향이면 w=0을 넣는다)
	constexpr vec4(const vec3& _xyz, _f32 _w) : x(_xyz.x), y(_xyz.y), z(_xyz.z), w(_w) {}

	constexpr vec4 operator+(const vec4& _other) const { return vec4(x + _other.x, y + _other.y, z + _other.z, w + _other.w); }
	constexpr vec4 operator-(const vec4& _other) const { return vec4(x - _other.x, y - _other.y, z - _other.z, w - _other.w); }
	constexpr vec4 operator*(_f32 _scalar) const { return vec4(x * _scalar, y * _scalar, z * _scalar, w * _scalar); }

	// 내적
	_f32 Dot(const vec4& _other) const { return x * _other.x + y * _other.y + z * _other.z + w * _other.w; }

	// xyz 성분만 추출
	vec3 XYZ() const { return vec3(x, y, z); }
};

// =====================================================================================
// color : RGBA 색상 (각 성분 0.0 ~ 1.0)
// =====================================================================================
// [왜 0~255가 아니라 0.0~1.0인가?]
//  - GPU/셰이더는 색상을 부동소수점 0~1로 다룬다. 곱셈(조명 계산)이 자연스럽기 때문.
//  - 0~255 정수는 최종적으로 모니터에 출력될 때의 포맷일 뿐이다.
// =====================================================================================
struct color
{
	_f32 r, g, b, a;

	// 기본 생성자: 불투명 흰색
	constexpr color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

	// 성분 지정 생성자
	constexpr color(_f32 _r, _f32 _g, _f32 _b, _f32 _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {}

	// 0~255 정수 값으로부터 생성하는 헬퍼
	// 예: color::FromBytes(255, 128, 0) -> 주황색
	static color FromBytes(_u8 _r, _u8 _g, _u8 _b, _u8 _a = 255)
	{
		return color(_r / 255.0f, _g / 255.0f, _b / 255.0f, _a / 255.0f);
	}

	// 색상끼리 곱하기 (조명 계산, 틴트 효과에 사용)
	constexpr color operator*(const color& _other) const
	{
		return color(r * _other.r, g * _other.g, b * _other.b, a * _other.a);
	}

	constexpr color operator*(_f32 _scalar) const
	{
		return color(r * _scalar, g * _scalar, b * _scalar, a * _scalar);
	}

	// 자주 쓰는 색상 프리셋
	static constexpr color White() { return color(1.0f, 1.0f, 1.0f, 1.0f); }
	static constexpr color Black() { return color(0.0f, 0.0f, 0.0f, 1.0f); }
	static constexpr color Red() { return color(1.0f, 0.0f, 0.0f, 1.0f); }
	static constexpr color Green() { return color(0.0f, 1.0f, 0.0f, 1.0f); }
	static constexpr color Blue() { return color(0.0f, 0.0f, 1.0f, 1.0f); }
	static constexpr color Yellow() { return color(1.0f, 1.0f, 0.0f, 1.0f); }
	static constexpr color Cyan() { return color(0.0f, 1.0f, 1.0f, 1.0f); }
	static constexpr color Magenta() { return color(1.0f, 0.0f, 1.0f, 1.0f); }
	static constexpr color CornflowerBlue() { return color(0.39f, 0.58f, 0.93f, 1.0f); }	// XNA 전통의 하늘색
};

// =====================================================================================
// mat4 : 4x4 행렬
// =====================================================================================
// [행렬이란?]
//  - 숫자를 4x4 격자로 배열한 것. "좌표 내 변환기"라고 생각하면 된다.
//  - 벡터에 행렬을 곱하면 이동/회전/크기/투영 변환이 한번에 적용된다.
//  - 행렬 곱하면 "변환의 합성"이 된다. (World * View * Proj)
//
// [저장 방식: row-major, 규약: 행벡터 v' = v * M]
//  - m[행][열] 순서로 메모리에 배치한다. (m[0][0], m[0][1], ... 순서로 연속)
//  - 이동 성분은 4번째 행 m[3][0..2]에 위치한다.
//  - HLSL 상수 버퍼에 row_major로 선언하면 전치 없이 그대로 올려도 된다.
// =====================================================================================
struct mat4
{
	_f32 m[4][4];

	// 기본 생성자: 단위 행렬(Identity)로 초기화
	mat4()
	{
		SetIdentity();
	}

	// 단위 행렬로 만든다. 대각선만 1, 나머지는 0.
	// | 1 0 0 0 |
	// | 0 1 0 0 |
	// | 0 0 1 0 |
	// | 0 0 0 1 |
	void SetIdentity()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				m[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
	}

	// ---------------------------------------------------------------------------------
	// 행렬 곱셈: 변환의 합성
	// ---------------------------------------------------------------------------------
	// result = this * _other
	// 행벡터 규약에서는 v * (A * B) = (v * A) * B 이므로
	// "A 변환을 먼저 적용하고 B 변환을 나중에 적용"이 A * B로 표현된다.
	// 예: World = Scale * Rotation * Translation
	mat4 operator*(const mat4& _other) const
	{
		mat4 result;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result.m[i][j] =
					m[i][0] * _other.m[0][j] +
					m[i][1] * _other.m[1][j] +
					m[i][2] * _other.m[2][j] +
					m[i][3] * _other.m[3][j];
			}
		}
		return result;
	}

	// 점(Point) 변환: w=1로 취급하여 이동까지 적용된다.
	vec3 TransformPoint(const vec3& _point) const
	{
		const _f32 x = _point.x * m[0][0] + _point.y * m[1][0] + _point.z * m[2][0] + m[3][0];
		const _f32 y = _point.x * m[0][1] + _point.y * m[1][1] + _point.z * m[2][1] + m[3][1];
		const _f32 z = _point.x * m[0][2] + _point.y * m[1][2] + _point.z * m[2][2] + m[3][2];
		return vec3(x, y, z);
	}

	// 방향(Vector) 변환: 이동을 무시하고 회전/크기만 적용된다.
	vec3 TransformVector(const vec3& _vector) const
	{
		const _f32 x = _vector.x * m[0][0] + _vector.y * m[1][0] + _vector.z * m[2][0];
		const _f32 y = _vector.x * m[0][1] + _vector.y * m[1][1] + _vector.z * m[2][1];
		const _f32 z = _vector.x * m[0][2] + _vector.y * m[1][2] + _vector.z * m[2][2];
		return vec3(x, y, z);
	}

	// vec4 완전 변환 (w 포함. 투영 행렬 결과 확인용)
	vec4 Transform(const vec4& _v) const
	{
		return vec4(
			_v.x * m[0][0] + _v.y * m[1][0] + _v.z * m[2][0] + _v.w * m[3][0],
			_v.x * m[0][1] + _v.y * m[1][1] + _v.z * m[2][1] + _v.w * m[3][1],
			_v.x * m[0][2] + _v.y * m[1][2] + _v.z * m[2][2] + _v.w * m[3][2],
			_v.x * m[0][3] + _v.y * m[1][3] + _v.z * m[2][3] + _v.w * m[3][3]
		);
	}

	// 전치 행렬: 행과 열을 바꿔치기
	// (열벡터 규약 라이브러리와 데이터를 주고받을 때 필요)
	mat4 Transposed() const
	{
		mat4 result;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				result.m[i][j] = m[j][i];
			}
		}
		return result;
	}

	// ---------------------------------------------------------------------------------
	// 이동 행렬 (Translation)
	// ---------------------------------------------------------------------------------
	// | 1  0  0  0 |
	// | 0  1  0  0 |
	// | 0  0  1  0 |
	// | tx ty tz 1 |   <- 행벡터 규약에서는 이동이 마지막 "행"에 위치
	static mat4 Translation(_f32 _x, _f32 _y, _f32 _z)
	{
		mat4 result;			// 단위 행렬로 시작
		result.m[3][0] = _x;
		result.m[3][1] = _y;
		result.m[3][2] = _z;
		return result;
	}
	static mat4 Translation(const vec3& _pos)
	{
		return Translation(_pos.x, _pos.y, _pos.z);
	}

	// ---------------------------------------------------------------------------------
	// 크기 행렬 (Scale)
	// ---------------------------------------------------------------------------------
	// | sx 0  0  0 |
	// | 0  sy 0  0 |
	// | 0  0  sz 0 |
	// | 0  0  0  1 |
	static mat4 Scale(_f32 _x, _f32 _y, _f32 _z)
	{
		mat4 result;
		result.m[0][0] = _x;
		result.m[1][1] = _y;
		result.m[2][2] = _z;
		return result;
	}
	static mat4 Scale(_f32 _uniform)
	{
		return Scale(_uniform, _uniform, _uniform);
	}

	// ---------------------------------------------------------------------------------
	// 회전 행렬 (Rotation) - 각 축 기준, 라디안 단위
	// ---------------------------------------------------------------------------------
	static mat4 RotationZ(_f32 _radian)
	{
		mat4 result;
		const _f32 c = std::cos(_radian);
		const _f32 s = std::sin(_radian);
		result.m[0][0] = c;  result.m[0][1] = s;
		result.m[1][0] = -s; result.m[1][1] = c;
		return result;
	}

	static mat4 RotationX(_f32 _radian)
	{
		mat4 result;
		const _f32 c = std::cos(_radian);
		const _f32 s = std::sin(_radian);
		result.m[1][1] = c;  result.m[1][2] = s;
		result.m[2][1] = -s; result.m[2][2] = c;
		return result;
	}

	static mat4 RotationY(_f32 _radian)
	{
		mat4 result;
		const _f32 c = std::cos(_radian);
		const _f32 s = std::sin(_radian);
		result.m[0][0] = c;  result.m[0][2] = -s;
		result.m[2][0] = s;  result.m[2][2] = c;
		return result;
	}

	// ---------------------------------------------------------------------------------
	// SRT 합성 헬퍼: Scale -> Rotation(Z) -> Translation 순서로 합성된 월드 행렬
	// ---------------------------------------------------------------------------------
	static mat4 SRT2D(const vec2& _scale, _f32 _rotationRad, const vec2& _position)
	{
		return Scale(_scale.x, _scale.y, 1.0f) * RotationZ(_rotationRad) * Translation(_position.x, _position.y, 0.0f);
	}

	// ---------------------------------------------------------------------------------
	// 뷰 행렬 (View Matrix) - 왼손 좌표계 LookAt
	// ---------------------------------------------------------------------------------
	static mat4 LookAtLH(const vec3& _eye, const vec3& _target, const vec3& _up)
	{
		const vec3 zAxis = (_target - _eye).Normalized();		// 카메라가 바라보는 방향
		const vec3 xAxis = _up.Cross(zAxis).Normalized();		// 카메라의 오른쪽 방향
		const vec3 yAxis = zAxis.Cross(xAxis);					// 카메라의 위 방향

		mat4 result;
		result.m[0][0] = xAxis.x; result.m[0][1] = yAxis.x; result.m[0][2] = zAxis.x;
		result.m[1][0] = xAxis.y; result.m[1][1] = yAxis.y; result.m[1][2] = zAxis.y;
		result.m[2][0] = xAxis.z; result.m[2][1] = yAxis.z; result.m[2][2] = zAxis.z;
		result.m[3][0] = -xAxis.Dot(_eye);
		result.m[3][1] = -yAxis.Dot(_eye);
		result.m[3][2] = -zAxis.Dot(_eye);
		return result;
	}

	// ---------------------------------------------------------------------------------
	// 원근 투영 행렬 (Perspective Projection) - 왼손 좌표계
	// ---------------------------------------------------------------------------------
	static mat4 PerspectiveFovLH(_f32 _fovY, _f32 _aspect, _f32 _nearZ, _f32 _farZ)
	{
		if (_nearZ <= 0.0f || _farZ <= _nearZ || _aspect <= 0.0f ||
			_fovY <= 0.0f || _fovY >= jc_math_pi)
		{
			return Identity();
		}

		mat4 result;
		const _f32 yScale = 1.0f / std::tan(_fovY * 0.5f);
		const _f32 xScale = yScale / _aspect;
		const _f32 zRange = _farZ / (_farZ - _nearZ);

		result.m[0][0] = xScale;
		result.m[1][1] = yScale;
		result.m[2][2] = zRange;			result.m[2][3] = 1.0f;
		result.m[3][2] = -_nearZ * zRange;	result.m[3][3] = 0.0f;
		return result;
	}

	// ---------------------------------------------------------------------------------
	// 직교 투영 행렬 (Perspective Projection) - 왼손 좌표계
	// ---------------------------------------------------------------------------------
	static mat4 OrthographicOffCenterLH(_f32 _left, _f32 _right, _f32 _bottom, _f32 _top, _f32 _nearZ, _f32 _farZ)
	{
		if (_right <= _left || _top <= _bottom || _farZ <= _nearZ)
		{
			return Identity();
		}

		mat4 result;
		result.m[0][0] = 2.0f / (_right - _left);
		result.m[1][1] = 2.0f / (_top - _bottom);
		result.m[2][2] = 1.0f / (_farZ - _nearZ);
		result.m[3][0] = (_left + _right) / (_left - _right);
		result.m[3][1] = (_top + _bottom) / (_bottom - _top);
		result.m[3][2] = _nearZ / (_nearZ - _farZ);
		return result;
	}

	// 화면 중앙이 원점이고 y가 위쪽인 2D 카메라용 직교 투영 (폭/높이 지정)
	static mat4 Orthographic2D(_f32 _width, _f32 _height)
	{
		return OrthographicOffCenterLH(-_width * 0.5f, _width * 0.5f, -_height * 0.5f, _height * 0.5f, 0.0f, 1.0f);
	}

	// 단위 행렬 반환
	static mat4 Identity()
	{
		return mat4();
	}
};

NS_END