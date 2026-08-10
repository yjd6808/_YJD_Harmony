/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 2:00:00 PM
 * =====================
 * 19. 래스터라이저 스테이트 (Rasterizer State)
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. 래스터라이저 단계: 삼각형 -> 화면 픽셀 변환의 옵션 조절
 *  2. 와이어프레임: 메쉬 구조를 선으로 보며 디버깅하는 법
 *  3. 컬링(Culling): 안 보이는 면을 버려 그리기 비용을 절반으로 줄이는 원리
 *  4. 정점 감기 순서(시계/반시계)가 앞면/뒷면을 결정한다는 사실
 *
 * [Before/After 비교 뷰]
 *  같은 큐브가 나란히 두 개 그려진다. (같은 회전, 같은 지오메트리)
 *  - 왼쪽  (Before): 기본 상태 고정 = Solid + Back 컬링
 *  - 오른쪽(After) : 현재 선택한 와이어프레임/컬링 적용
 *  래스터라이저 스테이트도 드로우 단위 상태이므로 드로우 사이에 바꿔 끼워 비교한다.
 *
 * [관찰 포인트]
 *  - Front 컬링으로 바꾸면 오른쪽 큐브만 "속"이 보인다 (뒷면만 남으므로!)
 *  - 와이어프레임 + None 컬링이면 모든 삼각형 모서리가 다 보인다
 *  - 왼쪽 큐브는 무슨 키를 눌러도 그대로다 (기준이니까!)
 *
 * [조작법]
 *  - W: 와이어프레임 토글 (오른쪽에만 적용)
 *  - 1: 컬링 없음 / 2: 뒷면 컬링(기본) / 3: 앞면 컬링 (오른쪽에만 적용)
 *  - ESC: 종료
 */

#include "Core.h"
#include "sgfr/19_RasterizerState/19_RasterizerState_Main.h"
#include "sgfr/19_RasterizerState/19_RasterizerState_Function.h"
#include "sgfr/Common/TutorialCommon.h"	// 셰이더/큐브 공용 사용

using namespace sgf;
using namespace jc;

namespace
{
	// HLSL의 CbTransform(b0)과 메모리 배치가 일치해야 한다. (64바이트 = 16의 배수)
	struct CbTransform
	{
		mat4 wvp_;	// 월드 x 뷰 x 투영
	};

	// 창 제목에 표시할 컬링 이름표 (CullMode enum 순서와 일치)
	const wchar_t* s_szCullNames[] = { L"None(양면)", L"Back(뒷면 제거)", L"Front(앞면 제거)" };
}

// 래스터라이저 스테이트 튜토리얼을 실행한다. (좌: 기본 상태 / 우: 선택 상태 Before/After 비교)
void RasterizerState_Main()
{
	PrintRasterizerExplanation();

	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create(L"19. 래스터라이저 스테이트 (W 와이어, 1/2/3 컬링, ESC 종료)", 800, 600))
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

	// 2. 면마다 색이 다른 큐브 지오메트리 (하나를 두 번 그려 재활용한다)
	VertexPC vertices[8];
	_u32 indices[36];
	FillColorCube(vertices, indices);

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

	// 3. 셰이더 + 상수 버퍼
	UINT layoutCount = 0;
	const D3D11_INPUT_ELEMENT_DESC* pLayoutDescs = VertexPC::LayoutDescs(&layoutCount);

	Shader shader;
	ConstantBuffer<CbTransform> cbTransform;
	if (!shader.CompileFromString(&device, ColorTransformShaderSource(), pLayoutDescs, layoutCount) ||
		!cbTransform.Create(&device))
		{
		printf("셰이더/상수 버퍼 생성 실패!\n");
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 카메라: 큐브 두 개가 모두 보이도록 약간 뒤로 물러서 내려다본다.
	const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.5f, -4.5f), vec3::Zero(), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_pi_div4, window.AspectRatio(), 0.1f, 100.0f);

	// 5. 현재 래스터라이저 상태 (키 입력으로 바꾼다. 오른쪽 After 큐브에만 적용된다)
	bool bWireframe = false;
	GraphicDevice::CullMode cullMode = GraphicDevice::CullMode::cmBack;

	auto UpdateTitle = [&]()
	{
		wchar_t szTitle[256];
		swprintf_s(szTitle, L"19. 래스터라이저 - 왼쪽: Solid+Back(기준) | 오른쪽: %s / %s (W, 1/2/3, ESC)",
			bWireframe ? L"Wireframe(선)" : L"Solid(면)",
			s_szCullNames[static_cast<int>(cullMode)]);
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
		if (input.IsKeyPressed('W')) { bWireframe = !bWireframe;                       bChanged = true; }
		if (input.IsKeyPressed('1')) { cullMode = GraphicDevice::CullMode::cmNone;      bChanged = true; }
		if (input.IsKeyPressed('2')) { cullMode = GraphicDevice::CullMode::cmBack;      bChanged = true; }
		if (input.IsKeyPressed('3')) { cullMode = GraphicDevice::CullMode::cmFront;     bChanged = true; }
		if (bChanged)
		{
			UpdateTitle();
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		device.BeginFrame(color(0.08f, 0.08f, 0.12f, 1.0f));

		// 두 큐브의 공통 회전: 모든 면이 골고루 보이도록 두 축 회전
		const mat4 rotation = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.4f);

		vb.Bind(&device);
		ib.Bind(&device);
		shader.Bind(&device);
		device.Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// 같은 큐브를 지정한 x 위치/래스터라이저 상태로 그리는 보조 람다
		auto DrawCube = [&](_f32 _offsetX, bool _bWire, GraphicDevice::CullMode _cull)
		{
			device.SetWireframe(_bWire);
			device.SetCullMode(_cull);

			CbTransform cb;
			cb.wvp_ = rotation * mat4::Translation(_offsetX, 0.0f, 0.0f) * view * proj;	// 회전 후 이동
			cbTransform.UpdateAndBind(&device, cb, 0);
			device.Context()->DrawIndexed(36, 0, 0);
		};

		// [Before] 왼쪽 큐브: 기본 상태 고정 (Solid + Back 컬링)
		DrawCube(-1.1f, false, GraphicDevice::CullMode::cmBack);

		// [After] 오른쪽 큐브: 현재 선택한 상태 적용
		DrawCube(+1.1f, bWireframe, cullMode);

		device.EndFrame(true);
	}

	// 7. 정리: 다음 튜토리얼을 위해 기본 상태로 되돌리고 종료
	device.SetWireframe(false);
	device.SetCullMode(GraphicDevice::CullMode::cmBack);
	device.Finalize();
	window.Destroy();
}
