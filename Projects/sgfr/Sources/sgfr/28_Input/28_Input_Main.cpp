/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:10:00 AM
 * =====================
 * 28. 입력 처리 (Input) - 폴링 vs 이벤트, 마우스로 그리기
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 폴링 방식: IsKeyDown / MouseX / MouseY 로 매 프레임 상태 읽기
 *  2. 이벤트 방식: onMousePressed / onKeyPressed / onMouseWheel 구독하기
 *  3. 픽셀 좌표 -> NDC 좌표 변환 (마우스로 화면에 그리는 핵심!)
 *  4. 동적 정점 버퍼로 매 프레임 다른 도형 그리기
 *
 * [동작]
 *  - 마우스 커서를 따라다니는 사각형이 그려진다. (폴링)
 *  - 왼클릭 유지: 빨강 / 오른클릭 유지: 파랑 (폴링)
 *  - 휠: 사각형 크기 조절 (이벤트)
 *  - 아무 키나 누르면 콘솔에 키 코드 출력 (이벤트)
 *  - ESC로 종료.
 */

#include "Core.h"
#include "sgfr/28_Input/28_Input_Main.h"
#include "sgfr/28_Input/28_Input_Function.h"

using namespace sgf;
using namespace jc;

// 입력 처리 튜토리얼을 실행한다. (폴링 vs 이벤트, 마우스로 그리기)
void Input_Main()
{
	PrintInputModelExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"28. 입력 처리 - 마우스/키보드/휠 (ESC로 종료)", 800, 600))
	{
		printf("윈도우 생성 실패!\n");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		printf("그래픽 디바이스 초기화 실패!\n");
		window.Destroy();
		return;
	}

	// 2. 이벤트 방식 구독 (jc::Event::Register(id, 함수))
	//    람다 캡처로 바깥 변수를 수정할 수 있다.
	_f32 halfSize = 0.08f;		// 사각형 반폭 (NDC 단위)

	// 휠 이벤트: 굴리는 순간마다 크기를 바꾼다.
	// (onMouseWheel의 delta는 이미 120으로 나눈 "틱 단위"이므로 또 나누면 안 된다)
	input.onMouseWheel.Register(1, [&halfSize](int _delta)
	{
		halfSize = Clamp(halfSize + _f32(_delta) * 0.02f, 0.02f, 0.4f);
		printf("[이벤트] 휠 delta=%d -> 반폭 %.2f\n", _delta, halfSize);
	});

	// 키 이벤트: 눌리는 순간 1번만 발생한다. (꾹 눌러도 1번!)
	input.onKeyPressed.Register(1, [](int _vkCode)
	{
		printf("[이벤트] 키 눌림: VK 코드 0x%02X\n", _vkCode);
	});

	// 마우스 버튼 이벤트: 어느 버튼이 어디서 눌렸는지 알려준다.
	input.onMousePressed.Register(1, [](MouseButton _button, int _x, int _y)
	{
		const char* szName = (_button == MouseButton::Left) ? "왼쪽"
			: (_button == MouseButton::Right) ? "오른쪽" : "가운데";
		printf("[이벤트] 마우스 %s 버튼 눌림 @ (%d, %d)\n", szName, _x, _y);
	});

	// 3. 동적 정점 버퍼 (매 프레임 위치/크기/색이 바뀌므로 bDynamic = true)
	VertexPC vertices[4] = {};
	const _u32 indices[] = { 0, 1, 2, 2, 1, 3 };

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPC), 4, true) ||
		!ib.Create(&device, indices, 6))
		{
		printf("버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 컴파일
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, InputDemoShaderSource(), pLayoutDescs, layoutCount))
	{
		printf("셰이더 컴파일 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		// === 폴링 방식 ===
		// 매 프레임 마우스 위치와 버튼 상태를 직접 읽는다.
		const vec2 center = PixelToNdc(input.MouseX(), input.MouseY(), window.Width(), window.Height());

		// 버튼 상태에 따른 색상 (누르고 있는 동안 계속 유지 = 폴링의 특징)
		color tintColor = color(0.9f, 0.9f, 0.9f, 1.0f);			// 기본: 흰색
		if (input.IsMouseDown(MouseButton::Left))
		{
			tintColor = color(0.95f, 0.3f, 0.3f, 1.0f);				// 왼클릭: 빨강
		}
		else if (input.IsMouseDown(MouseButton::Right))
		{
			tintColor = color(0.3f, 0.5f, 0.95f, 1.0f);				// 오른클릭: 파랑
		}

		input.NextFrame();	// 이번 프레임 입력 확정 (Pressed/Released 계산용)

		// 마우스 중심의 사각형 정점 4개 계산
		// (화면 비율 보정: x 반폭에 비해 y 반폭에 aspect를 곱해 정사각형 유지)
		const _f32 aspect = window.AspectRatio();
		vertices[0] = { vec3(center.x - halfSize, center.y + halfSize * aspect, 0.0f), tintColor };
		vertices[1] = { vec3(center.x + halfSize, center.y + halfSize * aspect, 0.0f), tintColor };
		vertices[2] = { vec3(center.x - halfSize, center.y - halfSize * aspect, 0.0f), tintColor };
		vertices[3] = { vec3(center.x + halfSize, center.y - halfSize * aspect, 0.0f), tintColor };
		vb.Update(&device, vertices, 4);

		device.BeginFrame(color(0.08f, 0.08f, 0.12f, 1.0f));

		vb.Bind(&device);
		ib.Bind(&device);
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 6. 정리: 등록했던 이벤트는 꼭 해제한다! (달링 참조 방지 습관)
	input.onMouseWheel.Unregister(1);
	input.onKeyPressed.Unregister(1);
	input.onMousePressed.Unregister(1);

	device.Finalize();
	window.Destroy();
}
