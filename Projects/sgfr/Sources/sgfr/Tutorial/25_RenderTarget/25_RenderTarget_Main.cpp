/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:20:00 PM
 * =====================
 * 25. 렌더 타깃 (Render Target)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 백버퍼가 아닌 "내가 만든 텍스처"에 장면을 그리는 법
 *  2. 그려진 결과를 다시 텍스처로 샘플링해서 화면에 붙이는 법
 *  3. 한 프레임에 여러 번의 "패스(Pass)"로 나누어 그리는 구조
 *  4. 미니맵/백미러/포스트 프로세싱/그림자의 공통 토대 익히기
 *
 * [관찰 포인트]
 *  - 가운데 큐브: 일반 카메라 시점 (백버퍼에 직접)
 *  - 오른쪽 위 사각형: 같은 큐브를 하늘에서 내려다본 미니맵 (렌더 타깃)
 *  - 둘은 같은 지오메트리지만 카메라만 다르다!
 *
 * [조작법]
 *  - ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/25_RenderTarget/25_RenderTarget_Main.h"
#include "sgfr/Tutorial/25_RenderTarget/25_RenderTarget_Function.h"
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
}

// 렌더 타깃 튜토리얼을 실행한다. (화면 밖 텍스처에 그리기 + 미니맵)
void RenderTarget_Main()
{
	PrintRenderTargetExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"25. 렌더 타깃 (오른쪽 위 = 미니맵, ESC 종료)", 800, 600))
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

	// 2. 렌더 타깃: 256x256 텍스처 + 전용 깊이 버퍼가 함께 만들어진다.
	RenderTarget miniMapTarget;
	if (!miniMapTarget.Create(&device, 256, 256))
	{
		printf("렌더 타깃 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 큐브 지오메트리 (장면용)
	VertexPC cubeVertices[8];
	_u32 cubeIndices[36];
	FillColorCube(cubeVertices, cubeIndices);

	VertexBuffer cubeVb;
	IndexBuffer cubeIb;
	if (!cubeVb.Create(&device, cubeVertices, sizeof(VertexPC), 8) ||
		!cubeIb.Create(&device, cubeIndices, 36))
		{
		printf("큐브 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 미니맵 출력용 사각형: 오른쪽 위 구석, 화면 비율(800:600) 보정으로 정사각형 유지
	VertexPTC quadVertices[4];
	_u32 quadIndices[6] = { 0, 1, 2, 2, 1, 3 };
	FillScreenQuad(quadVertices, vec2(0.62f, 0.55f), vec2(0.25f, 0.25f * window.AspectRatio()));

	VertexBuffer quadVb;
	IndexBuffer quadIb;
	if (!quadVb.Create(&device, quadVertices, sizeof(VertexPTC), 4) ||
		!quadIb.Create(&device, quadIndices, 6))
		{
		printf("미니맵 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 셰이더 2종 + 상수 버퍼
	UINT cubeLayoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pCubeLayout = VertexPC::LayoutDescs(&cubeLayoutCount);
	UINT quadLayoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pQuadLayout = VertexPTC::LayoutDescs(&quadLayoutCount);

	Shader cubeShader;
	Shader quadShader;
	ConstantBuffer<CbTransform> cbTransform;
	if (!cubeShader.CompileFromString(&device, ColorTransformShaderSource(), pCubeLayout, cubeLayoutCount) ||
		!quadShader.CompileFromString(&device, TextureShaderSource(), pQuadLayout, quadLayoutCount) ||
		!cbTransform.Create(&device))
		{
		printf("셰이더/상수 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 6. 카메라 2개: 장면은 경사 시점, 미니맵은 바로 위에서 내려다본다.
	const mat4 sceneView = mat4::LookAtLH(vec3(0.0f, 1.5f, -4.0f), vec3::Zero(), vec3::Up());
	const mat4 sceneProj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);

	// 바로 위에서 볼 때는 Up 벡터로 +Y를 쓸 수 없으므로 +Z를 사용한다.
	const mat4 topView = mat4::LookAtLH(vec3(0.0f, 5.0f, 0.0f), vec3::Zero(), vec3::Forward());
	const mat4 topProj = mat4::PerspectiveFovLH(jc_math_pi_div4, 1.0f, 0.1f, 100.0f);	// 렌더 타깃은 정사각형(256x256)

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

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		const mat4 world = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.3f);

		// ---- 패스 1: 렌더 타깃에 미니맵 시점으로 그리기 ----
		device.BeginFrame(color(0.08f, 0.08f, 0.12f, 1.0f));	// 백버퍼도 미리 클리어

		device.SetRenderTarget(&miniMapTarget);					// 이제 그리기는 텍스처로!
		miniMapTarget.Clear(&device, color(0.1f, 0.25f, 0.15f, 1.0f));	// 초록 배경 = 미니맵 티가 나게

		CbTransform cb;
		cb.wvp_ = world * topView * topProj;
		cbTransform.UpdateAndBind(&device, cb, 0);

		cubeVb.Bind(&device);
		cubeIb.Bind(&device);
		cubeShader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.Context()->DrawIndexed(36, 0, 0);

		// ---- 패스 2: 백버퍼에 장면 그리기 ----
		device.SetRenderTarget(nullptr);						// 다시 화면으로 복귀!

		cb.wvp_ = world * sceneView * sceneProj;
		cbTransform.UpdateAndBind(&device, cb, 0);

		cubeVb.Bind(&device);
		cubeIb.Bind(&device);
		cubeShader.Bind(&device);
		device.Context()->DrawIndexed(36, 0, 0);

		// ---- 패스 3: 렌더 타깃 결과를 미니맵 사각형에 출력 ----
		device.SetSampler(GraphicDevice::SamplerFilter::fmLinear, GraphicDevice::SamplerAddress::amClamp, 0);
		miniMapTarget.BindAsTexture(&device, 0);				// 그려진 결과를 텍스처로 장착

		quadVb.Bind(&device);
		quadIb.Bind(&device);
		quadShader.Bind(&device);
		device.Context()->DrawIndexed(6, 0, 0);

		device.EndFrame(true);
	}

	// 8. 정리
	miniMapTarget.Destroy();
	device.Finalize();
	window.Destroy();
}
