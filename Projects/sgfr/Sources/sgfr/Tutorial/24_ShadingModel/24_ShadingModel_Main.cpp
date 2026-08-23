/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:10:00 PM
 * =====================
 * 24. 셰이딩 모델 (Shading Model)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 셰이딩 = 빛의 반사를 근사하는 수식 (주변광/확산광/정반사광)
 * 2. 램버트 / 퐁 / 블린-퐁의 차이와 계산 방법
 * 3. 정반사광에는 카메라 위치(시선)가 필요하다는 사실
 * 4. UV 구 지오메트리를 직접 만들어 그리는 법 (jc::Vector 활용)
 *
 * [관찰 포인트]
 * - 1번(램버트)은 반짝임이 없다. 2/3번은 하이라이트가 생긴다!
 * - 퐁과 블린-퐁은 비슷하지만 하이라이트 모양이 미묘하게 다르다
 * - 방향키로 SpecPower를 바꿔가며 하이라이트 크기 변화를 보자
 *
 * [조작법]
 * - 1: 램버트 / 2: 퐁 / 3: 블린-퐁
 * - 위/아래 방향키: 하이라이트 날카로움 조절 (2~256)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/24_ShadingModel/24_ShadingModel_Main.h"
#include "sgfr/Tutorial/24_ShadingModel/24_ShadingModel_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// HLSL의 CbTransform(b0)과 일치 (128바이트)
	struct CbTransform
	{
		mat4 world_;
		mat4 wvp_;
	};

	// HLSL의 CbShading(b1)과 일치 (48바이트 = 16의 배수)
	// float3 + float 1개가 정확히 16바이트 한 칸을 채운다.
	struct CbShading
	{
		vec3 lightDir_;		// 빛이 나아가는 방향
		_s32 mode_;			// 0=램버트, 1=퐁, 2=블린-퐁
		vec3 cameraPos_;	// 월드 공간 카메라 위치
		_f32 specPower_;	// 하이라이트 날카로움
		vec4 baseColor_;	// 물체 기본색
	};

	// 창 제목에 표시할 모드 이름표 (gMode 값 순서와 일치)
	const char* s_szModeNames[] = { "램버트(확산만)", "퐁(R·V)", "블린-퐁(N·H)" };
}

// 셰이딩 모델 튜토리얼을 실행한다. (램버트/퐁/블린-퐁)
void ShadingModel_Main()
{
	PrintShadingExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("24. 셰이딩 모델 (1/2/3 모드, ↑↓ 날카로움, ESC 종료)", 800, 600))
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

	// 2. UV 구 지오메트리 생성 (32등분이면 충분히 둥글다)
	jc::Vector<VertexPNT> vertices;
	jc::Vector<_u32> indices;
	GenerateSphere(vertices, indices, 1.0f, 32, 32);

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices.Source(), sizeof(VertexPNT), static_cast<UINT>(vertices.Size())) ||
		!ib.Create(&device, indices.Source(), static_cast<UINT>(indices.Size())))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 셰이더 + 상수 버퍼 2개 (b0: 변환, b1: 셰이딩 설정)
	VertexLayoutSpan pLayoutDescs = VertexPNT::Layout();

	_u32 vsShader = device.Context().CreateVertexShader(ShadingShaderSource());
	_u32 psShader = device.Context().CreatePixelShader(ShadingShaderSource());
	ConstantBuffer<CbTransform> cbTransform;
	ConstantBuffer<CbShading> cbShading;
	if (vsShader == INVALID_HANDLE || psShader == INVALID_HANDLE ||
		!cbTransform.Create(&device) ||
		!cbShading.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 카메라: 정반사광 계산에 카메라 위치가 직접 쓰이므로 변수로 보관한다.
	const vec3 cameraPos(0.0f, 0.8f, -3.0f);
	const mat4 view = mat4::LookAtLH(cameraPos, vec3::Zero(), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);

	// 5. 셰이딩 상태 (키 입력으로 바꾼다)
	_s32 mode = 2;			// 시작은 블린-퐁 (현대 표준)
	_f32 specPower = 32.0f;

	auto UpdateTitle = [&]()
	{
		jc::String szTitle = jc::StringUtil::Format("24. 셰이딩 모델 - %s / 날카로움 %.0f (1/2/3, ↑↓, ESC)", s_szModeNames[mode], specPower);
		window.SetTitle(szTitle);
	};
	UpdateTitle();

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		bool bChanged = false;
		if (input.IsKeyPressed('1')) { mode = 0; bChanged = true; }
		if (input.IsKeyPressed('2')) { mode = 1; bChanged = true; }
		if (input.IsKeyPressed('3')) { mode = 2; bChanged = true; }
		if (input.IsKeyPressed(VK_UP))   { specPower = Clamp(specPower * 2.0f, 2.0f, 256.0f); bChanged = true; }
		if (input.IsKeyPressed(VK_DOWN)) { specPower = Clamp(specPower * 0.5f, 2.0f, 256.0f); bChanged = true; }
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0x14, 0x14, 0x1F, 0xFF));

		// 구는 천천히 자전, 빛은 구 주위를 돈다 -> 명암 경계가 움직이는 게 보인다.
		const mat4 world = mat4::RotationY(elapsed * 0.3f);
		const _f32 lightAngle = elapsed * 0.7f;
		const vec3 lightDir = vec3(cosf(lightAngle), -0.6f, sinf(lightAngle)).Normalized();

		CbTransform cbT;
		cbT.world_ = world;
		cbT.wvp_ = world * view * proj;
		cbTransform.UpdateAndBind(device.Context(), cbT, 0);

		CbShading cbS;
		cbS.lightDir_ = lightDir;
		cbS.mode_ = mode;
		cbS.cameraPos_ = cameraPos;
		cbS.specPower_ = specPower;
		cbS.baseColor_ = vec4(0.9f, 0.45f, 0.2f, 1.0f);	// 주황색 도자기 느낌
		cbShading.UpdateAndBind(device.Context(), cbS, 1);

		vb.Bind(device.Context());
		ib.Bind(device.Context());
		device.Context().SetVertexShader(vsShader);
		device.Context().SetPixelShader(psShader);
		{
			device.Context().SetInputLayout(vsShader, pLayoutDescs);
		}
		device.Context().SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		device.Context().DrawIndexed(static_cast<UINT>(indices.Size()), 0, 0);

		device.Present(true);
	}

	// 7. 정리
	device.Finalize();
	window.Destroy();
}
