/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:00:00 AM
 * =====================
 * 12. SVG 텍스처 그리기 - 보조 함수 구현부
 */

#include "Core.h"
#include "sgfr/12_SvgTextureDraw/12_SvgTextureDraw_Function.h"

using namespace sgf;
using namespace jc;

// 벡터 그래픽 vs 래스터 그래픽 차이를 콘솔에 출력한다. (학습용)
void PrintVectorVsRasterExplanation()
{
	printf("\n[래스터 그래픽 (PNG/JPG)]\n");
	printf(" 픽셀 격자에 색을 저장. 확대하면 계단 현상(픽셀 깨짐)이 생긴다.\n");
	printf("\n[벡터 그래픽 (SVG)]\n");
	printf(" '중심 (100,100)에 반지름 80짜리 빨간 원' 처럼 도형 명령어를 저장.\n");
	printf(" 아무리 확대해도 다시 계산해서 그리므로 항상 깨끗하다!\n");
	printf("\n[GPU는 벡터를 모른다?]\n");
	printf(" GPU 텍스처는 결국 픽셀 격자다. 그래서 SVG를 쓰려면\n");
	printf(" '래스터화(Rasterize)' 과정이 필요하다: SVG -> [nanosvg 파싱] ->\n");
	printf(" [nanosvgrast 래스터화] -> RGBA 픽셀 -> GPU 텍스처.\n");
	printf(" 원하는 크기로 래스터화할 수 있는 것이 SVG의 최대 장점! (scale 인자)\n\n");
}

// 예제 SVG 파일(sample.svg, 실행 팩더 기준)이 없으면 생성해준다.
// 반환값: 파일이 존재하거나 생성에 성공하면 true
bool EnsureSampleSvgFile(const char* _szFilePath)
{
	// 이미 존재하는지 확인 (읽기 모드로 열어본다)
	FILE* pFile = nullptr;
	if (fopen_s(&pFile, _szFilePath, "r") == 0 && pFile != nullptr)
	{
		fclose(pFile);
		return true;
	}

	// 없으면 간단한 SVG를 직접 작성한다.
	// SVG는 사람이 읽을 수 있는 텍스트(XML)라는 것을 보여주는 예제이기도 하다!
	if (fopen_s(&pFile, _szFilePath, "w") != 0 || pFile == nullptr)
	{
		return false;	// Resources 폴더가 없는 경우 등
	}

	fprintf(pFile,
		"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"256\" height=\"256\">\n"
		"  <rect x=\"16\" y=\"16\" width=\"224\" height=\"224\" rx=\"32\" fill=\"#2d6cdf\"/>\n"
		"  <circle cx=\"128\" cy=\"128\" r=\"72\" fill=\"#ffd166\"/>\n"
		"  <circle cx=\"104\" cy=\"112\" r=\"12\" fill=\"#333333\"/>\n"
		"  <circle cx=\"152\" cy=\"112\" r=\"12\" fill=\"#333333\"/>\n"
		"  <path d=\"M 96 152 Q 128 184 160 152\" stroke=\"#333333\" stroke-width=\"8\" fill=\"none\"/>\n"
		"</svg>\n");
	fclose(pFile);
	return true;
}

// nanosvg가 없을 때 대체용으로 그라데이션 원 텍스처를 직접 만든다.
// (SVG 래스터화 결과와 비슷한 부드러운 원을 CPU로 직접 그린다)
bool CreateFallbackCircleTexture(GraphicDevice* _pDevice, Texture* _pOutTexture)
{
	const int size = 256;
	const _f32 center = size * 0.5f;
	const _f32 radius = size * 0.35f;

	jc::Vector<_u8> pixels;
	pixels.Resize(size * size * 4);

	for (int y = 0; y < size; ++y)
	{
		for (int x = 0; x < size; ++x)
		{
			// 픽셀 중심에서 원 중심까지의 거리
			const _f32 dx = (x + 0.5f) - center;
			const _f32 dy = (y + 0.5f) - center;
			const _f32 dist = sqrtf(dx * dx + dy * dy);

			// 원 경계에서 2픽셀 정도 부드럽게 알파를 감쇠시킨다.
			// 이것이 바로 '안티에일리어싱'의 기본 아이디어다!
			_f32 alpha = Clamp((radius - dist) / 2.0f, 0.0f, 1.0f);

			const int idx = (y * size + x) * 4;
			pixels[idx + 0] = (_u8)(255.0f * alpha);	// R (노란 원)
			pixels[idx + 1] = (_u8)(209.0f * alpha);	// G
			pixels[idx + 2] = (_u8)(102.0f * alpha);	// B
			pixels[idx + 3] = (_u8)(255.0f * alpha);	// A (경계는 반투명)
		}
	}

	return _pOutTexture->CreateFromMemory(_pDevice, pixels.Source(), size, size);
}
