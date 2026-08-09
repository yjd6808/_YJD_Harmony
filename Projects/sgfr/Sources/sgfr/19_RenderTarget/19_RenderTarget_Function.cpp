/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:20:00 PM
 * =====================
 * 19. 렌더 타깃 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/19_RenderTarget/19_RenderTarget_Function.h"

using namespace sgf;
using namespace jc;

// 렌더 타깃의 개념과 활용처를 콘솔에 출력한다. (학습용)
void PrintRenderTargetExplanation()
{
	printf("\n[렌더 타깃(Render Target)이란?]\n");
	printf(" 지금까지는 항상 "); printf("'백버퍼(화면)'에만 그렸습니다.\n");
	printf(" 렌더 타깃을 쓰면 '내가 만든 텍스처'에 그릴 수 있습니다.\n");
	printf(" 그려진 결과는 그냥 텍스처이므로 다시 샘플링해서 화면에 붙일 수 있죠!\n\n");
	printf("[활용처 - 현대 엔진의 핵심 기술!]\n");
	printf(" - 미니맵/백미러/감시카메라: 다른 시점으로 한 번 더 그려서 작게 표시\n");
	printf(" - 포스트 프로세싱(20번): 화면 전체를 텍스처로 받아 후보정 효과\n");
	printf(" - 그림자 매핑(21번): 빛 시점의 깊이를 텍스처에 저장\n\n");
	printf("[이 화면]\n");
	printf(" 가운데: 카메라로 본 큐브 (백버퍼에 직접 그림)\n");
	printf(" 오른쪽 위: 같은 큐브를 '바로 위에서' 내려다본 미니맵 (렌더 타깃에 그림)\n\n");
}

// NDC 좌표기준 텍스처 출력용 사각형 정점 4개를 채운다. (UV 0~1, 흰색)
void FillScreenQuad(VertexPTC* _pOutVertices4, const vec2& _center, const vec2& _halfSize)
{
	const color white = color::White();

	// 왕위 -> 오른위 -> 왼아래 -> 오른아래 (인덱스 0,1,2 / 2,1,3과 짝)
	_pOutVertices4[0] = { vec3(_center.x - _halfSize.x, _center.y + _halfSize.y, 0.0f), vec2(0.0f, 0.0f), white };
	_pOutVertices4[1] = { vec3(_center.x + _halfSize.x, _center.y + _halfSize.y, 0.0f), vec2(1.0f, 0.0f), white };
	_pOutVertices4[2] = { vec3(_center.x - _halfSize.x, _center.y - _halfSize.y, 0.0f), vec2(0.0f, 1.0f), white };
	_pOutVertices4[3] = { vec3(_center.x + _halfSize.x, _center.y - _halfSize.y, 0.0f), vec2(1.0f, 1.0f), white };
}
