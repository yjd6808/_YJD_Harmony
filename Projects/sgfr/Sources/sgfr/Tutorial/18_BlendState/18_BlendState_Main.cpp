/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 1:50:00 PM
 * =====================
 * 18. 블렌드 스테이트 (Blend State)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 블렌딩 공식: 최종색 = 새색 x SrcBlend (+) 기존색 x DestBlend
 * 2. 불투명/알파/가산/곱셈 네 가지 모드의 차이와 용도
 * 3. 왜 반투명 물체는 그리는 순서가 중요한가
 * 4. 동적(Dynamic) 정점 버퍼로 매 프레임 정점을 갱신하는 법
 *
 * [Before/After 비교 뷰]
 * 흰 세로선을 경계로 같은 장면이 두 번 그려진다.
 * - 왼쪽 (Before): Opaque 고정 = 블렌딩 미적용 (알파 무시! 사각형이 통째로 보인다)
 * - 오른쪽(After): 현재 선택한 블렌드 모드 적용
 * 블렌드는 픽셀마다 고를 수 없는 출력 병합(OM) 단계 상태이므로,
 * "왼쪽 묶음은 Opaque로 드로우 -> 오른쪽 묶음은 선택 모드로 드로우"처럼
 * 드로우 호출 사이에 상태를 바꿔 끼워 비교한다. (상태 교체 비용은 싸다!)
 *
 * [관찰 포인트]
 * 양쪽 모두 세 개의 색 원(빨강/초록/파랑)이 똑같이 빙글빙글 돈다.
 * 모드를 바꿔가며 왼쪽(미적용)과 격짜가 어떻게 다른지 비교해보자!
 * - Additive: 겹칠수록 하얖게 (빛의 삼원색)
 * - Multiply: 겹칠수록 검게 (물감의 삼원색처럼)
 *
 * [조작법]
 * - 1: 불투명 / 2: 알파 / 3: 가산 / 4: 곱셈 (오른쪽에만 적용)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/18_BlendState/18_BlendState_Main.h"
#include "sgfr/Tutorial/18_BlendState/18_BlendState_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 창 제목에 표시할 모드 이름표 (BlendMode enum 순서와 일치)
	const char* s_szBlendNames[] = { "Opaque(불투명)", "Alpha(반투명)", "Additive(가산)", "Multiply(곱셈)" };
}

// 블렌드 스테이트 튜토리얼을 실행한다. (좌: Opaque 기준 / 우: 선택 모드 Before/After 비교)
void BlendState_Main()
{
	PrintBlendExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("18. 블렌드 스테이트 (1~4 모드, ESC 종료)", 800, 600))
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

	// 2. 부드러운 흰색 원 텍스처 (색은 정점 색으로 입힌다 -> 텍스처 하나로 재활용)
	static _u8 s_Pixels[128 * 128 * 4];
	FillSoftCirclePixels(s_Pixels, 128);

	Texture texture;
	if (!texture.CreateFromMemory(&device, s_Pixels, 128, 128))
	{
		jc::Console::WriteLine("텍스처 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 동적 정점 버퍼: 매 프레임 원의 위치가 바뀌므로 CPU에서 갱신한다.
	VertexPTC vertices[4] = {};
	_u32 indices[6] = { 0, 1, 2, 2, 1, 3 };	// 시계 방향 삼각형 2개

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPTC), 4, ResourceUsage::ruDynamic) ||	// bDynamic = true!
		!ib.Create(&device, indices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPTC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, BlendQuadShaderSource(), pLayoutDescs, layoutCount))
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 현재 블렌드 모드 (키 입력으로 바꾼다. 오른쪽 After 묶음에만 적용된다)
	GraphicDevice::BlendMode blendMode = GraphicDevice::BlendMode::bmAlpha;

	auto UpdateTitle = [&]()
	{
		jc::String szTitle = jc::StringUtil::Format("18. 블렌드 - 왼쪽: Opaque(기준) | 오른쪽: %s (1~4, ESC)", s_szBlendNames[static_cast<_s32>(blendMode)]);
		window.SetTitle(szTitle);
	};
	UpdateTitle();

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		bool bChanged = false;
		if (input.IsKeyPressed('1')) { blendMode = GraphicDevice::BlendMode::bmNone;   bChanged = true; }
		if (input.IsKeyPressed('2')) { blendMode = GraphicDevice::BlendMode::bmAlpha;    bChanged = true; }
		if (input.IsKeyPressed('3')) { blendMode = GraphicDevice::BlendMode::bmAdd; bChanged = true; }
		if (input.IsKeyPressed('4')) { blendMode = GraphicDevice::BlendMode::bmMultiply; bChanged = true; }
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		// 배경은 중간 밝기 회색: 가산(밝아짐)과 곱셈(어두워짐)을 모두 관찰하기 좋다.
		device.BeginFrame(color(0x73, 0x73, 0x73, 0xFF));

		const color circleColors[3] = {
			color(0xFF, 0x40, 0x40, 0xFF),	// 빨강
			color(0x40, 0xFF, 0x40, 0xFF),	// 초록
			color(0x4D, 0x66, 0xFF, 0xFF),		// 파랑
		};

		texture.Bind(&device, 0);
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 한 묶음(원 3개)을 지정한 중심/블렌드 모드로 그리는 보조 람다.
		// 블렌드 스테이트는 드로우 단위로만 바꿀 수 있으므로 묶음마다 SetBlendMode를 호출한다.
		auto DrawCircleGroup = [&](const vec2& _groupCenter, GraphicDevice::BlendMode _mode)
		{
			device.SetBlendMode(_mode);
			for (_s32 c = 0; c < 3; ++c)
			{
				// 각 원의 중심: 반지름 0.15짜리 궤도를 도는 위치 (서로 120도 차이)
				const _f32 angle = elapsed * 0.8f + jc_math_pi2 * c / 3.0f;
				const vec2 center(_groupCenter.x + cosf(angle) * 0.15f, _groupCenter.y + sinf(angle) * 0.15f);

				FillQuadVertices(vertices, center, 0.28f, circleColors[c]);
				vb.Update(&device, vertices, 4);	// 동적 버퍼 갱신

				vb.Bind(&device);
				ib.Bind(&device);
				device.Context()->DrawIndexed(6, 0, 0);
			}
		};

		// [Before] 왼쪽 묶음: 블렌딩 미적용(Opaque). 알파가 무시돼 사각형 그대로 보인다!
		DrawCircleGroup(vec2(-0.5f, 0.0f), GraphicDevice::BlendMode::bmNone);

		// [After] 오른쪽 묶음: 현재 선택한 모드 적용. 겹침 영역의 색 합성을 관찰!
		DrawCircleGroup(vec2(+0.5f, 0.0f), blendMode);

		// 경계선: 화면 중앙에 가늘고 흰 세로 띄(divider)를 그린다.
		// UV를 원 텍스처의 중심(0.5, 0.5) 한 점으로 고정하면 불투명 흰색 픽셀만 샘플링된다.
		device.SetBlendMode(GraphicDevice::BlendMode::bmNone);
		const vec2 uvCenter(0.5f, 0.5f);
		const color lineColor(0xF2, 0xF2, 0xF2);
		vertices[0] = { vec3(-0.004f, +1.0f, 0.0f), uvCenter, lineColor };
		vertices[1] = { vec3(+0.004f, +1.0f, 0.0f), uvCenter, lineColor };
		vertices[2] = { vec3(-0.004f, -1.0f, 0.0f), uvCenter, lineColor };
		vertices[3] = { vec3(+0.004f, -1.0f, 0.0f), uvCenter, lineColor };
		vb.Update(&device, vertices, 4);
		vb.Bind(&device);
		ib.Bind(&device);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 7. 정리 (블렌드 상태를 기본값으로 되돌려 다음 튜토리얼에 영향을 주지 않는다)
	device.SetBlendMode(GraphicDevice::BlendMode::bmNone);
	device.Finalize();
	window.Destroy();
}

