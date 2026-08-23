/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:10:00 AM
 * =====================
 * 10. 월드 변환 (World Transform) - 태양/지구/달 계층 구조
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 로컬 좌표계 -> 월드 좌표계 변환 (월드 행렬)
 * 2. SRT 순서: 크기(S) -> 회전(R) -> 이동(T) 순서로 곱하는 이유
 * 3. 행렬 곱셈으로 부모-자식 계층 구조 만들기 (태양-지구-달)
 *
 * [SRT 순서가 중요한 이유]
 * 행 벡터 규약(v' = v * M)에서는 왼쪽 행렬이 먼저 적용된다.
 * S * R * T: 크기 바꾸고 -> 제자리에서 회전하고 -> 이동 (원하는 결과!)
 * T * R * S: 먼저 이동해버리면 회전이 원점 기준 공전이 되어버린다.
 * (01번 선형대수 튜토리얼에서 숫자로 확인했던 내용을 눈으로 보는 회!)
 *
 * [조작법]
 * - 가운데 태양(노랑), 그 주위를 도는 지구(파랑), 지구 주위를 도는 달(회색).
 * - ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/10_WorldTransform/10_WorldTransform_Main.h"
#include "sgfr/Tutorial/10_WorldTransform/10_WorldTransform_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 셰이더 CbTransform과 메모리 배치 일치 (mat4 = 64바이트, 16의 배수 OK)
	struct CbTransform
	{
		mat4 worldViewProj_;	// 월드 x 투영 결합 행렬
	};
}

// 월드 변환 튜토리얼을 실행한다. (태양-지구-달 계층 구조)
void WorldTransform_Main()
{
	PrintHierarchyExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("10. 월드 변환 - 태양/지구/달 (ESC로 종료)", 800, 600))
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

	// 2. 큰 사각형 하나를 만들고, 행렬로 크기/위치를 바꿔가며 재활용한다.
	// 한 변 길이 1짜리 정사각형. (중심이 원점)
	// 색은 흰색으로 두고... 싶지만 VertexPC에는 틴트 기능이 없으므로
	// 천체별로 정점 버퍼를 3개 만든다. (단순함 우선!)
	auto MakeQuad = [](const color& _color, VertexPC* _pOut)
	{
		_pOut[0] = { vec3(-0.5f, +0.5f, 0.0f), _color };	// 왼쪽위
		_pOut[1] = { vec3(+0.5f, +0.5f, 0.0f), _color };	// 오른쪽위
		_pOut[2] = { vec3(-0.5f, -0.5f, 0.0f), _color };	// 왼쪽아래
		_pOut[3] = { vec3(+0.5f, -0.5f, 0.0f), _color };	// 오른쪽아래
	};

	VertexPC sunVertices[4], earthVertices[4], moonVertices[4];
	MakeQuad(color(0xFF, 0xCC, 0x1A, 0xFF), sunVertices);		// 태양 (노랑)
	MakeQuad(color(0x33, 0x66, 0xFF, 0xFF), earthVertices);	// 지구 (파랑)
	MakeQuad(color(0xB3, 0xB3, 0xB3, 0xFF), moonVertices);		// 달 (회색)

	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vbSun, vbEarth, vbMoon;
	IndexBuffer ib;
	if (!vbSun.Create(&device, sunVertices, sizeof(VertexPC), 4) ||
		!vbEarth.Create(&device, earthVertices, sizeof(VertexPC), 4) ||
		!vbMoon.Create(&device, moonVertices, sizeof(VertexPC), 4) ||
		!ib.Create(&device, indices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 셰이더 + 상수 버퍼
	VertexLayoutSpan pLayoutDescs = VertexPC::Layout();

	_u32 vsShader = device.Context().CreateVertexShader(TransformShaderSource());
	_u32 psShader = device.Context().CreatePixelShader(TransformShaderSource());
	ConstantBuffer<CbTransform> cbTransform;
	if (vsShader == INVALID_HANDLE || psShader == INVALID_HANDLE ||
		!cbTransform.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 투영 행렬: 화면 비율 보정용 직교 투영
	// NDC는 가로세로 모두 -1~1이라 800x600 화면에서는 가로로 늘어난다.
	// 가로를 화면 비율만큼 넓게(-1.33~+1.33) 잡으면 정사각형이 정사각형으로 보인다.
	const _f32 aspect = window.AspectRatio();
	const mat4 proj = mat4::OrthographicOffCenterLH(-aspect, +aspect, -1.0f, +1.0f, 0.0f, 1.0f);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 그리기 함수: 월드 행렬만 바꿔가며 같은 사각형을 그린다.
	auto DrawQuad = [&](VertexBuffer& _vb, const mat4& _world)
	{
		CbTransform cb = {};
		cb.worldViewProj_ = _world * proj;	// 월드 다음 투영 (왼쪽이 먼저 적용)
		cbTransform.UpdateAndBind(device.Context(), cb, 0);
		_vb.Bind(device.Context());
		ib.Bind(device.Context());
		device.Context().SetVertexShader(vsShader);
		device.Context().SetPixelShader(psShader);
		{
			device.Context().SetInputLayout(vsShader, pLayoutDescs);
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		device.Context().DrawIndexed(6, 0, 0);
	};

	jc::Console::WriteLine("태양 주위를 지구가, 지구 주위를 달이 돕니다. 모두 행렬 곱셈의 결과입니다!");

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0x05, 0x05, 0x0D, 0xFF));

		// === 태양: 제자리에서 천천히 자전 ===
		// SRT2D(크기, 회전각, 이동) 헬퍼로 한번에 로컬 행렬을 만든다.
		const mat4 sunWorld = mat4::SRT2D(vec2(0.5f, 0.5f), elapsed * 0.5f, vec2(0.0f, 0.0f));
		DrawQuad(vbSun, sunWorld);

		// === 지구: 태양으로부터 0.7 떨어져서 공전 ===
		// 공전 = "먼저 밖으로 이동(T)한 뒤 원점 기준 회전(R)"
		// 행 벡터 규약에서는 T가 왼쪽, R이 오른쪽에 온다.
		const mat4 earthOrbit = mat4::Translation(0.7f, 0.0f, 0.0f) * mat4::RotationZ(elapsed * 1.0f);
		const mat4 earthWorld = mat4::Scale(0.2f, 0.2f, 1.0f) * mat4::RotationZ(elapsed * 3.0f) * earthOrbit;
		DrawQuad(vbEarth, earthWorld);

		// === 달: 지구로부터 0.2 떨어져서 공전 ===
		// 핵심! 달의 부모는 지구다. 달 로컬 행렬 뒤에 지구의 공전 행렬을 통째로 곱한다.
		// (지구의 자전/크기는 달에게 물려주지 않기 위해 earthOrbit만 사용)
		const mat4 moonOrbit = mat4::Translation(0.2f, 0.0f, 0.0f) * mat4::RotationZ(elapsed * 4.0f);
		const mat4 moonWorld = mat4::Scale(0.08f, 0.08f, 1.0f) * moonOrbit * earthOrbit;
		DrawQuad(vbMoon, moonWorld);

		device.Present(true);
	}

	// 6. 정리
	device.Finalize();
	window.Destroy();
}
