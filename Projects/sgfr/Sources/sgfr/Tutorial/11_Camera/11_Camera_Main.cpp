/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:20:00 AM
 * =====================
 * 11. 카메라 좌표계 (Camera / View Matrix) - WASD 이동과 줌
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 뷰 행렬의 정체: 카메라 이동의 역변환
 * 2. 월드 -> 뷰 -> 투영으로 이어지는 전체 변환 체인 완성
 * 3. 키 입력으로 매 프레임 카메라 상태를 갱신하는 법 (deltaTime 사용)
 *
 * [왜 deltaTime을 곱하나?]
 * "초당 1.5 유닛 이동"처럼 시간 기준으로 속도를 정의하면
 * 60FPS에서도 144FPS에서도 같은 속도로 움직인다. (프레임 독립적 움직임)
 *
 * [조작법]
 * - WASD: 카메라 이동 / 마우스 휠: 줌 인/아웃 / R: 카메라 리셋 / ESC: 종료
 * - 바닥에 깔린 타일들이 카메라 반대 방향으로 움직이는 것을 관찰하자!
 */

#include "Core.h"
#include "sgfr/Tutorial/11_Camera/11_Camera_Main.h"
#include "sgfr/Tutorial/11_Camera/11_Camera_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 셰이더 CbTransform과 메모리 배치 일치 (64바이트)
	struct CbTransform
	{
		mat4 worldViewProj_;	// 월드 x 뷰 x 투영 결합 행렬
	};
}

// 카메라 튜토리얼을 실행한다. (WASD 이동 / 마우스 휠 줌)
void Camera_Main()
{
	PrintCameraExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"11. 카메라 - WASD 이동 / 휠 줌 / R 리셋 (ESC로 종료)", 800, 600))
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

	// 2. 타일 하나짜리 사각형 버퍼 (월드 행렬로 위치를 바꿔가며 여러 번 그린다)
	const VertexPC vertices[] = {
		{ vec3(-0.5f, +0.5f, 0.0f), color(0xFF, 0xFF, 0xFF, 0xFF) },
		{ vec3(+0.5f, +0.5f, 0.0f), color(0xCC, 0xCC, 0xCC, 0xFF) },
		{ vec3(-0.5f, -0.5f, 0.0f), color(0x99, 0x99, 0x99, 0xFF) },
		{ vec3(+0.5f, -0.5f, 0.0f), color(0x66, 0x66, 0x66, 0xFF) },
	};
	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPC), 4) ||
		!ib.Create(&device, indices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 셰이더 + 상수 버퍼
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	ConstantBuffer<CbTransform> cbTransform;
	if (!shader.CompileFromString(&device, CameraShaderSource(), pLayoutDescs, layoutCount) ||
		!cbTransform.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 카메라 상태 변수
	vec2 cameraPos = vec2(0.0f, 0.0f);	// 카메라가 바라보는 월드 위치
	_f32 zoom = 1.0f;					// 1보다 크면 확대, 작으면 축소

	const _f32 aspect = window.AspectRatio();
	const mat4 proj = mat4::OrthographicOffCenterLH(-aspect, +aspect, -1.0f, +1.0f, 0.0f, 1.0f);

	FrameTimer timer;
	timer.Reset();

	jc::Console::WriteLine("WASD로 이동, 마우스 휠로 줌, R로 리셋하세요.");

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		timer.Tick();
		const _f32 dt = timer.DeltaTime();

		// === 카메라 조작 (폴링 방식: 누르고 있는 동안 계속 움직임) ===
		const _f32 moveSpeed = 1.5f / zoom;	// 줌인 상태에서는 천천히 (화면 기준 속도가 일정하도록)
		if (input.IsKeyDown('W')) { cameraPos.y += moveSpeed * dt; }
		if (input.IsKeyDown('S')) { cameraPos.y -= moveSpeed * dt; }
		if (input.IsKeyDown('A')) { cameraPos.x -= moveSpeed * dt; }
		if (input.IsKeyDown('D')) { cameraPos.x += moveSpeed * dt; }

		// 마우스 휠: 한 칸(틱 ±1)당 10%씩 배율 증감
		// (WheelDelta는 이미 120으로 나눈 "틱 단위"를 반환하므로 추가 나눗셈 금지)
		const _s32 wheel = input.WheelDelta();
		if (wheel != 0)
		{
			zoom *= 1.0f + _f32(wheel) * 0.1f;
			zoom = Clamp(zoom, 0.2f, 5.0f);
		}

		// R: 카메라 리셋
		if (input.IsKeyPressed('R'))
		{
			cameraPos = vec2(0.0f, 0.0f);
			zoom = 1.0f;
		}

		input.NextFrame();

		// === 뷰 행렬 생성: 카메라 이동의 역변환! ===
		// 카메라가 (2, 1)에 있다면 세상을 (-2, -1)만큼 움직이면 된다.
		// 줌은 그 다음에 곱해서 화면 중앙 기준으로 확대/축소한다.
		const mat4 view = mat4::Translation(-cameraPos.x, -cameraPos.y, 0.0f) * mat4::Scale(zoom, zoom, 1.0f);

		device.BeginFrame(color(0x0D, 0x14, 0x0D, 0xFF));

		// === 5x5 타일을 그려서 "세상"을 만든다 ===
		for (_s32 y = -2; y <= 2; ++y)
		{
			for (_s32 x = -2; x <= 2; ++x)
			{
				// 타일 간격 0.6, 크기 0.5 (사이에 틈이 보이도록)
				const mat4 world = mat4::Scale(0.5f, 0.5f, 1.0f) * mat4::Translation(x * 0.6f, y * 0.6f, 0.0f);

				CbTransform cb = {};
				cb.worldViewProj_ = world * view * proj;	// 월드 -> 뷰 -> 투영 순서!
				cbTransform.UpdateAndBind(&device, cb, 0);

				vb.Bind(&device);
				ib.Bind(&device);
				shader.Bind(&device);
				device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				device.Context()->DrawIndexed(6, 0, 0);
			}
		}

		device.EndFrame(true);
	}

	// 6. 정리
	device.Finalize();
	window.Destroy();
}
