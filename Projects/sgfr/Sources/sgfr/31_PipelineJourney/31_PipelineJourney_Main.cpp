/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 31. 파이프라인 여행 (Pipeline Journey) - 캅스톤
 *
 * [이 튜토리얼의 목표]
 *  지금까지 배운 모든 개념을 하나의 프로그램으로 연결한다.
 *  각 단계 주석의 [§N]은 설계 기준이 된 수도 코드 문서의 항목 번호다.
 *  (§1 윈도우 ~ §20 메인 루프까지 전 과정을 v3 API로 재현한다)
 *
 * [흙름 요약]
 *  §1 윈도우 → §2~3 디바이스/컨텍스트 → §4~6 스왑체인/깊이버퍼(엔진 내부)
 *  → §7~10 상태/셰이더/머티리얼 → §11 메시 → §12 렌더 오브젝트
 *  → §13 프레임 상수 → §14 뷰포트 → §15~19 프레임 시작/바인딩/드로우 → §20 메인 루프
 *
 * [조작법]
 *  - ESC: 종료 (큐브와 체커 바닥이 함께 도는 장면)
 */

#include "Core.h"
#include "sgfr/31_PipelineJourney/31_PipelineJourney_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 체커보드 텍스처 픽셀을 채운다. (바닥용)
	void FillChecker(_u8* _pOutPixels, int _width, int _height)
	{
		for (int y = 0; y < _height; ++y)
		{
			for (int x = 0; x < _width; ++x)
			{
				const bool bLight = (((x / 8) + (y / 8)) % 2) == 0;
				_u8* pPixel = _pOutPixels + (y * _width + x) * 4;
				pPixel[0] = bLight ? 180 : 60;
				pPixel[1] = bLight ? 180 : 60;
				pPixel[2] = bLight ? 200 : 90;
				pPixel[3] = 255;
			}
		}
	}
}

// 파이프라인 여행 튜토리얼을 실행한다. (수도 코드 전 항목을 v3 API로 재현)
void PipelineJourney_Main()
{
	// [§1] 윈도우 초기화: window.init(1280, 720)
	Window window;
	if (!window.Create(L"31. 파이프라인 여행 (ESC 종료)", 1280, 720))
	{
		printf("윈도우 생성 실패!\n");
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	// [§2~§6] 디바이스 + 컨텍스트 + 스왑체인 + 백버퍼 RTV + 깊이버퍼 DSV
	//  - 수도 코드에서는 다섯 단계였지만, v3에서는 device.Initialize(hwnd, w, h) 한 번이다.
	//  - 분리된 GraphicsContext는 device.GetContext()로 얻는다. (생성과 바인딩의 분리!)
	GraphicDevice device;
	if (!device.Initialize(window.Handle(), window.Width(), window.Height()))
	{
		printf("그래픽 디바이스 초기화 실패!\n");
		window.Destroy();
		return;
	}
	GraphicContext& context = device.GetContext();

	// 리소스 매니저: 디폴트 셰이더/머티리얼이 여기서 준비된다. ([§8~§9]의 완성형)
	if (!g_cResourceMgr.Initialize(&device))
	{
		printf("리소스 매니저 초기화 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// [§13] 프레임 상수버퍼 + [§16~§19] 바인딩/드로우 도우미 = SceneRenderer
	SceneRenderer renderer;
	if (!renderer.Initialize(&device))
	{
		printf("씨 렌더러 초기화 실패!\n");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// [§9] 텍스처 생성 후 매니저 등록 (바닥 체커)
	_u8 pixels[64 * 64 * 4];
	FillChecker(pixels, 64, 64);

	Texture* pChecker = dbg_new Texture();
	if (!pChecker->CreateFromMemory(&device, pixels, 64, 64))
	{
		printf("텍스처 생성 실패!\n");
		delete pChecker;
		renderer.Finalize();
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pChecker->SetDebugName("JourneyChecker");
	const _u64 checkerKey = g_cResourceMgr.Add(pChecker, "memory://journey_checker");

	// [§9] 머티리얼 구성: 셰이더 + 상태 4종 + 텍스처 슬롯 (값 하나로 묶인다)
	//  - 바닥용: 디폴트 3D 셰이더 + 체커 텍스처
	Material* pFloorMaterial = dbg_new Material();
	if (!pFloorMaterial->Initialize(&device))
	{
		printf("머티리얼 초기화 실패!\n");
		delete pFloorMaterial;
		renderer.Finalize();
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pFloorMaterial->SetVertexShaderKey(g_cResourceMgr.GetDefaultVertexShader2DKey());
	pFloorMaterial->SetPixelShaderKey(g_cResourceMgr.GetDefaultPixelShader2DKey());
	pFloorMaterial->SetTextureKey(0, checkerKey);				// [§9] texture[8] 슬롯 중 t0
	pFloorMaterial->SetRasterizer(CullMode::cmBack);			// [§7] 래스터라이저 상태
	pFloorMaterial->SetBlend(BlendMode::bmNone);				// [§7] 블렌드 상태
	pFloorMaterial->SetDepth(DepthMode::dmReadWrite);			// [§7] 깊이 상태
	pFloorMaterial->SetSampler(FilterMode::fmLinear);			// [§7] 샘플러 상태
	const _u64 floorMaterialKey = g_cResourceMgr.Add(pFloorMaterial);

	// [§10~§11] 메시: VB + IB + 입력 레이아웃 + 토폴로지를 한 덩어리로
	Mesh* pCubeMesh = dbg_new Mesh();
	Mesh* pQuadMesh = dbg_new Mesh();
	if (!pCubeMesh->InitializeAsCube(&device, g_cResourceMgr.GetDefaultVertexShader3D()) ||
		!pQuadMesh->InitializeAsQuad2D(&device, g_cResourceMgr.GetDefaultVertexShader2D()))
	{
		printf("메시 생성 실패!\n");
		delete pCubeMesh;
		delete pQuadMesh;
		renderer.Finalize();
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pCubeMesh->SetDebugName("JourneyCube");
	pQuadMesh->SetDebugName("JourneyFloor");
	const _u64 cubeMeshKey = g_cResourceMgr.Add(pCubeMesh);
	const _u64 quadMeshKey = g_cResourceMgr.Add(pQuadMesh);

	// [§12] 렌더 오브젝트: 무엇을/어떻게/어디에
	RenderObject cube;
	cube.meshKey_ = cubeMeshKey;
	cube.materialKey_ = g_cResourceMgr.GetDefaultMaterial3DKey();

	RenderObject floor;
	floor.meshKey_ = quadMeshKey;
	floor.materialKey_ = floorMaterialKey;

	// [§13] 프레임 데이터: 뷰/투영/카메라 위치
	FrameConstants frame;
	frame.view_ = mat4::LookAtLH(vec3(0.0f, 2.0f, -4.0f), vec3::Zero(), vec3::Up());
	frame.projection_ = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);
	frame.cameraPosition_ = vec4(0.0f, 2.0f, -4.0f, 1.0f);

	// [§14] 뷰포트는 device.Initialize가 전체 화면으로 잡아둔다.
	//  (반읐만 쓰고 싶다면 context.SetViewport(Viewport(...))로 언제든 바꿀 수 있다)

	printf("수도 코드 §1~§20이 이 파일 하나에 전부 들어있습니다. 주석의 [§N]을 따라가세요!\n");

	// [§20] 메인 루프: while (window.isRunning())
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

		// 월드 행렬 갱신 (행벡터 규약: 회전 후 이동)
		cube.world_ = mat4::RotationY(elapsed) * mat4::Translation(0.0f, 0.5f, 0.0f);
		floor.world_ = mat4::Scale(6.0f) * mat4::RotationX(jc_math_deg2rad(90.0f)) * mat4::Translation(0.0f, -0.5f, 0.0f);

		// [§15] BeginFrame: 백버퍼/깊이버퍼 클리어 + 뷰포트/렌더타겟 장착
		device.BeginFrame(color(0.08f, 0.08f, 0.12f, 1.0f));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		// [§16] UpdateFrameConstant: 프레임당 한 번 b0 갱신
		renderer.BeginScene(frame);

		// [§17~§19] BindMaterial + BindMesh + UpdateObjectConstant + Draw 가 한 줄씩
		renderer.Draw(floor);
		renderer.Draw(cube);

		// Present: 그린 결과를 화면으로 (GPU 파이프라인 여행의 종착역!)
		device.EndFrame(true);
	}

	// 정리: 등록한 리소스(메시/머티리얼/텍스처)는 매니저 Finalize가 일괄 소멸시킨다.
	renderer.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
