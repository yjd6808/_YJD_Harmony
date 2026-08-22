/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:30:00 PM
 * =====================
 * 26. 포스트 프로세싱 (Post Processing)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 장면 전체를 렌더 타깃에 그리고 후보정하는 2단계 구조
 * 2. 화면 전체 사각형(Fullscreen Quad)이란 무엇인가
 * 3. 그레이/세피아/반전/비네트/물결 효과의 수식과 원리
 * 4. 상수 버퍼로 셰이더 동작을 실시간으로 바꾸는 법
 *
 * [Before/After 비교 슬라이더]
 * 노란 세로 경계선 왼쪽은 원본(Before), 오른쪽은 효과 적용(After)이다.
 * 왼쪽/오른쪽 방향키로 경계를 실시간으로 좀이며 같은 장면의
 * 보정 전/후를 한 화면에서 직접 비교할 수 있다. (사진 앱 필터 미리보기와 같은 구조!)
 *
 * [관찰 포인트]
 * - 장면(회전 큐브)은 그대로인데 화면 느낌만 확 바뀜다!
 * - 효과는 픽셀 단위로 적용되므로 장면 복잡도와 무관하다
 * - 경계를 끝까지 밀면 화면 전체가 원본/효과 하나로 채워진다
 *
 * [조작법]
 * - 0: 원본 / 1: 그레이 / 2: 세피아 / 3: 반전 / 4: 비네트 / 5: 물결
 * - 왼쪽/오른쪽 방향키: Before/After 경계 이동
 * - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/26_PostProcess/26_PostProcess_Main.h"
#include "sgfr/Tutorial/26_PostProcess/26_PostProcess_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// 셰이더/큐브 공용 사용

using namespace sgf;
using namespace jc;

namespace
{
	// HLSL의 CbTransform(b0)과 일치 (64바이트)
	struct CbTransform
	{
		mat4 wvp_;
	};

	// HLSL의 CbPost(b0)와 일치 (16바이트)
	struct CbPost
	{
		_s32 mode_;		// 0=원본, 1=그레이, 2=세피아, 3=반전, 4=비네트, 5=물결
		_f32 time_;		// 물결 애니메이션용 누적 시간
		_f32 split_;	// Before/After 경계 (UV x, 0~1)
		_f32 padding_;	// 16바이트 정렬용 여백
	};

	// 창 제목에 표시할 효과 이름표 (gMode 값 순서와 일치)
	const char* s_szEffectNames[] = { "원본", "그레이", "세피아", "색 반전", "비네트", "물결" };
}

// 포스트 프로세싱 튜토리얼을 실행한다. (좌: 원본 / 우: 효과, 이동형 경계 슬라이더)
void PostProcess_Main()
{
	PrintPostProcessExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("26. 포스트 프로세싱 (0~5 효과, ←→ 경계, ESC 종료)", 800, 600))
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

	// 2. 화면과 같은 크기의 렌더 타깃: 장면이 먼저 여기에 그려진다.
	RenderTarget sceneTarget;
	if (!sceneTarget.Create(&device, window.Width(), window.Height()))
	{
		jc::Console::WriteLine("렌더 타깃 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 장면용 큐브 + 화면 전체 사각형 지오메트리
	VertexPC cubeVertices[8];
	_u32 cubeIndices[36];
	FillColorCube(cubeVertices, cubeIndices);

	VertexPTC quadVertices[4];
	_u32 quadIndices[6] = { 0, 1, 2, 2, 1, 3 };
	FillFullscreenQuad(quadVertices);

	VertexBuffer cubeVb;
	IndexBuffer cubeIb;
	VertexBuffer quadVb;
	IndexBuffer quadIb;
	if (!cubeVb.Create(&device, cubeVertices, sizeof(VertexPC), 8) ||
		!cubeIb.Create(&device, cubeIndices, 36) ||
		!quadVb.Create(&device, quadVertices, sizeof(VertexPTC), 4) ||
		!quadIb.Create(&device, quadIndices, 6))
		{
		jc::Console::WriteLine("버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 2종 + 상수 버퍼 2종
	UINT cubeLayoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pCubeLayout = VertexPC::LayoutDescs(&cubeLayoutCount);
	UINT quadLayoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pQuadLayout = VertexPTC::LayoutDescs(&quadLayoutCount);

	Shader sceneShader;
	Shader postShader;
	ConstantBuffer<CbTransform> cbTransform;
	ConstantBuffer<CbPost> cbPost;
	if (!sceneShader.CompileFromString(&device, ColorTransformShaderSource(), pCubeLayout, cubeLayoutCount) ||
		!postShader.CompileFromString(&device, PostProcessShaderSource(), pQuadLayout, quadLayoutCount) ||
		!cbTransform.Create(&device) ||
		!cbPost.Create(&device))
		{
		jc::Console::WriteLine("셰이더/상수 버퍼 생성 실패!");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 카메라
	const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.5f, -4.0f), vec3::Zero(), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);

	// 6. 현재 효과 모드 + Before/After 경계 위치
	_s32 effectMode = 1;
	_f32 split = 0.5f;	// 0.5 = 화면 정가운데

	auto UpdateTitle = [&]()
	{
		jc::String szTitle = jc::StringUtil::Format("26. 포스트 프로세싱 - 왼쪽: 원본 | 오른쪽: %s (경계 %d%%) (0~5, ←→, ESC)", s_szEffectNames[effectMode], (_s32)(split * 100.0f));
		window.SetTitle(szTitle);
	};
	UpdateTitle();

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
		const _f32 dt = timer.DeltaTime();
		elapsed += dt;

		bool bChanged = false;
		for (_s32 k = 0; k <= 5; ++k)
		{
			// '0'~'5' 숫자 키: 문자 코드와 가상 키 코드가 같다.
			if (input.IsKeyPressed('0' + k))
			{
				effectMode = k;
				bChanged = true;
			}
		}

		// 경계 슬라이더: 누르고 있는 동안 부드럽게 이동 (IsKeyDown)
		if (input.IsKeyDown(VK_LEFT))  { split = Clamp(split - 0.4f * dt, 0.02f, 0.98f); bChanged = true; }
		if (input.IsKeyDown(VK_RIGHT)) { split = Clamp(split + 0.4f * dt, 0.02f, 0.98f); bChanged = true; }
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		// ---- 패스 1: 장면을 렌더 타깃에 그린다 ----
		device.BeginFrame(color(0x00, 0x00, 0x00, 0xFF));

		device.SetRenderTarget(&sceneTarget);
		sceneTarget.Clear(&device, color(0x1F, 0x1A, 0x33, 0xFF));

		const mat4 world = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.3f);
		CbTransform cbT;
		cbT.wvp_ = world * view * proj;
		cbTransform.UpdateAndBind(&device, cbT, 0);

		cubeVb.Bind(&device);
		cubeIb.Bind(&device);
		sceneShader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.Context()->DrawIndexed(36, 0, 0);

		// ---- 패스 2: 렌더 타깃을 후보정해서 백버퍼에 출력 ----
		// 셰이더가 gSplit 왼쪽은 원본, 오른쪽은 효과를 적용해 Before/After를 한 화면에 보여준다.
		device.SetRenderTarget(nullptr);	// 화면으로 복귀 (SRV 언바인드도 엔진이 처리)

		CbPost cbP;
		cbP.mode_ = effectMode;
		cbP.time_ = elapsed;
		cbP.split_ = split;
		cbP.padding_ = 0.0f;
		cbPost.UpdateAndBind(&device, cbP, 0);

		device.SetSampler(GraphicDevice::SamplerFilter::fmLinear, GraphicDevice::SamplerAddress::amClamp, 0);
		sceneTarget.BindAsTexture(&device, 0);

		quadVb.Bind(&device);
		quadIb.Bind(&device);
		postShader.Bind(&device);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 8. 정리
	sceneTarget.Destroy();
	device.Finalize();
	window.Destroy();
}
