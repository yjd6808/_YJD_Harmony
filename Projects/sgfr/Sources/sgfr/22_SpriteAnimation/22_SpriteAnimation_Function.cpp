/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:50:00 PM
 * =====================
 * 22. 스프라이트 애니메이션 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/22_SpriteAnimation/22_SpriteAnimation_Function.h"

#include <math.h>

using namespace sgf;
using namespace jc;

// 스프라이트 시트/프레임 애니메이션/이징의 원리를 콘솔에 출력한다. (학습용)
void PrintSpriteAnimationExplanation()
{
	printf("\n[스프라이트 시트(Sprite Sheet)란?]\n");
	printf(" 애니메이션 프레임들을 한 장의 큰 텍스처에 바둑판처럼 모아둔 것.\n");
	printf(" 텍스처 교체 없이 UV 오프셋만 바꿔서 프레임을 넘기므로 매우 빠르다!\n");
	printf(" (이 튜토리얼은 256x256 시트에 64x64 프레임 16개를 코드로 직접 그려 사용한다)\n\n");
	printf("[프레임 선택 공식]\n");
	printf(" frame = (int)(누적시간 x 초당프레임수) %% 전체프레임수\n");
	printf(" uvOffset = (열 x 0.25, 행 x 0.25), uvScale = 0.25 (4x4 시트)\n\n");
	printf("[이징(Easing)이란?]\n");
	printf(" 이동 진행도 t(0~1)를 그대로 쓰면(선형) 기계적으로 보인다.\n");
	printf(" t를 곡선 함수에 통과시키면 가속/감속/통통 튀는 느낌이 난다.\n");
	printf(" Cocos2d-x의 EaseInOut/EaseBounce 액션도 정확히 이 원리다!\n\n");
	printf("[조작법]\n");
	printf(" 1: 선형 / 2: 부드럽게(ease-in-out) / 3: 바운스\n");
	printf(" 위/아래 방향키: 초당 프레임 수(FPS) 증감 (4~60)\n\n");
}

// 256x256 RGBA 버퍼에 4x4 = 16프레임짜리 스프라이트 시트를 절차적으로 그려 채운다.
void FillSpriteSheetPixels(_u8* _pOutRgba)
{
	const int sheetSize = 256;		// 시트 한 변 픽셀 수
	const int frameSize = 64;		// 프레임 한 변 픽셀 수
	const int framesPerRow = 4;		// 한 줄에 4프레임
	const int frameCount = 16;		// 전체 16프레임

	// 전체를 먼저 완전 투명으로 초기화
	memset(_pOutRgba, 0, sheetSize * sheetSize * 4);

	for (int frame = 0; frame < frameCount; ++frame)
	{
		const int frameCol = frame % framesPerRow;	// 시트에서의 열
		const int frameRow = frame / framesPerRow;	// 시트에서의 행
		const _f32 progress = (_f32)frame / (_f32)frameCount;	// 0~1 애니메이션 진행도

		// 프레임마다 원 반지름이 사인파로 커졌다 작아졌다 한다. (0.3 ± 0.15, 프레임 좌표 기준)
		const _f32 radius = 0.3f + 0.15f * sinf(jc_math_pi2 * progress);

		// 색상도 진행도에 따라 무지개처럼 변한다. (위상을 120도씩 밀어서 RGB 계산)
		const _f32 r = 0.5f + 0.5f * sinf(jc_math_pi2 * progress);
		const _f32 g = 0.5f + 0.5f * sinf(jc_math_pi2 * progress + jc_math_pi2 / 3.0f);
		const _f32 b = 0.5f + 0.5f * sinf(jc_math_pi2 * progress + jc_math_pi2 * 2.0f / 3.0f);

		// 하이라이트 점이 원 가장자리를 따라 한 바퀴 도는 위치 (회전하는 느낌 연출)
		const _f32 dotAngle = jc_math_pi2 * progress;
		const _f32 dotX = 0.5f + cosf(dotAngle) * radius * 0.7f;
		const _f32 dotY = 0.5f + sinf(dotAngle) * radius * 0.7f;

		for (int y = 0; y < frameSize; ++y)
		{
			for (int x = 0; x < frameSize; ++x)
			{
				// 프레임 내부 정규 좌표 (0~1)
				const _f32 u = ((_f32)x + 0.5f) / (_f32)frameSize;
				const _f32 v = ((_f32)y + 0.5f) / (_f32)frameSize;

				// 중심(0.5, 0.5)에서의 거리
				const _f32 dx = u - 0.5f;
				const _f32 dy = v - 0.5f;
				const _f32 dist = sqrtf(dx * dx + dy * dy);

				if (dist > radius)
				{
					continue;	// 원 밖 = 투명 그대로
				}

				// 가장자리로 갈수록 부드럽게 어두워지는 음영 (공 느낌)
				const _f32 shade = 1.0f - (dist / radius) * 0.5f;

				_f32 outR = r * shade;
				_f32 outG = g * shade;
				_f32 outB = b * shade;

				// 하이라이트 점 근처는 흰색으로 밝힌다
				const _f32 hx = u - dotX;
				const _f32 hy = v - dotY;
				if (sqrtf(hx * hx + hy * hy) < 0.06f)
				{
					outR = outG = outB = 1.0f;
				}

				// 시트 전체 기준의 픽셀 위치로 변환해서 기록
				const int pixelX = frameCol * frameSize + x;
				const int pixelY = frameRow * frameSize + y;
				const int offset = (pixelY * sheetSize + pixelX) * 4;

				_pOutRgba[offset + 0] = (_u8)(Clamp(outR, 0.0f, 1.0f) * 255.0f);
				_pOutRgba[offset + 1] = (_u8)(Clamp(outG, 0.0f, 1.0f) * 255.0f);
				_pOutRgba[offset + 2] = (_u8)(Clamp(outB, 0.0f, 1.0f) * 255.0f);
				_pOutRgba[offset + 3] = 255;
			}
		}
	}
}

// 스프라이트 사각형 정점 4개를 채운다. (동적 버퍼용, 매 프레임 갱신)
void FillSpriteQuad(VertexPTC* _pOutVertices4, const vec2& _center, const vec2& _halfSize,
	const vec2& _uvOffset, const vec2& _uvScale)
{
	const color white = color::White();

	// UV는 시트에서 이 프레임 영역만 잘라 쓴다: offset ~ offset + scale
	_pOutVertices4[0] = { vec3(_center.x - _halfSize.x, _center.y + _halfSize.y, 0.0f),
		vec2(_uvOffset.x, _uvOffset.y), white };
	_pOutVertices4[1] = { vec3(_center.x + _halfSize.x, _center.y + _halfSize.y, 0.0f),
		vec2(_uvOffset.x + _uvScale.x, _uvOffset.y), white };
	_pOutVertices4[2] = { vec3(_center.x - _halfSize.x, _center.y - _halfSize.y, 0.0f),
		vec2(_uvOffset.x, _uvOffset.y + _uvScale.y), white };
	_pOutVertices4[3] = { vec3(_center.x + _halfSize.x, _center.y - _halfSize.y, 0.0f),
		vec2(_uvOffset.x + _uvScale.x, _uvOffset.y + _uvScale.y), white };
}

// 진행도 t(0~1)에 이징(Easing) 함수를 적용해 보간 공식용 진행도로 변환한다.
_f32 ApplyEasing(_f32 _t, _s32 _mode)
{
	const _f32 t = Clamp(_t, 0.0f, 1.0f);

	if (_mode == 1)
	{
		// [부드럽게] 스무스스텝: 천천히 출발 -> 중간에 빠르게 -> 천천히 도착
		return t * t * (3.0f - 2.0f * t);
	}

	if (_mode == 2)
	{
		// [바운스] 도착 지점에서 공처럼 몇 번 튀는 고전 공식 (Robert Penner의 easeOutBounce)
		_f32 tb = t;
		if (tb < 1.0f / 2.75f)
		{
			return 7.5625f * tb * tb;
		}
		if (tb < 2.0f / 2.75f)
		{
			tb -= 1.5f / 2.75f;
			return 7.5625f * tb * tb + 0.75f;
		}
		if (tb < 2.5f / 2.75f)
		{
			tb -= 2.25f / 2.75f;
			return 7.5625f * tb * tb + 0.9375f;
		}
		tb -= 2.625f / 2.75f;
		return 7.5625f * tb * tb + 0.984375f;
	}

	// [선형] 그대로 반환. 등속 이동 (기계적인 느낌)
	return t;
}
