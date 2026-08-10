/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 23. 스테이트 캐시 (State Cache)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 같은 값을 다시 바인딩하는 API 호출은 GraphicContext가 알아서 건너뛴다
 *  2. GetApiCallCount / GetSkippedCallCount로 절감 효과를 직접 수치로 본다
 *  3. 실전 렌더러에 스테이트 캐시가 필수인 이유 (GPU 파이프라인 설명서의 결론!)
 *
 * [Before/After 비교]
 *  - Before(v2): 그리기 함수마다 모든 상태를 무조건 다시 바인딩 (중복 호출 그대로 전달)
 *  - After (v3): 컨텍스트가 마지막 값을 기억해 같은 값이면 D3D 호출을 생략
 *
 * [조작법]
 *  - 1: 캐시 무력화 토글 (매 Draw 앞 InvalidateCache 호출 = v2 방식 흉내내기)
 *  - 창 제목에서 프레임당 API 호출/생략 횟수를 확인하세요
 *  - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/23_StateCache/23_StateCache_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	constexpr int CUBE_GRID_SIZE = 10;	// 10x10 = 100개 큐브
}

// 스테이트 캐시 튜토리얼을 실행한다. (100개 큐브로 중복 바인딩 절감 관찰)
void StateCache_Main()
{
	// 1. 윈도우 + 디바이스 + 리소스 매니저 + 씨 렌더러 준비
	Window window;
	if (!window.Create(L"23. 스테이트 캐시 (1 캐시 무력화 토글, ESC 종료)", 800, 600))
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

	if (!g_cResourceMgr.Initialize(&device))
	{
		printf("리소스 매니저 초기화 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	SceneRenderer renderer;
	if (!renderer.Initialize(&device))
	{
		printf("씨 렌더러 초기화 실패!\n");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 2. 큐브 메시 하나를 100개 객체가 공유한다. (캐시 효과가 극대화되는 조건)
	Mesh* pCube = dbg_new Mesh();
	if (!pCube->InitializeAsCube(&device, g_cResourceMgr.GetDefaultVertexShader3D()))
	{
		printf("큐브 메시 생성 실패!\n");
		delete pCube;
		renderer.Finalize();
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pCube->SetDebugName("CacheDemoCube");
	const _u64 cubeMeshKey = g_cResourceMgr.Add(pCube);

	RenderObject object;
	object.meshKey_ = cubeMeshKey;
	object.materialKey_ = g_cResourceMgr.GetDefaultMaterial3DKey();

	printf("같은 메시/머티리얼 %d개를 그립니다. 창 제목의 수치를 보세요!\n", CUBE_GRID_SIZE * CUBE_GRID_SIZE);
	printf("1번 키로 캐시를 무력화하면 v2처럼 매번 전부 바인딩합니다.\n");

	// 3. 카메라: 그리드 전체가 보이도록 위에서 내려다본다.
	FrameConstants frame;
	frame.view_ = mat4::LookAtLH(vec3(0.0f, 12.0f, -14.0f), vec3::Zero(), vec3::Up());
	frame.projection_ = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);
	frame.cameraPosition_ = vec4(0.0f, 12.0f, -14.0f, 1.0f);

	GraphicContext& context = device.GetContext();
	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;
	bool bCacheDisabled = false;

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		if (input.IsKeyPressed('1'))
		{
			bCacheDisabled = !bCacheDisabled;
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0.06f, 0.06f, 0.1f, 1.0f));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다
		context.ResetStats();		// 이번 프레임 통계만 재다

		renderer.BeginScene(frame);

		for (int z = 0; z < CUBE_GRID_SIZE; ++z)
		{
			for (int x = 0; x < CUBE_GRID_SIZE; ++x)
			{
				if (bCacheDisabled)
				{
					context.InvalidateCache();	// v2 흉내내기: 매번 "처음 그리는 척" 하기
				}

				RenderObject& drawn = object;
				drawn.world_ =
					mat4::Scale(0.35f) *
					mat4::RotationY(elapsed + (_f32)(x + z) * 0.2f) *
					mat4::Translation((_f32)(x - CUBE_GRID_SIZE / 2) * 1.5f, 0.0f, (_f32)(z - CUBE_GRID_SIZE / 2) * 1.5f);
				renderer.Draw(drawn);
			}
		}

		// 4. 통계를 창 제목으로 출력 (API 호출 수 vs 캐시가 생략한 수)
		wchar_t szTitle[256];
		swprintf_s(szTitle, L"23. 스테이트 캐시 [%s] - API 호출: %llu, 생략: %llu (1 토글, ESC 종료)",
			bCacheDisabled ? L"캐시 꺼짐" : L"캐시 켜짐",
			context.GetApiCallCount(),
			context.GetSkippedCallCount());
		window.SetTitle(szTitle);

		device.EndFrame(true);
	}

	// 5. 정리
	renderer.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
