/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:50:00 AM
 * =====================
 * 15. PNG 텍스처 그리기 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/15_PngTextureDraw/15_PngTextureDraw_Function.h"

using namespace sgf;

// UV 좌표계의 원리를 콘솔에 출력한다. (학습용)
void PrintUvExplanation()
{
	printf("\n[UV 좌표계]\n");
	printf(" 텍스처 위의 위치를 나타내는 0~1 좌표계.\n");
	printf(" (0,0) = 왼쪽 위, (1,1) = 오른쪽 아래. 해상도와 무관하다!\n");
	printf(" 정점에 UV를 적어두면 래스터라이저가 픽셀마다 보간해서 전달한다.\n");
	printf(" 픽셀 셰이더는 그 UV로 텍스처 색을 읽는다(Sample).\n\n");
}

// 체커보드 무늬 픽셀(RGBA)을 직접 만들어 텍스처를 생성한다.
// PNG 파일이 없을 때도 튜토리얼이 항상 동작하도록 하는 대체 경로다.
bool CreateCheckerboardTexture(GraphicDevice* _pDevice, Texture* _pOutTexture)
{
	const int size = 256;		// 256 x 256 픽셀
	const int cell = 32;		// 한 칸 32픽셀

	// RGBA 8비트 x 4채널 = 픽셀당 4바이트
	jc::Vector<_u8> pixels;
	pixels.Resize(size * size * 4);

	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			// 체커보드 판정: 칸 좌표의 합이 짝수면 밝은 칸, 홀수면 어두운 칸
			const bool bLight = (((x / cell) + (y / cell)) % 2) == 0;

			const int idx = (y * size + x) * 4;
			pixels[idx + 0] = bLight ? 235 : 60;	// R
			pixels[idx + 1] = bLight ? 235 : 120;	// G
			pixels[idx + 2] = bLight ? 235 : 200;	// B
			pixels[idx + 3] = 255;					// A (불투명)
		}
	}

	return _pOutTexture->CreateFromMemory(_pDevice, pixels.Source(), size, size);
}
