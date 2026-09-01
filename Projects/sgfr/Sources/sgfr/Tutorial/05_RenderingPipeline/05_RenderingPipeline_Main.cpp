/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:40:00 AM
 * 수정일: 8/23/2026 - 방향키 자유 전환 (처음 Legacy, 좌/우로 토글)
 * =====================
 * 05. 렌더링 파이프라인 (Rendering Pipeline) - 화면에 첫 삼각형!
 *
 * [렌더링 파이프라인이란?]
 * 정점 데이터가 화면 픽셀이 되기까지 거치는 GPU의 조립 라인.
 *
 * [정점 데이터] -> IA -> VS -> RS -> PS -> OM -> [화면]
 *
 * 1. IA (Input Assembler): 정점 버퍼에서 정점을 읽어 삼각형으로 묶는다.
 * 2. VS (Vertex Shader): 정점마다 실행. 위치를 클립 공간으로 변환한다.
 * 3. RS (Rasterizer): 삼각형이 덮는 픽셀들을 찾고, 정점 속성을 보간한다.
 * 4. PS (Pixel Shader): 픽셀마다 실행. 최종 색상을 계산한다.
 * 5. OM (Output Merger): 깊이 테스트/블렌딩을 거쳐 백버퍼에 기록한다.
 *
 * [이 튜토리얼의 목표]
 * 가장 작은 구성으로 삼각형 하나를 그려본다.
 * 필요한 것: 정점 3개 + 정점 버퍼 + 셰이더 + Draw 호출. 이게 전부다!
 *
 * [조작법]
 * - 처음에는 Legacy(기존 방식)로 시작한다.
 * - 방향키 좌(LEFT): Legacy 모드로 전환 (VertexBuffer/Shader 직접 바인딩)
 * - 방향키 우(RIGHT): Simple 모드로 전환 (목표 코드 gd.Create / gc.Set / DrawIndexed)
 * - ESC로 종료. 두 모드는 화면 결과가 동일하다.
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Main.h"
#include "sgfr/Tutorial/05_RenderingPipeline/05_RenderingPipeline_Function.h"

using namespace sgf;
using namespace jc;

// 렌더링 파이프라인 튜토리얼 진입점 - 단일 윈도우에서 Legacy/Simple 자유 전환
void RenderingPipeline_Main()
{
	PrintNdcExplanation();

	jc::Console::WriteLine("[05] 렌더링 파이프라인 - 처음은 Legacy, 방향키로 자유 전환");
	jc::Console::WriteLine("  LEFT  : Legacy (기존, 직접 바인딩)");
	jc::Console::WriteLine("  RIGHT : Simple (목표 코드, gd/gc 핸들)");
	jc::Console::WriteLine("  ESC   : 종료");
	jc::Console::WriteLine("");

	// 1. 윈도우 + 디바이스 (단일 인스턴스 공유)
	Window window;
	if (!window.Create("05. 렌더링 파이프라인 - [Legacy] (LEFT Legacy | RIGHT Simple | ESC 종료)", 800, 600))
	{
		jc::Console::WriteLine("윈도우 생성 실패!");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	// 최종 설계서 기준: 잘 알려진 인스턴스 + 명시적 수명
	GraphicDevice& gd = GraphicDevice::Get();
	if (!gd.Initialize())
	{
		jc::Console::WriteLine("그래픽 디바이스 초기화 실패!");
		window.Destroy();
		return;
	}
	if (!g_cResourceMgr.Initialize(&gd))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
	g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	if (!gd.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
		jc::Console::WriteLine("스왑체인 생성 실패!");
	g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}
	GraphicContext& gc = gd.Context();

	// 2. 공통 정점 데이터 (두 모드가 동일 삼각형 공유)
	const VertexPC vertices[] = {
		{ vec3(0.0f,  +0.5f, 0.0f), color(0xFF, 0x00, 0x00, 0xFF) }, // 위 (빨강)
		{ vec3(+0.5f, -0.5f, 0.0f), color(0x00, 0x00, 0xFF, 0xFF) }, // 오른쪽아래 (파랑)
		{ vec3(-0.5f, -0.5f, 0.0f), color(0x00, 0xFF, 0x00, 0xFF) }, // 왼쪽아래 (초록)
	};
	const _u32 indices[] = { 0, 1, 2 };

	// 3-A. Legacy 리소스 (직접 바인딩 경로 - 레거시 API)
	VertexBuffer vbLegacy;
	if (!vbLegacy.Create(&gd, vertices, 3, VertexPC::Decl()))
	{
		jc::Console::WriteLine("Legacy 정점 버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}
	_u64 vsShaderLegacy = gc.CreateVertexShader(TriangleShaderSource());
	_u64 psShaderLegacy = gc.CreatePixelShader(TriangleShaderSource());
	if (vsShaderLegacy == INVALID_RESOURCE_KEY || psShaderLegacy == INVALID_RESOURCE_KEY)
	{
		jc::Console::WriteLine("Legacy 셰이더 컴파일 실패!");
	g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}
	// Legacy InputLayout은 핸들+레이아웃 경로로 수동 지정 (SetInputLayout 자체로 동작, 실패 시 assert)

	// 3-B. Simple 리소스 (목표 코드 경로 - 핸들 기반, 분리형 VS/PS)
	const _u64 hVsSimple = gc.CreateVertexShader(TriangleShaderSource());
	const _u64 hPsSimple = gc.CreatePixelShader(TriangleShaderSource());
	const _u64 hVbSimple = gd.CreateVertexBuffer<VertexPC>(vertices, 3);
	const _u64 hIbSimple = gd.CreateIndexBuffer(indices, 3);
	if (hVsSimple == INVALID_RESOURCE_KEY || hPsSimple == INVALID_RESOURCE_KEY || hVbSimple == INVALID_RESOURCE_KEY || hIbSimple == INVALID_RESOURCE_KEY)
	{
		jc::Console::WriteLine("Simple 핸들 리소스 생성 실패!");
	g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("삼각형이 보이면 성공! [Legacy]로 시작 - RIGHT로 Simple, LEFT로 Legacy 전환 가능 (ESC 종료)");

	enum class Mode { Legacy, Simple };
	Mode mode = Mode::Legacy;

	// 4. 렌더 루프 - 단일 윈도우에서 모드 토글
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		// 방향키로 자유 전환 (IsKeyPressed는 엣지 트리거)
		if (input.IsKeyPressed(VK_LEFT) && mode != Mode::Legacy)
		{
			mode = Mode::Legacy;
			window.SetTitle("05. 렌더링 파이프라인 - [Legacy] (LEFT Legacy | RIGHT Simple | ESC 종료)");
			jc::Console::WriteLine("[전환] Legacy 모드 (직접 바인딩)");
		}
		if (input.IsKeyPressed(VK_RIGHT) && mode != Mode::Simple)
		{
			mode = Mode::Simple;
			window.SetTitle("05. 렌더링 파이프라인 - [Simple] (LEFT Legacy | RIGHT Simple | ESC 종료)");
			jc::Console::WriteLine("[전환] Simple 모드 (목표 코드 핸들)");
		}

		gd.BeginFrame(color::CORNFLOWER_BLUE);

		if (mode == Mode::Legacy)
		{
			// Legacy 경로: 포인터 기반 직접 바인딩 (수동 InputLayout 필수)
			vbLegacy.Bind(gc);
			gc.SetVertexShader(vsShaderLegacy);
			gc.SetPixelShader(psShaderLegacy);
			gc.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
			gc.Draw(3, 0);
		}
		else
		{
			// Simple 경로: 핸들 기반 (VS/VB 핸들로 InputLayout 수동 지정, 없으면 assert)
			gc.SetVertexShader(hVsSimple);
			gc.SetPixelShader(hPsSimple);
			gc.SetVertexBuffer(hVbSimple);
			gc.SetIndexBuffer(hIbSimple);
			gc.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
			gc.SetRasterizer(CullMode::cmNone, FillMode::fmSolid);
			gc.SetDepth(DepthMode::dmDisabled);
			gc.SetBlend(BlendMode::bmNone);
			gc.DrawIndexed(3, 0, 0);
		}

		gd.Present(true);
		input.NextFrame();
	}

	// 5. 정리 - gd.Finalize() 하나로 레지스트리/표면/컨텍스트 역순 정리
	g_cResourceMgr.Finalize();
	gd.Finalize();
	window.Destroy();
}
