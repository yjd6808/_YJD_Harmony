/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:10:00 AM
 * =====================
 * 01. 선형대수 (Linear Algebra)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 벡터(Vector)   : 크기와 방향을 가진 화살표. 게임에서는 위치/이동/방향으로 사용.
 *  2. 내적(Dot)      : 두 벡터가 얼마나 같은 방향을 보는지. (조명 계산, 시야 판정의 핵심)
 *  3. 외적(Cross)    : 두 벡터에 수직인 벡터를 구함. (법선 계산, 좌/우 판정의 핵심)
 *  4. 행렬(Matrix)   : 이동/회전/크기 변환을 하나로 합치는 "좌표 변환 기계".
 *  5. SRT 순서      : 왜 Scale -> Rotation -> Translation 순서로 곱하는지.
 *
 * [우리 엔진의 규약 - 꼭 기억할 것]
 *  - row-major 저장 + 행벡터 규약: v' = v * M
 *  - 그래서 변환을 순서대로 적용하려면 "왼쪽부터" 순서대로 곱한다.
 *    예) 먼저 크기 -> 회전 -> 이동이면  M = S * R * T
 *  - 이동 성분은 행렬의 4번째 행(m[3][0..2])에 저장된다.
 */

#include "Core.h"
#include "sgfr/01_LinearAlgebra/01_LinearAlgebra_Main.h"
#include "sgfr/01_LinearAlgebra/01_LinearAlgebra_Function.h"

using namespace sgf;
using namespace jc;

// 선형대수 튜토리얼을 실행한다. (콘솔 출력 전용, 윈도우 없음)
void LinearAlgebra_Main()
{
	// =================================================================
	// 1. 벡터 기초 연산
	// =================================================================
	printf("\n--- 1. 벡터 기초 ---\n");

	const Vec2 a(3.0f, 4.0f);
	const Vec2 b(1.0f, 0.0f);

	PrintVec2("a", a);
	PrintVec2("b", b);
	PrintVec2("a + b", a + b);				// 성분끼리 더하기 = 화살표 이어붙이기
	PrintVec2("a - b", a - b);				// b에서 a로 향하는 벡터
	PrintVec2("a * 2", a * 2.0f);			// 길이만 2배 (방향 그대로)

	// 길이: 피타고라스 정리. (3, 4)의 길이는 sqrt(9+16) = 5
	printf("a.Length()            = %.3f (기대값: 5)\n", a.Length());

	// 정규화: 방향은 그대로, 길이만 1로 만든 벡터. "순수한 방향"이 필요할 때 사용.
	PrintVec2("a.Normalized()", a.Normalized());

	// =================================================================
	// 2. 내적 (Dot Product)
	// =================================================================
	// 공식: a.Dot(b) = |a||b|cos(theta)
	// 내적의 부호만 봐도 많은 걸 안다:
	//  > 0 : 두 벡터가 같은 방향 (90도 미만)
	//  = 0 : 서로 수직
	//  < 0 : 반대 방향 (90도 초과)
	// 게임 활용: "적이 내 앞에 있나 뒤에 있나?" = 앞방벡터.Dot(적방향)의 부호
	// =================================================================
	printf("\n--- 2. 내적 ---\n");

	const Vec2 right(1.0f, 0.0f);
	const Vec2 up(0.0f, 1.0f);
	const Vec2 diag = Vec2(1.0f, 1.0f).Normalized();

	printf("right.Dot(right)      = %.3f (같은 방향: +1)\n", right.Dot(right));
	printf("right.Dot(up)         = %.3f (수직: 0)\n", right.Dot(up));
	printf("right.Dot(-right)     = %.3f (반대: -1)\n", right.Dot(-right));
	printf("right.Dot(diag)       = %.3f (45도: cos45 = 0.707)\n", right.Dot(diag));

	// =================================================================
	// 3. 외적 (Cross Product)
	// =================================================================
	// 3D 외적: 두 벡터에 모두 수직인 벡터를 만든다. (평면의 법선 계산)
	// 왼손 좌표계(DX)에서 X cross Y = Z 방향이 나온다.
	// 2D 외적(스칼라): 부호로 "b가 a의 왼쪽이냐 오른쪽이냐"를 판정할 수 있다.
	// =================================================================
	printf("\n--- 3. 외적 ---\n");

	const Vec3 xAxis = Vec3::Right();
	const Vec3 yAxis = Vec3::Up();
	PrintVec3("X cross Y", xAxis.Cross(yAxis));		// (0, 0, 1) = Z축

	printf("right cross up (2D)   = %.3f (양수: up은 right의 반시계 방향)\n", right.Cross(up));

	// =================================================================
	// 4. 행렬 기초
	// =================================================================
	printf("\n--- 4. 행렬 기초 ---\n");

	PrintMat4("Identity (무변환)", Mat4::Identity());
	PrintMat4("Translation(10, 20, 0)", Mat4::Translation(10.0f, 20.0f, 0.0f));
	PrintMat4("Scale(2, 3, 1)", Mat4::Scale(2.0f, 3.0f, 1.0f));
	PrintMat4("RotationZ(90도)", Mat4::RotationZ(jc_math_deg2rad(90.0f)));

	// 점 변환 확인: (1, 0, 0)을 Z축 90도 회전하면 (0, 1, 0)이 되어야 한다.
	const Vec3 p(1.0f, 0.0f, 0.0f);
	PrintVec3("(1,0,0)을 Z축 90도 회전", Mat4::RotationZ(jc_math_deg2rad(90.0f)).TransformPoint(p));

	// =================================================================
	// 5. 변환 합성과 순서의 중요성
	// =================================================================
	// "회전 후 이동"과 "이동 후 회전"은 완전히 다른 결과를 낸다!
	//  - 회전 후 이동: 제자리에서 돌고 나서 옮김   -> 보통 원하는 결과
	//  - 이동 후 회전: 원점 기준으로 크게 공전함   -> 의도치 않은 결과
	// =================================================================
	printf("\n--- 5. 변환 순서 ---\n");

	const Mat4 r = Mat4::RotationZ(jc_math_deg2rad(90.0f));
	const Mat4 t = Mat4::Translation(10.0f, 0.0f, 0.0f);

	// 행벡터 규약: 왼쪽에 있는 행렬이 먼저 적용된다.
	PrintVec3("회전 -> 이동 (R*T)", (r * t).TransformPoint(p));	// (10, 1, 0) 부근
	PrintVec3("이동 -> 회전 (T*R)", (t * r).TransformPoint(p));	// (0, 11, 0) 부근

	// SRT2D는 Scale * RotationZ * Translation을 한번에 만들어주는 헬퍼다.
	// 씬 그래프의 모든 Node가 이 함수로 로컬 변환을 만든다.
	PrintMat4("SRT2D(배쀨1, 90도, (10,0))", Mat4::SRT2D(Vec2::One(), jc_math_deg2rad(90.0f), Vec2(10.0f, 0.0f)));

	printf("\n[정리]\n");
	printf(" - 벡터 = 크기 + 방향, 내적 = 방향 유사도, 외적 = 수직 벡터\n");
	printf(" - 행렬 = 좌표 변환 기계, 곱셈 = 변환 합성 (순서 중요!)\n");
	printf(" - 우리 엔진은 v' = v * M 규약이라 왼쪽 행렬부터 적용된다\n");
}
