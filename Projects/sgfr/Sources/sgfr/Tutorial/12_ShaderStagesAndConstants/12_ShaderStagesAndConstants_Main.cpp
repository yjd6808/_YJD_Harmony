/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 12. 셰이더 스테이지와 상수버퍼 (Shader Stages & Constant Buffers)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 상수버퍼는 '스테이지(VS/PS)별로 따로' 바인딩된다는 사실
 * 2. 슬롯 규약: b0 = 프레임 공통, b1 = 오브젝트별 (엔진 전체가 이 규약을 따른다)
 * 3. 16바이트 정렬: C++ 구조체와 HLSL cbuffer의 메모리 배치 맞추기
 *
 * [Before/After 비교]
 * - Before: 상수버퍼 슬롯을 튜토리얼마다 임의로 사용 (b0 하나에 전부 구겨넣기)
 * - After: b0(프레임)/b1(오브젝트) 역할 분리 → 갱신 주기가 다른 데이터를 분리해
 * 프레임당 Update 횟수를 최소화한다. (Renderer3D가 이 규약의 완성형)
 *
 * [조작법]
 * - ESC: 종료 (사각형 두 개가 서로 다른 주기로 춤춘다)
 */

#include "Core.h"
#include "sgfr/Tutorial/12_ShaderStagesAndConstants/12_ShaderStagesAndConstants_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	// b0: 프레임에 한 번만 갱신되는 공통 데이터 (16바이트 배수!)
	struct CbFrame
	{
		_f32 time_;				// 누적 시간(초)
		_f32 padding_[3];		// 16바이트 정렬용 여백
	};

	// b1: 오브젝트마다 다른 데이터 (VS는 offset, PS는 tint를 읽는다)
	struct CbObject
	{
		vec2 offset_;			// NDC 이동량 (VS용)
		_f32 wobble_;			// 춤 진폭 배율 (VS용)
		_f32 padding_;			// 16바이트 정렬용 여백
		vec4 tint_;			// 곱해질 색 (PS용)
	};

	// b0은 VS만, b1은 VS와 PS 모두 읽는 셰이더
	const char* STAGE_DEMO_SHADER_SOURCE = R"(
cbuffer CbFrame : register(b0)
{
	float gTime;
	float3 gPadding0;
};

cbuffer CbObject : register(b1)
{
	float2 gOffset;
	float  gWobble;
	float  gPadding1;
	float4 gTint;
};

struct VsIn
{
	float3 position : POSITION;
	float2 uv       : TEXCOORD0;
	float4 color    : COLOR0;
};

struct VsOut
{
	float4 position : SV_Position;
	float4 color    : COLOR0;
};

VsOut VSMain(VsIn _in)
{
	VsOut result;
	float2 wobbled = _in.position.xy + gOffset;
	wobbled.y += sin(gTime * 3.0f + _in.position.x * 4.0f) * gWobble;	// b0의 시간으로 춤추기
	result.position = float4(wobbled, _in.position.z, 1.0f);
	result.color = _in.color;
	return result;
}

float4 PSMain(VsOut _in) : SV_Target
{
	return _in.color * gTint;	// 같은 b1을 PS 스테이지에서도 읽는다
}
)";
}

// 셰이더 스테이지와 상수버퍼 튜토리얼을 실행한다. (b0/b1 역할 분리 체험)
void ShaderStagesAndConstants_Main()
{
	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("12. 셰이더 스테이지와 상수버퍼 (ESC 종료)", 800, 600))
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

	GraphicContext& context = device.GetContext();

	// 2. 셰이더 + 메시 준비
	VertexShader vs;
	PixelShader ps;
	if (!vs.InitializeFromSource(&device, STAGE_DEMO_SHADER_SOURCE) ||
		!ps.InitializeFromSource(&device, STAGE_DEMO_SHADER_SOURCE))
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	const VertexPTC vertices[4] =
	{
		{ vec3(-0.25f,  0.25f, 0.0f), vec2(0.0f, 0.0f), color(0xFF, 0xFF, 0xFF, 0xFF) },
		{ vec3( 0.25f,  0.25f, 0.0f), vec2(1.0f, 0.0f), color(0xFF, 0xFF, 0xFF, 0xFF) },
		{ vec3( 0.25f, -0.25f, 0.0f), vec2(1.0f, 1.0f), color(0x99, 0x99, 0x99, 0xFF) },
		{ vec3(-0.25f, -0.25f, 0.0f), vec2(0.0f, 1.0f), color(0x99, 0x99, 0x99, 0xFF) },
	};
	const _u32 indices[6] = { 0, 1, 2, 0, 2, 3 };

	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPTC::LayoutDescs(&layoutCount);

	Mesh quad;
	if (!quad.Initialize(&device, vertices, sizeof(VertexPTC), 4, pLayoutDescs, layoutCount, &vs, indices, 6))
	{
		jc::Console::WriteLine("메시 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 상수버퍼 2개: 갱신 주기가 다르므로 분리한다
	ConstantBuffer<CbFrame> frameCb;
	ConstantBuffer<CbObject> objectCb;
	if (!frameCb.Create(&device) || !objectCb.Create(&device))
	{
		jc::Console::WriteLine("상수버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("b0(프레임)/b1(오브젝트) 슬롯 규약을 눈으로 확인하세요!");

	// 4. 렌더 루프
	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0x0F, 0x0F, 0x1A, 0xFF));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		context.SetVertexShader(&vs);
		context.SetPixelShader(&ps);

		// (1) b0: 프레임에 딱 한 번만 갱신 + VS 스테이지에 바인딩
		CbFrame frame = {};
		frame.time_ = elapsed;
		frameCb.Update(&device, frame);
		context.SetConstantBuffer(ShaderStage::ssVertex, 0, frameCb.Raw());

		quad.Bind(context);

		// (2) b1: 오브젝트마다 갱신. VS(이동)와 PS(색) '두 스테이지 모두'에 바인딩해야 한다!
		// 한 쪽에만 바인딩하면 다른 쪽 스테이지는 이전 값(또는 0)을 읽는다. 직접 지워보면 안다!
		CbObject left = {};
		left.offset_ = vec2(-0.4f, 0.0f);
		left.wobble_ = 0.08f;
		left.tint_ = vec4(0.3f, 0.8f, 1.0f, 1.0f);
		objectCb.Update(&device, left);
		context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb.Raw());
		context.SetConstantBuffer(ShaderStage::ssPixel, 1, objectCb.Raw());
		quad.Draw(context);

		CbObject right = {};
		right.offset_ = vec2(0.4f, 0.0f);
		right.wobble_ = 0.2f;
		right.tint_ = vec4(1.0f, 0.6f, 0.2f, 1.0f);
		objectCb.Update(&device, right);
		context.SetConstantBuffer(ShaderStage::ssVertex, 1, objectCb.Raw());
		context.SetConstantBuffer(ShaderStage::ssPixel, 1, objectCb.Raw());
		quad.Draw(context);

		device.EndFrame(true);
	}

	// 5. 정리
	quad.Finalize();
	ps.Finalize();
	vs.Finalize();
	device.Finalize();
	window.Destroy();
}
