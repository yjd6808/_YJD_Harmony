/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:40:00 AM
 * =====================
 * 05. 렌더링 파이프라인 (Rendering Pipeline) - 화면에 첫 삼각형!
 *
 * [렌더링 파이프라인이란?]
 *  정점 데이터가 화면 픽셀이 되기까지 거치는 GPU의 조립 라인.
 *
 *  [정점 데이터] -> IA -> VS -> RS -> PS -> OM -> [화면]
 *
 *  1. IA (Input Assembler)  : 정점 버퍼에서 정점을 읽어 삼각형으로 묶는다.
 *  2. VS (Vertex Shader)    : 정점마다 실행. 위치를 클립 공간으로 변환한다.
 *  3. RS (Rasterizer)       : 삼각형이 덮는 픽셀들을 찾고, 정점 속성을 보간한다.
 *  4. PS (Pixel Shader)     : 픽셀마다 실행. 최종 색상을 계산한다.
 *  5. OM (Output Merger)    : 깊이 테스트/블렌딩을 거쳐 백버퍼에 기록한다.
 *
 * [이 튜토리얼의 목표]
 *  가장 작은 구성으로 삼각형 하나를 그려본다.
 *  필요한 것: 정점 3개 + 정점 버퍼 + 셰이더 + Draw 호출. 이게 전부다!
 *
 * [조작법]
 *  - 빨간/초록/파란 꼭짓점을 가진 삼각형이 보인다. ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Main.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Function.h"

using namespace sgf;
using namespace jc;

// 렌더링 파이프라인 튜토리얼을 실행한다. (화면에 첫 삼각형 그리기)
void RenderingPipeline_Main()
{
	PrintNdcExplanation();

	// 1. 윈도우 + 디바이스 준비 (02, 04번에서 배운 내용)
	Window window;
	if (!window.Create(L"05. 렌더링 파이프라인 - 첫 삼각형 (ESC로 종료)", 800, 600))
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

	// 2. 정점 데이터 정의
	//    주의! 시계 방향(CW) 순서로 적어야 한다.
	//    DX11 기본 설정은 반시계 방향 삼각형을 "뒷면"으로 보고 그리지 않는다. (백페이스 컬링)
	const VertexPC vertices[] = {
		{ vec3(0.0f,  +0.5f, 0.0f), color(1.0f, 0.0f, 0.0f, 1.0f) },	// 위   (빨강)
		{ vec3(+0.5f, -0.5f, 0.0f), color(0.0f, 0.0f, 1.0f, 1.0f) },	// 오른쪽아래 (파랑)
		{ vec3(-0.5f, -0.5f, 0.0f), color(0.0f, 1.0f, 0.0f, 1.0f) },	// 왼쪽아래  (초록)
	};

	// 3. 정점 버퍼 생성: 정점 데이터를 GPU 메모리로 복사한다.
	//    CPU 메모리의 배열을 GPU가 직접 읽을 수 없기 때문에 복사가 필요하다.
	VertexBuffer vb;
	if (!vb.Create(&device, vertices, sizeof(VertexPC), 3))
	{
		printf("정점 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 컴파일
	//    입력 레이아웃(LayoutDescs)은 "C++ 구조체의 메모리 배치와 HLSL 입력을
	//    어떻게 연결할지" 알려주는 설명서다. (POSITION은 12바이트 오프셋 0, COLOR0는 오프셋 12...)
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	if (!shader.CompileFromString(&device, TriangleShaderSource(), pLayoutDescs, layoutCount))
	{
		printf("셰이더 컴파일 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	printf("삼각형이 보이면 성공! 세 꼭짓점 색이 부드럽게 섞이는 이유는 래스터라이저의 보간 때문이다.\n");

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		input.NextFrame();

		device.BeginFrame(color::CornflowerBlue());

		// === 그리기 순서: "무엇을(VB), 어떻게(Shader), 어떤 모양으로(Topology) 그릴지" 설정 후 Draw ===

		// (1) 정점 버퍼를 IA 단계에 묶는다.
		vb.Bind(&device);

		// (2) VS/PS/입력레이아웃을 파이프라인에 묶는다.
		shader.Bind(&device);

		// (3) 정점 3개씩 묶어 삼각형으로 해석하라고 지시한다.
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// (4) 드디어 그리기! "정점 3개를 0번부터 사용해 그려라"
		//     이 한 줄이 위의 모든 설정을 가지고 파이프라인 전체를 가동시킨다.
		device.Context()->Draw(3, 0);

		device.EndFrame(true);
	}

	// 6. 정리 (VertexBuffer/Shader는 소멸자에서 ComPtr이 자동 해제)
	device.Finalize();
	window.Destroy();
}
