/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:00:00 AM
 * =====================
 * 07. 정점/인덱스 버퍼 (Vertex & Index Buffer) - 사각형 그리기
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. GPU는 오직 삼각형만 그린다. 사각형 = 삼각형 2개.
 * 2. 인덱스 버퍼로 정점을 재활용하는 법 (꼭짓점 4개로 삼각형 2개 만들기)
 * 3. DrawIndexed 호출
 *
 * [사각형을 삼각형 2개로 쪼개는 법]
 *
 * 0 ---- 1 삼각형 A: 0 -> 1 -> 2 (시계 방향!)
 * | / | 삼각형 B: 2 -> 1 -> 3 (시계 방향!)
 * | / |
 * 2 ---- 3
 *
 * 정점 1과 2는 두 삼각형이 공유한다. 인덱스 버퍼가 없다면
 * 같은 정점을 또 적어야 하지만, 인덱스는 "번호로 재활용"한다.
 *
 * [조작법]
 * - 네 꼭짓점 색이 다른 사각형이 보인다. ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/07_VertexIndexBuffer/07_VertexIndexBuffer_Main.h"
#include "sgfr/Tutorial/07_VertexIndexBuffer/07_VertexIndexBuffer_Function.h"

using namespace sgf;
using namespace jc;

// 정점/인덱스 버퍼 튜토리얼을 실행한다. (삼각형 2개로 사각형 그리기)
void VertexIndexBuffer_Main()
{
	PrintIndexBufferBenefit();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"07. 정점/인덱스 버퍼 - 사각형 (ESC로 종료)", 800, 600))
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

	// 2. 정점 4개 (꼭짓점만!)
	// 주석의 그림과 같은 번호 순서다.
	const VertexPC vertices[] = {
		{ vec3(-0.5f, +0.5f, 0.0f), color(0xFF, 0x00, 0x00, 0xFF) },	// 0: 왼쪽위 (빨강)
		{ vec3(+0.5f, +0.5f, 0.0f), color(0x00, 0xFF, 0x00, 0xFF) },	// 1: 오른쪽위 (초록)
		{ vec3(-0.5f, -0.5f, 0.0f), color(0x00, 0x00, 0xFF, 0xFF) },	// 2: 왼쪽아래 (파랑)
		{ vec3(+0.5f, -0.5f, 0.0f), color(0xFF, 0xFF, 0x00, 0xFF) },	// 3: 오른쪽아래 (노랑)
	};

	// 3. 인덱스 6개 = 삼각형 2개
	// 둘 다 시계 방향이어야 앞면으로 인정된다! (백페이스 컬링 주의)
	const _u32 indices[] = {
		0, 1, 2,	// 삼각형 A
		2, 1, 3,	// 삼각형 B
	};

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

	// 4. 셰이더 컴파일
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, PassThroughShaderSource(), pLayoutDescs, layoutCount))
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("꼭짓점 4개 + 인덱스 6개로 사각형을 그렸습니다.");

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		device.BeginFrame(color::CORNFLOWER_BLUE);

		vb.Bind(&device);
		ib.Bind(&device);		// 인덱스 버퍼도 IA 단계에 묶는다.
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Draw 대신 DrawIndexed!
		// "인덱스 6개를 0번부터 사용해 그려라. 정점 번호에 더할 값(BaseVertex)은 0"
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 6. 정리
	device.Finalize();
	window.Destroy();
}
