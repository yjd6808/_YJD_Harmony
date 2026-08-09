/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:00:00 PM
 * =====================
 * 17. 래스터라이저 스테이트 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/17_RasterizerState/17_RasterizerState_Function.h"

using namespace sgf;

// 래스터라이저 단계의 역할과 컬링/와이어프레임의 원리를 콘솔에 출력한다. (학습용)
void PrintRasterizerExplanation()
{
	printf("\n[래스터라이저란?]\n");
	printf(" 삼각형을 화면 픽셀로 바꾸는 파이프라인 고정 단계입니다.\n");
	printf(" 이 단계의 옵션(스테이트)을 바꾸면 그리는 방식이 확 달라집니다.\n\n");
	printf("[채우기 모드 (W 키로 토글)]\n");
	printf(" Solid     : 삼각형 내부를 꽉 채운다 (기본)\n");
	printf(" Wireframe : 모서리 선만 그린다  (메쉬 구조 디버깅에 필수!)\n\n");
	printf("[컬링 (1/2/3 키로 전환)] - 어느 쪽 면을 버릴것인가?\n");
	printf(" 1. None  : 양면 모두 그린다       (2배로 비싸지만 종이처럼 얇은 물체에 필요)\n");
	printf(" 2. Back  : 뒷면을 버린다          (기본! 닫힌 물체의 뒷면은 어차피 안 보인다)\n");
	printf(" 3. Front : 앞면을 버린다          (큐브 속이 뒤집어 보이는 특이한 모습!)\n\n");
	printf(" 컬링 판정 기준: 화면에 투영된 삼각형의 정점 순서(우리 엔진은 시계 방향 = 앞면)\n");
	printf(" 그래서 인덱스를 지정하는 순서가 중요합니다!\n\n");
}
