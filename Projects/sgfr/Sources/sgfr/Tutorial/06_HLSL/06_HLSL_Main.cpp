/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:50:00 AM
 * =====================
 * 06. HLSL 셰이더 (HLSL) - 상수 버퍼로 삼각형 애니메이션
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. HLSL 문법 기초 (타입, 시맨틱, 내장 함수)
 * 2. 상수 버퍼(cbuffer)로 C++ -> GPU로 데이터 보내기
 * 3. 16바이트 정렬 규칙 (C++ 구조체와 HLSL cbuffer의 메모리 일치)
 * 4. 시간 기반 애니메이션 (크기 맥박 + 색상 반전 왕복)
 *
 * [05번과의 차이]
 * 05번 셰이더는 입력을 그대로 통과시켰다. (정적)
 * 이번에는 매 프레임 C++이 시간을 보내주어 GPU가 알아서 움직인다. (동적)
 * 정점 데이터는 한 번도 안 바꿨는데 화면이 움직이는 것이 포인트!
 *
 * [조작법]
 * - 삼각형이 커졌다 작아졌다 하며 색이 반전색과 섞인다. ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/06_HLSL/06_HLSL_Main.h"
#include "sgfr/Tutorial/06_HLSL/06_HLSL_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 셰이더 CbTime(b0)과 메모리 배치 일치 (16바이트)
	// ConstantBuffer<T>는 sizeof(T)가 16의 배수가 아니면 컴파일 에러를 낸다.
	struct CbTime
	{
		_f32 time_;			// 경과 시간(초)
		_f32 padding_[3];	// 16바이트 정렬용 (HLSL의 gPadding과 대응)
	};
}

// HLSL 셰이더 튜토리얼을 실행한다. (상수 버퍼로 삼각형 색/크기 애니메이션)
void HLSL_Main()
{
	PrintHlslSummary();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("06. HLSL 셰이더 - 상수 버퍼 애니메이션 (ESC로 종료)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize())
	{
	jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}
	if (!device.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
	jc::Console::WriteLine("스왑체인 생성 실패!");
	device.Finalize();
	window.Destroy();
	return;
	}

	// 2. 삼각형 정점 (05번과 동일한 NDC 좌표 직접 지정)
	const VertexPC vertices[] = {
		{ vec3( 0.0f,  +0.5f, 0.0f), color(0xFF, 0x00, 0x00, 0xFF) },	// 위 (빨강)
		{ vec3(+0.5f, -0.5f, 0.0f), color(0x00, 0x00, 0xFF, 0xFF) },	// 오른아래 (파랑)
		{ vec3(-0.5f, -0.5f, 0.0f), color(0x00, 0xFF, 0x00, 0xFF) },	// 왼아래 (초록)
	};

	VertexBuffer vb;
	if (!vb.Create(&device, vertices, sizeof(VertexPC), 3))
	{
		jc::Console::WriteLine("정점 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 셰이더 + 상수 버퍼
	VertexLayoutSpan pLayoutDescs = VertexPC::Layout();

	_u32 vsShader = device.Context().CreateVertexShader(AnimatedShaderSource());
	_u32 psShader = device.Context().CreatePixelShader(AnimatedShaderSource());
	ConstantBuffer<CbTime> cbTime;
	if (vsShader == INVALID_HANDLE || psShader == INVALID_HANDLE ||
		!cbTime.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 4. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0x1A, 0x1A, 0x1F, 0xFF));

		// 매 프레임 시간을 GPU로 보낸다. 이것이 상수 버퍼의 역할!
		CbTime cb = {};
		cb.time_ = elapsed;
		cbTime.UpdateAndBind(device.Context(), cb, 0);	// register(b0)에 연결

		vb.Bind(device.Context());
		device.Context().SetVertexShader(vsShader);
		device.Context().SetPixelShader(psShader);
		{
			device.Context().SetInputLayout(vsShader, pLayoutDescs);
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		device.Context().Draw(3, 0);

		device.Present(true);
	}

	// 5. 정리
	device.Finalize();
	window.Destroy();
}
