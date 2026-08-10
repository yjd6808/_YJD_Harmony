/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 10:30:00 AM
 * =====================
 * 13. 3D 큐브 (Cube 3D) - 깊이 버퍼와 원근 투영
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 드디어 3D! 정점 8개 + 인덱스 36개로 정육면체 만들기
 *  2. 원근 투영 행렬 (PerspectiveFovLH): 멀수록 작게
 *  3. 뷰 행렬 (LookAtLH): 카메라 위치에서 목표를 바라보는 행렬
 *  4. 깊이 버퍼: 앞뒤 관계를 자동으로 정리해주는 장치
 *
 * [좀 더 깊이: 왜 인덱스가 36개인가?]
 *  정육면체 = 6면, 한 면 = 삼각형 2개 = 인덱스 6개. 6 x 6 = 36.
 *  꼭짓점은 8개뿐이므로 인덱스 버퍼의 재활용 효과가 크다! (07번 복습)
 *
 * [조작법]
 *  - 무지개색 큐브가 빙글뺙글 돈다. 스페이스바: 깊이 테스트 켜기/끄기 토글.
 *  - 깊이 테스트를 끄면 뒷면이 앞면을 덮는 버그 장면을 직접 볼 수 있다!
 *  - ESC로 종료.
 */

#include "Core.h"
#include "sgfr/Tutorial/13_Cube3D/13_Cube3D_Main.h"
#include "sgfr/Tutorial/13_Cube3D/13_Cube3D_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 셰이더 CbTransform과 메모리 배치 일치 (64바이트)
	struct CbTransform
	{
		mat4 worldViewProj_;	// 월드 x 뷰 x 투영 결합 행렬
	};
}

// 3D 큐브 튜토리얼을 실행한다. (깊이 버퍼와 원근 투영)
void Cube3D_Main()
{
	PrintDepthAndPerspectiveExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"13. 3D 큐브 - 스페이스바로 깊이 테스트 토글 (ESC로 종료)", 800, 600))
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

	// 2. 정육면체 정점 8개 (각 꼭짓점마다 다른 색 -> 면 색이 그라데이션으로 보간된다)
	//
	//        4 -------- 5
	//       /|         /|
	//      0 -------- 1 |          (z가 작은 쪽이 앞면: 0,1,2,3)
	//      | 6 -------|-7
	//      |/         |/
	//      2 -------- 3
	const VertexPC vertices[] = {
		{ vec3(-0.5f, +0.5f, -0.5f), color(1.0f, 0.0f, 0.0f, 1.0f) },	// 0: 앞-왼위
		{ vec3(+0.5f, +0.5f, -0.5f), color(0.0f, 1.0f, 0.0f, 1.0f) },	// 1: 앞-오른위
		{ vec3(-0.5f, -0.5f, -0.5f), color(0.0f, 0.0f, 1.0f, 1.0f) },	// 2: 앞-왼아래
		{ vec3(+0.5f, -0.5f, -0.5f), color(1.0f, 1.0f, 0.0f, 1.0f) },	// 3: 앞-오른아래
		{ vec3(-0.5f, +0.5f, +0.5f), color(1.0f, 0.0f, 1.0f, 1.0f) },	// 4: 뒤-왼위
		{ vec3(+0.5f, +0.5f, +0.5f), color(0.0f, 1.0f, 1.0f, 1.0f) },	// 5: 뒤-오른위
		{ vec3(-0.5f, -0.5f, +0.5f), color(1.0f, 1.0f, 1.0f, 1.0f) },	// 6: 뒤-왼아래
		{ vec3(+0.5f, -0.5f, +0.5f), color(0.3f, 0.3f, 0.3f, 1.0f) },	// 7: 뒤-오른아래
	};

	// 3. 인덱스 36개 (6면 x 삼각형 2개 x 3)
	//    바깥에서 볼 때 시계 방향이 되도록 순서를 맞춰야 한다. (백페이스 컬링)
	const _u32 indices[] = {
		0, 1, 2,  2, 1, 3,	// 앞면   (z-)
		5, 4, 7,  7, 4, 6,	// 뒷면   (z+)
		4, 5, 0,  0, 5, 1,	// 윗면   (y+)
		2, 3, 6,  6, 3, 7,	// 아랫면 (y-)
		4, 0, 6,  6, 0, 2,	// 왼면   (x-)
		1, 5, 3,  3, 5, 7,	// 오른면 (x+)
	};

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(&device, vertices, sizeof(VertexPC), 8) ||
		!ib.Create(&device, indices, 36))
		{
		printf("버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 셰이더 + 상수 버퍼
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	ConstantBuffer<CbTransform> cbTransform;
	if (!shader.CompileFromString(&device, CubeShaderSource(), pLayoutDescs, layoutCount) ||
		!cbTransform.Create(&device))
		{
		printf("셰이더/상수 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 5. 뷰/투영 행렬 준비
	//    LookAtLH(카메라 위치, 바라보는 지점, 위 방향)
	//    카메라를 뒤쪽(z = -2.5)에 두고 원점을 바라본다. (LH: +z가 화면 안쪽)
	const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.2f, -2.5f), vec3::Zero(), vec3::Up());

	//    PerspectiveFovLH(수직 시야각, 화면비율, 근평면, 원평면)
	//    시야각 60도: 사람 눈과 비슷한 자연스러운 화각
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;
	bool bDepthTest = true;

	printf("스페이스바로 깊이 테스트를 켜고 꺼보세요. 꺼지면 뒷면이 앞을 덮는 오류가 보입니다!\n");

	// 6. 렌더 루프
	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		// 스페이스바: 깊이 테스트 토글 (문제 상황을 직접 눈으로 확인하는 실험!)
		if (input.IsKeyPressed(VK_SPACE))
		{
			bDepthTest = !bDepthTest;
			device.SetDepthTest(bDepthTest);
			printf("깊이 테스트: %s\n", bDepthTest ? "ON (정상)" : "OFF (뒷면이 덮일 수 있음)");
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0.05f, 0.05f, 0.1f, 1.0f));

		// 큐브를 두 축으로 천천히 회전 (회전 행렬 두 개를 결합)
		const mat4 world = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.5f);

		CbTransform cb = {};
		cb.worldViewProj_ = world * view * proj;	// 월드 -> 뷰 -> 투영
		cbTransform.UpdateAndBind(&device, cb, 0);

		vb.Bind(&device);
		ib.Bind(&device);
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device.Context()->DrawIndexed(36, 0, 0);

		device.EndFrame(true);
	}

	// 7. 정리
	device.Finalize();
	window.Destroy();
}
