/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 22. 렌더 오브젝트 루프 (RenderObject Loop)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. RenderObject = 메시 키 + 머티리얼 키 + 월드 행렬 + visible_, 그리기의 최소 단위
 * 2. Renderer3D::BeginScene(프레임 상수) 후 Draw(오브젝트)를 반복하는 표준 루프
 * 3. b0(프레임)/b1(오브젝트) 갱신을 Renderer3D가 전담한다 (12번의 자동화판)
 *
 * [Before/After 비교]
 * - Before: 객체마다 셰이더/버퍼/텍스처 바인딩 코드를 직접 나열
 * - After: "무엇을(메시) 어떻게(머티리얼) 어디에(월드)"만 채운 구조체 배열을
 * 돌면서 renderer.Draw(object) 한 줄씩 (수도 코드 §16~19 대응)
 *
 * [조작법]
 * - 1: 가운데 큰 큐브 보이기/숨기기 (bVisible_ 토글)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/22_RenderObjectLoop/22_RenderObjectLoop_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	constexpr _s32 ORBIT_CUBE_COUNT = 4;	// 주변을 도는 작은 큐브 개수
}

// 렌더 오브젝트 루프 튜토리얼을 실행한다. (BeginScene → Draw 반복 패턴)
void RenderObjectLoop_Main()
{
	_LogInfo_("[22] RenderObjectLoop 시작");
	// 1. 윈도우 + 디바이스 + 리소스 매니저 준비
	Window window;
	if (!window.Create("22. 렌더 오브젝트 루프 (1 가운데 큐브 토글, ESC 종료)", 800, 600))
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

	if (!g_cResourceMgr.Initialize(&device))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 2. 씬 렌더러 준비 (b0/b1 상수버퍼를 내부에서 관리) — Renderer3D (이전 이름 SceneRenderer 통합)
	Renderer3D renderer;
	if (!renderer.Initialize(&device))
	{
		jc::Console::WriteLine("씬 렌더러 초기화 실패!");
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 큐브 메시를 만들어 리소스 매니저에 등록 (이후는 키로만 참조)
	Mesh* pCube = dbg_new Mesh();
	if (!pCube->InitializeAsCube(&device, g_cResourceMgr.GetDefaultVertexShader3D()))
	{
		jc::Console::WriteLine("큐브 메시 생성 실패!");
		delete pCube;
		renderer.Finalize();
		g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}
	pCube->SetDebugName("UnitCube");
	const _u64 cubeMeshKey = g_cResourceMgr.Add(pCube);
	_LogInfo_("[22] 리소스 준비 완료 — 메시 키=%llu", (unsigned long long)cubeMeshKey);

	// 4. 그릴 목록 구성: 가운데 큰 큐브 1개 + 주변 궤도 큐브 4개
	// 월드 행렬은 매 프레임 갱신하므로 여기서는 키/머티리얼/visible_만 채운다.
	RenderObject objects[1 + ORBIT_CUBE_COUNT];
	for (_s32 i = 0; i < 1 + ORBIT_CUBE_COUNT; ++i)
	{
		objects[i].meshKey_ = cubeMeshKey;
		objects[i].materialKey_ = g_cResourceMgr.GetDefaultMaterial3DKey();
		objects[i].visible_ = true;
	}

	jc::Console::Write("RenderObject %d개를 BeginScene → Draw 루프로 그립니다!\n", 1 + ORBIT_CUBE_COUNT);

	// 5. 카메라는 고정, 객체만 회전시킨다.
	FrameConstants frame;
	frame.view_ = mat4::LookAtLH(vec3(0.0f, 2.5f, -5.0f), vec3::Zero(), vec3::Up());
	frame.projection_ = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);
	frame.cameraPosition_ = vec4(0.0f, 2.5f, -5.0f, 1.0f);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;
	_s32 frameCount = 0;	// 검증 로그용 프레임 카운터

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		if (input.IsKeyPressed('1'))
		{
			objects[0].visible_ = !objects[0].visible_;	// 보이기/숨기기는 플래그 하나로
		}
		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		// 6. 월드 행렬 갱신 (행벡터 규약: 왼쪽이 먼저 적용 → 회전 후 이동)
		objects[0].world_ = mat4::RotationY(elapsed);	// 가운데 큰 큐브: 제자리 회전

		for (_s32 i = 0; i < ORBIT_CUBE_COUNT; ++i)
		{
			const _f32 angle = elapsed * 1.5f + jc_math_pi * 2.0f * (_f32)i / (_f32)ORBIT_CUBE_COUNT;
			objects[1 + i].world_ =
			mat4::Scale(0.3f) *							// (1) 작게
			mat4::RotationY(elapsed * 3.0f) *				// (2) 제자리 회전
			mat4::Translation(2.0f, 0.0f, 0.0f) *			// (3) 반지름만큼 밀고
			mat4::RotationY(angle);						// (4) 중심 기준 공전
		}

		// 7. 표준 렌더 루프: BeginFrame → BeginScene(b0 한 번) → Draw들(b1씩) → Present
		device.BeginFrame(color(0x0F, 0x0F, 0x1A, 0xFF));
		device.Context().InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		renderer.BeginScene(frame);
	for (_s32 i = 0; i < 1 + ORBIT_CUBE_COUNT; ++i)
		{
			renderer.Draw(objects[i]);
		}
		renderer.EndScene();

		device.Present(true);
		++frameCount;
		if (frameCount == 1 || frameCount == 60 || frameCount == 300)
		{
			_LogDebug_("[22] 렌더 프레임 %d회 — Draw %d건", frameCount, 1 + ORBIT_CUBE_COUNT);
		}
	}

	// 8. 정리: 큐브 메시는 매니저 소유라 Finalize가 함께 소멸시킨다.
	renderer.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
	_LogInfo_("[22] RenderObjectLoop 종료 — 렌더 프레임 %d회", frameCount);
}
