/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 09. 파이프라인 상태 객체 (Pipeline State Objects)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. 래스터라이저/블렌드/깊이/샘플러 = 파이프라인의 4대 상태 객체
 * 2. 각 상태를 독립 객체로 만들어 들고 다닐 수 있다 (수도 코드 §7 대응)
 * 3. 상태 객체는 설정 조합마다 자기만의 D3D 상태 객체를 만들어 갖는다 (독립 '값')
 *
 * [Before/After 비교]
 * - Before: device.Context().SetRasterizer(CullMode::cmBack, FillMode::fmWireframe) 같은 개별 스위치 함수 호출
 * - After: RasterizerState 객체를 만들어 context.SetRasterizerState(&rs)로 교체
 * 상태 조합이 '값'이 되므로 Material(20번)에 통째로 담을 수 있다!
 *
 * [조작법]
 * - 1/2: 채우기 (솔리드+뒷면컬링 / 와이어프레임+컬링없음)
 * - 3/4: 블렌드 (불투명 / 알파 반투명)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/09_PipelineStateObjects/09_PipelineStateObjects_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	// NDC 좌표를 그대로 쓰는 최소 셰이더 (정점 색의 알파까지 그대로 출력)
	const char* PASSTHROUGH_SHADER_SOURCE = R"(
struct VsIn
{
	float3 position : POSITION;
	float2 uv       : TEXCOORD0;
	float4 color    : COLOR0;
};

struct VsOut
{
	float4 position : SV_Position;
	float4 color    : COLOR0;
};

VsOut VSMain(VsIn _in)
{
	VsOut result;
	result.position = float4(_in.position, 1.0f);
	result.color = _in.color;
	return result;
}

float4 PSMain(VsOut _in) : SV_Target
{
	return _in.color;
}
)";

	//////////////////////////////////////////////////////////////////////////////////////////
	// 사각형 하나를 메시로 만든다. (중심 _center, 반 변 _halfSize, 색 _color)
	bool BuildQuadMesh(sgf::GraphicDevice* _pDevice, sgf::Mesh* _pOutMesh,
		const vec2& _center,
		_f32 _halfSize,
		const color& _color)
	{
		const sgf::VertexPTC vertices[4] =
		{
			{ vec3(_center.x - _halfSize, _center.y + _halfSize, 0.0f), vec2(0.0f, 0.0f), _color },
			{ vec3(_center.x + _halfSize, _center.y + _halfSize, 0.0f), vec2(1.0f, 0.0f), _color },
			{ vec3(_center.x + _halfSize, _center.y - _halfSize, 0.0f), vec2(1.0f, 1.0f), _color },
			{ vec3(_center.x - _halfSize, _center.y - _halfSize, 0.0f), vec2(0.0f, 1.0f), _color },
		};
		const _u32 indices[6] = { 0, 1, 2, 0, 2, 3 };	// 시계방향 삼각형 2개

		return _pOutMesh->Initialize(_pDevice, vertices, 4, VertexPTC::Decl(), indices, 6);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// 파이프라인 상태 객체 튜토리얼을 실행한다. (상태 객체를 손으로 교체해보기)
void PipelineStateObjects_Main()
{
	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("09. 파이프라인 상태 객체 (1/2 채우기, 3/4 블렌드, ESC 종료)", 800, 600))
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

	GraphicContext& context = device.Context();

	// 2. 셰이더 준비
	VertexShader vs;
	PixelShader ps;
	if (!vs.InitializeFromSource(&device, PASSTHROUGH_SHADER_SOURCE) ||
		!ps.InitializeFromSource(&device, PASSTHROUGH_SHADER_SOURCE))
	{
		jc::Console::WriteLine("셰이더 컴파일 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 3. 메시 2개: 바닥 사각형(불투명) + 위에 겹치는 반투명 사각형
	Mesh backQuad;
	Mesh frontQuad;
	if (!BuildQuadMesh(&device, &backQuad, vec2(-0.15f, 0.0f), 0.5f, color(0x33, 0x99, 0xFF, 0xFF)) ||
		!BuildQuadMesh(&device, &frontQuad, vec2(0.15f, 0.0f), 0.5f, color(0xFF, 0x66, 0x33, 0x80)))
	{
		jc::Console::WriteLine("메시 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	// 4. 상태 객체 준비: 같은 종류라도 설정 조합마다 따로 만들어 둔다.
	// (수도 코드 §7의 rasterizerState.init / blendState.init에 해당)
	// 각 객체는 자기만의 D3D 상태 객체를 소유 생성한다. (중복 제거 없음 — 설정 조합이 곧 독립된 값)
	RasterizerState rsSolid;
	RasterizerState rsWireframe;
	BlendState blendOpaque;
	BlendState blendAlpha;
	DepthStencilState depthDisabled;
	if (!rsSolid.Initialize(&device, CullMode::cmBack, FillMode::fmSolid) ||
		!rsWireframe.Initialize(&device, CullMode::cmNone, FillMode::fmWireframe) ||
		!blendOpaque.Initialize(&device, BlendMode::bmNone) ||
		!blendAlpha.Initialize(&device, BlendMode::bmAlpha) ||
		!depthDisabled.Initialize(&device, DepthMode::dmDisabled))
	{
		jc::Console::WriteLine("상태 객체 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("1/2로 채우기, 3/4로 블렌드를 바꿔보세요!");

	// 5. 렌더 루프
	RasterizerState* pCurrentRs = &rsSolid;
	BlendState* pCurrentBlend = &blendAlpha;

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		if (input.IsKeyPressed('1')) { pCurrentRs = &rsSolid; }
		if (input.IsKeyPressed('2')) { pCurrentRs = &rsWireframe; }
		if (input.IsKeyPressed('3')) { pCurrentBlend = &blendOpaque; }
		if (input.IsKeyPressed('4')) { pCurrentBlend = &blendAlpha; }
		input.NextFrame();

		device.BeginFrame(color(0x0F, 0x0F, 0x1A, 0xFF));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		// 상태 객체 교체: 포인터 하나 바꾸는 것이 전부다!
		context.SetVertexShader(&vs);
		context.SetPixelShader(&ps);
		context.SetRasterizerState(pCurrentRs);
		context.SetDepthStencilState(&depthDisabled);

		// 바닥 사각형은 항상 불투명으로
		context.SetBlendState(&blendOpaque);
		backQuad.Bind(context);
		backQuad.Draw(context);

		// 앞 사각형은 선택한 블렌드로 (알파 0.5 색이라 차이가 바로 보인다)
		context.SetBlendState(pCurrentBlend);
		frontQuad.Bind(context);
		frontQuad.Draw(context);

		device.Present(true);
	}

	// 6. 정리: 상태 객체도 Finalize()를 제공하지만 스택 소멸 시 자동 정리되므로 여기서는 직접 호출하지 않는다.
	frontQuad.Finalize();
	backQuad.Finalize();
	ps.Finalize();
	vs.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
