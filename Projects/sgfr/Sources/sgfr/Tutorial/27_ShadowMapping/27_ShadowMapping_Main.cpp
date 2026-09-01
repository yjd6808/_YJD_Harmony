/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:40:00 PM
 * =====================
 * 27. 그림자 매핑 (Shadow Mapping)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 그림자 매핑의 핵심: "빛의 입장에서 안 보이는 곳이 그림자"
 * 2. 깊이 전용 렌더 타깃(그림자 맵)을 만드는 법
 * 3. 2패스 렌더링: 빛 시점 깊이 기록 -> 카메라 시점 그림자 판정
 * 4. 그림자 여드름(acne)과 바이어스의 관계
 * 5. 방향광(태양)은 직교 투영을 쓰는 이유
 *
 * [Before/After 비교 뷰]
 * 화면이 노란 세로선으로 좌/우로 나뉜다. (같은 장면, 같은 라이팅)
 * - 왼쪽 (Before): 그림자 판정 없음 = 라이팅만 있는 세상
 * - 오른쪽(After): 그림자 매핑 적용 = 큐브 그림자가 바닥에 드리워진다
 * 그림자 하나가 "물체가 떠 있다/바닥에 붙어 있다" 같은 공간감을
 * 얼마나 만드는지 한 화면에서 직접 비교할 수 있다!
 *
 * [관찰 포인트]
 * - 왼쪽은 큐브가 공중에 떠 있는지 바닥에 있는지 구분이 안 된다
 * - 오른쪽은 그림자 덕분에 큐브의 높이가 바로 느껴진다
 * - 빛 방향을 돌리면 오른쪽 그림자만 반대편으로 이동한다
 * - 바이어스를 0으로 내리면 오른쪽 표면에만 줄무늬(여드름)이 생긴다!
 *
 * [조작법]
 * - 왼쪽/오른쪽 방향키: 빛 방향 회전
 * - 위/아래 방향키: 그림자 바이어스 증감 (0 ~ 0.01)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/27_ShadowMapping/27_ShadowMapping_Main.h"
#include "sgfr/Tutorial/27_ShadowMapping/27_ShadowMapping_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// HLSL의 CbDepth(b0)와 일치 (64바이트)
	struct CbDepth
	{
		mat4 lightWvp_;
	};

	// HLSL의 CbScene(b0)과 일치 (192바이트)
	struct CbScene
	{
		mat4 world_;
		mat4 wvp_;
		mat4 lightViewProj_;
	};

	// HLSL의 CbLight(b1)와 일치 (48바이트 = 16의 배수)
	struct CbLight
	{
		vec3 lightDir_;		// 빛이 나아가는 방향
		_f32 shadowBias_;	// 그림자 여드름 방지 보정값
		vec4 baseColor_;	// 물체 기본색
		_f32 splitPixelX_;	// Before/After 경계 픽셀 x (왼쪽=그림자 없음, 오른쪽=그림자 적용)
		_f32 padding_[3];	// 16바이트 정렬용 여백
	};
}

// 그림자 매핑 튜토리얼을 실행한다. (좌: 그림자 없음 / 우: 그림자 적용 Before/After 비교)
void ShadowMapping_Main()
{
	PrintShadowExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("27. 그림자 매핑 - 왼쪽: 그림자 없음 | 오른쪽: 그림자 적용 (←→ 빛, ↑↓ 바이어스, ESC)", 800, 600))
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
	if (!g_cResourceMgr.Initialize(&device))
	{
		jc::Console::WriteLine("리소스 매니저 초기화 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	if (!device.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
	jc::Console::WriteLine("스왑체인 생성 실패!");
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
	return;
	}

	// 2. 그림자 맵: 색 없이 깊이만 담는 1024x1024 렌더 타깃
	// 해상도가 클수록 그림자 경계가 선명해진다. (대신 메모리 증가)
	RenderTarget shadowMap;
	if (!shadowMap.CreateDepthOnly(&device, 1024, 1024))
	{
		jc::Console::WriteLine("그림자 맵 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 지오메트리: 바닥 평면 + 법선 있는 큐브
	VertexPNT planeVertices[4];
	_u32 planeIndices[6];
	FillGroundPlane(planeVertices, planeIndices, 3.0f);

	VertexPNT cubeVertices[24];
	_u32 cubeIndices[36];
	FillShadowCube(cubeVertices, cubeIndices);

	VertexBuffer planeVb;
	IndexBuffer planeIb;
	VertexBuffer cubeVb;
	IndexBuffer cubeIb;
	if (!planeVb.Create(&device, planeVertices, 4, VertexPNT::Decl()) ||
		!planeIb.Create(&device, planeIndices, 6) ||
		!cubeVb.Create(&device, cubeVertices, 24, VertexPNT::Decl()) ||
		!cubeIb.Create(&device, cubeIndices, 36))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 2종(깊이 전용/장면용) + 상수 버퍼 3종

	_u64 vsDepthShader = device.Context().CreateVertexShader(ShadowDepthShaderSource());
	_u64 psDepthShader = device.Context().CreatePixelShader(ShadowDepthShaderSource());
	_u64 vsSceneShader = device.Context().CreateVertexShader(ShadowSceneShaderSource());
	_u64 psSceneShader = device.Context().CreatePixelShader(ShadowSceneShaderSource());
	ConstantBuffer<CbDepth> cbDepth;
	ConstantBuffer<CbScene> cbScene;
	ConstantBuffer<CbLight> cbLight;
	if (vsDepthShader == INVALID_RESOURCE_KEY || psDepthShader == INVALID_RESOURCE_KEY || vsSceneShader == INVALID_RESOURCE_KEY || psSceneShader == INVALID_RESOURCE_KEY || !cbDepth.Create(&device) || !cbScene.Create(&device) || !cbLight.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 카메라(관찰자 시점)
	const mat4 view = mat4::LookAtLH(vec3(0.0f, 3.0f, -6.0f), vec3(0.0f, 0.5f, 0.0f), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);

	// 6. 빛 상태: 방위각(azimuth)으로 태양 방향을 돌린다.
	_f32 lightAzimuth = 0.8f;
	_f32 shadowBias = 0.002f;

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 7. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		timer.Tick();
		elapsed += timer.DeltaTime();

		// 빛 방향 회전: 누르고 있는 동안 연속 회전 (IsKeyDown)
		if (input.IsKeyDown(VK_LEFT))
		{
			lightAzimuth -= timer.DeltaTime() * 1.5f;
		}
		if (input.IsKeyDown(VK_RIGHT))
		{
			lightAzimuth += timer.DeltaTime() * 1.5f;
		}

		// 바이어스 조절: 한 번 누를 때마다 단계적으로 (IsKeyPressed)
		if (input.IsKeyPressed(VK_UP))
		{
			shadowBias = Clamp(shadowBias + 0.001f, 0.0f, 0.01f);
			jc::Console::Write("그림자 바이어스: %.4f\n", shadowBias);
		}
		if (input.IsKeyPressed(VK_DOWN))
		{
			shadowBias = Clamp(shadowBias - 0.001f, 0.0f, 0.01f);
			jc::Console::Write("그림자 바이어스: %.4f (0이면 오른쪽에서 여드름 관찰 가능!)\n", shadowBias);
		}

		input.NextFrame();

		// 빛 방향과 빛 시점 행렬 계산
		// 방향광(태양)은 무한히 멀리 있다고 가정 -> 평행광선 -> 직교 투영 사용!
		const vec3 lightDir = vec3(cosf(lightAzimuth), -1.2f, sinf(lightAzimuth)).Normalized();
		const vec3 lightPos = lightDir * -8.0f;	// 빛을 거슬러 올라간 가상 위치
		const mat4 lightView = mat4::LookAtLH(lightPos, vec3::Zero(), vec3::Up());
		const mat4 lightProj = mat4::OrthographicOffCenterLH(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 20.0f);
		const mat4 lightViewProj = lightView * lightProj;

		// 오브젝트 월드 행렬 (큐브는 회전 후 바닥 위로 띄움)
		const mat4 planeWorld = mat4::Identity();
		const mat4 cubeWorld = mat4::RotationY(elapsed * 0.5f) * mat4::Translation(0.0f, 0.75f, 0.0f);

		device.BeginFrame(color(0x00, 0x00, 0x00, 0xFF));

		// ---- 패스 1: 빛 시점에서 깊이만 그림자 맵에 기록 ----
		device.SetRenderTarget(&shadowMap);
		shadowMap.Clear(&device, color::BLACK);	// 깊이 전용이므로 깊이만 1.0으로 초기화된다

		device.Context().SetVertexShader(vsDepthShader);
		device.Context().SetPixelShader(psDepthShader);
		{
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);

		CbDepth cbD;

		// 바닥도 깊이에 포함 (바닥 근처에 다른 물체가 있다면 서로 그림자를 주고받을 수 있게)
		cbD.lightWvp_ = planeWorld * lightViewProj;
		cbDepth.UpdateAndBind(device.Context(), cbD, 0);
		planeVb.Bind(device.Context());
		planeIb.Bind(device.Context());
		device.Context().DrawIndexed(6, 0, 0);

		cbD.lightWvp_ = cubeWorld * lightViewProj;
		cbDepth.UpdateAndBind(device.Context(), cbD, 0);
		cubeVb.Bind(device.Context());
		cubeIb.Bind(device.Context());
		device.Context().DrawIndexed(36, 0, 0);

		// ---- 패스 2: 카메라 시점으로 그리며 그림자 판정 ----
		// 셰이더가 화면 왼쪽 절반은 그림자 판정을 건너뛰어 Before/After를 한 화면에 보여준다.
		device.SetRenderTarget(nullptr);	// 백버퍼 복귀 (그림자 맵 SRV 바인딩 준비)

		// 그림자 맵은 이웃 픽셀과 섞이면 깊이 값이 망가지므로 Point + Clamp 샘플러 사용
		device.Context().SetSampler(FilterMode::fmPoint, AddressMode::amClamp, 0);
		shadowMap.BindAsTexture(&device, 0);

		device.Context().SetVertexShader(vsSceneShader);
		device.Context().SetPixelShader(psSceneShader);
		{
		}

		CbLight cbL;
		cbL.lightDir_ = lightDir;
		cbL.shadowBias_ = shadowBias;
		cbL.splitPixelX_ = window.Width() * 0.5f;	// 화면 정가운데가 Before/After 경계
		cbL.padding_[0] = cbL.padding_[1] = cbL.padding_[2] = 0.0f;

		CbScene cbS;
		cbS.lightViewProj_ = lightViewProj;

		// 바닥 (연한 회색)
		cbS.world_ = planeWorld;
		cbS.wvp_ = planeWorld * view * proj;
		cbScene.UpdateAndBind(device.Context(), cbS, 0);
		cbL.baseColor_ = vec4(0.75f, 0.75f, 0.78f, 1.0f);
		cbLight.UpdateAndBind(device.Context(), cbL, 1);
		planeVb.Bind(device.Context());
		planeIb.Bind(device.Context());
		device.Context().DrawIndexed(6, 0, 0);

		// 큐브 (주황색)
		cbS.world_ = cubeWorld;
		cbS.wvp_ = cubeWorld * view * proj;
		cbScene.UpdateAndBind(device.Context(), cbS, 0);
		cbL.baseColor_ = vec4(0.9f, 0.5f, 0.2f, 1.0f);
		cbLight.UpdateAndBind(device.Context(), cbL, 1);
		cubeVb.Bind(device.Context());
		cubeIb.Bind(device.Context());
		device.Context().DrawIndexed(36, 0, 0);

		device.Present(true);
	}

	// 8. 정리
	shadowMap.Destroy();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
