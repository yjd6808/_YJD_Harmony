/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:40:00 AM
 * =====================
 * 14. 라이팅 (Lighting) - 람버트 확산광 + 주변광
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 법선(Normal)이 무엇이고 왜 필요한가
 * 2. 람버트 법칙: 밝기 = max(0, dot(N, -L))
 * 3. 상수 버퍼 여러 개(b0, b1)를 슬롯별로 쓰는 법
 * 4. 왜 큐브 정점이 8개가 아니라 24개가 되는가 (면마다 법선이 다르다!)
 *
 * [왜 정점 24개?]
 * 13번에서는 꼭짓점 8개로 충분했다. 색은 꼭짓점마다 하나면 되니까.
 * 하지만 법선은 '면'의 속성이다. 한 꼭짓점은 3개 면에 속하므로
 * 면마다 다른 법선을 주려면 정점을 면별로 분리해야 한다. (4개 x 6면 = 24개)
 *
 * [조작법]
 * - 회전하는 큐브에 빛이 비친다. 밝은 면/어두운 면이 구분되는 것을 관찰!
 * - ↑↓ 화살표: 주변광 세기 조절 / ESC: 종료
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/14_Lighting/14_Lighting_Main.h"
#include "sgfr/Tutorial/14_Lighting/14_Lighting_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 셰이더 CbTransform(b0)과 메모리 배치 일치 (mat4 x 2 = 128바이트)
	struct CbTransform
	{
		mat4 world_;			// 월드 행렬 (법선 변환용)
		mat4 worldViewProj_;	// 월드 x 뷰 x 투영 결합 행렬
	};

	// 셰이더 CbLight(b1)와 메모리 배치 일치 (16 + 16 = 32바이트)
	struct CbLight
	{
		vec3 lightDir_;			// 빛이 나아가는 방향 (정규화 필수!)
		_f32 ambient_;			// 주변광 세기 (0~1)
		vec4 baseColor_;		// 물체 기본 색상
	};
}

// 라이팅 튜토리얼을 실행한다. (람버트 확산광 + 주변광)
void Lighting_Main()
{
	PrintLambertExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("14. 라이팅 - 람버트 확산광 (↑↓ 주변광, ESC 종료)", 800, 600))
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

	// 2. 법선 포함 큐브 생성 (면별 정점 24개 + 인덱스 36개)
	VertexPNT vertices[24];
	_u32 indices[36];
	FillCubeWithNormals(vertices, indices);

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, 24, VertexPNT::Decl()) ||
		!ib.Create(&device, indices, 36))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 셰이더 + 상수 버퍼 2개 (변환용 b0, 조명용 b1)

	_u64 vsShader = device.Context().CreateVertexShader(LambertShaderSource());
	_u64 psShader = device.Context().CreatePixelShader(LambertShaderSource());
	ConstantBuffer<CbTransform> cbTransform;
	ConstantBuffer<CbLight> cbLight;
	if (vsShader == INVALID_RESOURCE_KEY || psShader == INVALID_RESOURCE_KEY ||
		!cbTransform.Create(&device) ||
		!cbLight.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 뷰/투영 행렬 (13번과 동일)
	const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.2f, -2.5f), vec3::Zero(), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;
	_f32 ambient = 0.15f;	// 주변광 기본값

	jc::Console::WriteLine("빛은 오른쪽 위 앞에서 대각선으로 내리쪼입니다. ↑↓로 주변광을 조절해보세요.");

	// 5. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		timer.Tick();
		const _f32 dt = timer.DeltaTime();
		elapsed += dt;

		// ↑↓로 주변광 조절 (누르고 있는 동안 연속 변화)
		if (input.IsKeyDown(VK_UP))   { ambient = Clamp(ambient + 0.5f * dt, 0.0f, 1.0f); }
		if (input.IsKeyDown(VK_DOWN)) { ambient = Clamp(ambient - 0.5f * dt, 0.0f, 1.0f); }

		input.NextFrame();

		device.BeginFrame(color(0x0D, 0x0D, 0x1A, 0xFF));

		// 큐브 회전 (빛은 고정, 물체만 회전 -> 면의 밝기가 계속 변한다)
		const mat4 world = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.3f);

		CbTransform cbT = {};
		cbT.world_ = world;
		cbT.worldViewProj_ = world * view * proj;
		cbTransform.UpdateAndBind(device.Context(), cbT, 0);	// register(b0)

		CbLight cbL = {};
		cbL.lightDir_ = vec3(-0.5f, -0.7f, 0.5f).Normalized();	// 오른쪽 위 앞 -> 왼쪽 아래 뒤 (광원은 오른쪽 위 앞쪽에 위치)
		cbL.ambient_ = ambient;
		cbL.baseColor_ = vec4(0.9f, 0.6f, 0.2f, 1.0f);			// 주황색 큐브
		cbLight.UpdateAndBind(device.Context(), cbL, 1);		// register(b1)

		vb.Bind(device.Context());
		ib.Bind(device.Context());
		device.Context().SetVertexShader(vsShader);
		device.Context().SetPixelShader(psShader);
		{
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		device.Context().DrawIndexed(36, 0, 0);

		device.Present(true);
	}

	// 6. 정리
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
