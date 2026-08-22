/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:50:00 PM
 * =====================
 * 30. 스프라이트 애니메이션 (Sprite Animation)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 스프라이트 시트: 한 텍스처에 여러 프레임을 바둑판처럼 배치
 * 2. UV 오프셋/스케일로 원하는 프레임만 오려내기
 * 3. 경과 시간 -> 프레임 번호 변환 (FPS 개념)
 * 4. 이징(Easing): 같은 이동도 속도 곡선에 따라 완전히 다른 느낌이 된다
 * 5. 동적 정점 버퍼로 매 프레임 정점을 갱신하는 법
 *
 * [Before/After 비교 뷰]
 * 같은 캐릭터가 두 줄로 동시에 왜다걔다 한다. (같은 프레임, 같은 이동 구간)
 * - 지단 (Before): 항상 '선형' 이동 = 기준이 되는 등속 움직임
 * - 아랫줄(After): 선택한 이징 적용 = 속도 곡선이 바뀌 움직임
 * 두 캐릭터가 벌어졌다 따라잡는 모습을 보면 이징이 속도를 어떻게
 * 재분배하는지 한눈에 비교된다! (1번 선택 시 두 줄이 완전히 같이 간다)
 *
 * [관찰 포인트]
 * - '부드럽게'는 양끝에서 느리고 중간에서 빠르다 (앞뒤로 벌어졌다 좁혀진다)
 * - '바운스'는 도착 지점에서 공처럼 통통 튀는 느낌이 난다
 * - FPS를 낮추면 걸음 동작이 뚝뚝 끊기지만 이동 속도는 그대로다
 * (프레임 애니메이션과 위치 이동은 독립적인 시간 축!)
 *
 * [조작법]
 * - 1: 선형 / 2: 부드럽게(EaseInOut) / 3: 바운스 (아랫줄에만 적용)
 * - 위/아래 방향키: 애니메이션 FPS 증감 (4 ~ 60)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/30_SpriteAnimation/30_SpriteAnimation_Main.h"
#include "sgfr/Tutorial/30_SpriteAnimation/30_SpriteAnimation_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// 셰이더 공용 사용

using namespace sgf;
using namespace jc;

namespace
{
	// 창 제목에 표시할 이징 이름표 (ApplyEasing의 모드 순서와 일치)
	const char* s_szEasingNames[] = { "선형", "부드럽게", "바운스" };
}

// 스프라이트 애니메이션 튜토리얼을 실행한다. (위: 선형 기준 / 아래: 선택 이징 Before/After 비교)
void SpriteAnimation_Main()
{
	PrintSpriteAnimationExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("30. 스프라이트 - 위: 선형(기준) | 아래: 선택 이징 (1~3, ↑↓ FPS, ESC)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}

	// 2. 스프라이트 시트 텍스처: 256x256, 4x4 = 16프레임을 코드로 생성
	const _s32 sheetSize = 256;
	_u8* pPixels = new _u8[sheetSize * sheetSize * 4];
	FillSpriteSheetPixels(pPixels);

	Texture sheetTexture;
	const bool bTextureOk = sheetTexture.CreateFromMemory(&device, pPixels, sheetSize, sheetSize);
	JC_DELETE_SAFE(pPixels);	// GPU로 복사되었으므로 CPU 메모리는 바로 해제
	if (!bTextureOk)
	{
		jc::Console::WriteLine("스프라이트 시트 텍스처 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 동적 정점 버퍼: 매 프레임 위치/UV가 바뀌므로 CPU에서 갱신한다.
	// 한 버퍼를 프레임당 두 번(지단/아랫줄) Update->Draw 해도 되는 이유:
	// WRITE_DISCARD는 이전 내용을 보호하며 새 공간을 주므로 그리기 순서가 꿬이지 않는다!
	VertexPTC quadVertices[4];
	FillSpriteQuad(quadVertices, vec2(0.0f, 0.0f), vec2(0.25f, 0.25f), vec2(0.0f, 0.0f), vec2(0.25f, 0.25f));

	_u32 quadIndices[6] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer quadVb;
	IndexBuffer quadIb;
	if (!quadVb.Create(&device, quadVertices, sizeof(VertexPTC), 4, ResourceUsage::ruDynamic) ||
		!quadIb.Create(&device, quadIndices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayout = VertexPTC::LayoutDescs(&layoutCount);

	Shader spriteShader;
	if (!spriteShader.CompileFromString(&device, TextureShaderSource(), pLayout, layoutCount))
	{
		jc::Console::WriteLine("셰이더 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 상태: 아랫줄에 적용할 이징 모드와 애니메이션 FPS
	_s32 easingMode = 1;	// 0=선형, 1=부드럽게, 2=바운스 (시작은 '부드럽게'로 차이가 바로 보이게)
	_s32 animFps = 16;

	auto UpdateTitle = [&]()
	{
		jc::String szTitle = jc::StringUtil::Format("30. 스프라이트 - 위: 선형(기준) | 아래: %s / FPS %d (1~3, ↑↓, ESC)", s_szEasingNames[easingMode], animFps);
		window.SetTitle(szTitle);
	};
	UpdateTitle();

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 알파 블렌드: 캐릭터 주변의 투명 픽셀이 배경과 자연스럽게 섞이도록
	device.SetBlendMode(GraphicDevice::BlendMode::bmAlpha);
	// 도트 느낌을 살리기 위해 Point 샘플러 (픽셀아트 게임의 핵심 세팅!)
	device.SetSampler(GraphicDevice::SamplerFilter::fmPoint, GraphicDevice::SamplerAddress::amClamp, 0);

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		timer.Tick();
		elapsed += timer.DeltaTime();

		bool bChanged = false;
		for (_s32 k = 0; k < 3; ++k)
		{
			if (input.IsKeyPressed('1' + k))
			{
				easingMode = k;
				bChanged = true;
			}
		}
		if (input.IsKeyPressed(VK_UP))
		{
			animFps = Clamp(animFps + 4, 4, 60);
			bChanged = true;
		}
		if (input.IsKeyPressed(VK_DOWN))
		{
			animFps = Clamp(animFps - 4, 4, 60);
			bChanged = true;
		}
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		// --- 공통 애니메이션 계산 (두 줄이 완전히 같은 프레임/진행도를 공유) ---

		// (1) 경과 시간 -> 프레임 번호. 4x4 시트이므로 16으로 나눈 나머지로 순환
		const _s32 frame = (_s32)(elapsed * animFps) % 16;
		const _s32 col = frame % 4;
		const _s32 row = frame / 4;
		const vec2 uvOffset = vec2(col * 0.25f, row * 0.25f);
		const vec2 uvScale = vec2(0.25f, 0.25f);

		// (2) 2초 주기 핑퇰: 0~1초는 왼->오른, 1~2초는 오른->왼 (왕복 이동)
		const _f32 cycle = fmodf(elapsed, 2.0f);
		const _f32 pingPong = (cycle < 1.0f) ? cycle : 2.0f - cycle;

		// (3) 이동 진행도(0~1)에 이징 적용. 지단은 항상 선형(=그대로), 아랫줄만 선택 모드!
		const _f32 easedBefore = ApplyEasing(pingPong, 0);			// Before: 선형 기준
		const _f32 easedAfter = ApplyEasing(pingPong, easingMode);	// After: 선택 이징

		// (4) 진행도 -> 화면 x좌표 (NDC -0.6 ~ +0.6)
		const _f32 posXBefore = Lerp(-0.6f, 0.6f, easedBefore);
		const _f32 posXAfter = Lerp(-0.6f, 0.6f, easedAfter);

		// 종횡비 보정: NDC는 화면 비율과 무관하므로 x를 나누어 정사각형으로 보이게 한다
		const vec2 halfSize = vec2(0.25f / window.AspectRatio(), 0.25f);

		device.BeginFrame(color(0x1A, 0x1A, 0x24, 0xFF));
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		sheetTexture.Bind(&device, 0);
		spriteShader.Bind(&device);
		quadIb.Bind(&device);

		// --- [Before] 지단 (y=+0.4): 항상 선형 이동 = 기준선 ---
		FillSpriteQuad(quadVertices, vec2(posXBefore, +0.4f), halfSize, uvOffset, uvScale);
		quadVb.Update(&device, quadVertices, 4);
		quadVb.Bind(&device);
		device.Context()->DrawIndexed(6, 0, 0);

		// --- [After] 아랫줄 (y=-0.4): 선택한 이징 적용 ---
		FillSpriteQuad(quadVertices, vec2(posXAfter, -0.4f), halfSize, uvOffset, uvScale);
		quadVb.Update(&device, quadVertices, 4);
		quadVb.Bind(&device);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 7. 정리 (다음 튜토리얼을 위해 불투명 블렌드로 복원)
	device.SetBlendMode(GraphicDevice::BlendMode::bmNone);
	device.Finalize();
	window.Destroy();
}

