/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:10:00 AM
 * =====================
 * 01. 선형대수 - 보조 함수 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/01_LinearAlgebra/01_LinearAlgebra_Function.h"

// Vec2를 "이름: (x, y)" 형태로 콘솔에 출력한다.
void PrintVec2(const char* _szName, const jc::vec2& _v)
{
	printf("%-24s = (%8.3f, %8.3f)\n", _szName, _v.x, _v.y);
}

// Vec3를 "이름: (x, y, z)" 형태로 콘솔에 출력한다.
void PrintVec3(const char* _szName, const jc::vec3& _v)
{
	printf("%-24s = (%8.3f, %8.3f, %8.3f)\n", _szName, _v.x, _v.y, _v.z);
}

// Mat4를 4x4 격자 형태로 콘솔에 출력한다.
// m[행][열] 순서이므로 한 줄이 행렬의 한 행(row)이다.
// 이동 성분은 4번째 행(m[3][0..2])에서 확인할 수 있다.
void PrintMat4(const char* _szName, const jc::mat4& _m)
{
	printf("%s =\n", _szName);
	for (int row = 0; row < 4; ++row)
	{
		printf("  | %8.3f %8.3f %8.3f %8.3f |\n",
			_m.m[row][0], _m.m[row][1], _m.m[row][2], _m.m[row][3]);
	}
}
