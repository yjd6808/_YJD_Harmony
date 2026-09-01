/*
 * 작성자: 윤정도
 * 생성일: 8/9/2026 6:30:00 PM
 * =====================
 * 08. 메시와 토폴로지 (Mesh & Topology)
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. Mesh: 정점버퍼 + (인덱스버퍼) + 입력레이아웃 + 토폴로지를 한 덩어리로 묶는 단위
 * 2. 토폴로지: 같은 정점 6개도 '해석 방법'에 따라 삼각형/선/점으로 달라진다
 * 3. Mesh::Bind 한 번이면 IA 단계 세팅이 끝난다 (핵심 편의)
 *
 * [Before/After 비교]
 * - Before: VertexBuffer/InputLayout/토폴로지를 튜토리얼마다 따로따로 바인딩
 * - After: Mesh 하나로 묶어 mesh.Bind(context) 한 줄이면 끝
 *
 * [조작법]
 * - 1: 삼각형 리스트 (6정점 = 삼각형 2개)
 * - 2: 선 스트립 (6정점 = 이어진 선 5개)
 * - 3: 점 리스트 (6정점 = 점 6개)
 * - ESC: 종료
 */

#include "Core.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Tutorial/08_MeshAndTopology/08_MeshAndTopology_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	// NDC 좌표를 그대로 쓰는 최소 셰이더 (행렬 없음, 정점 색 그대로)
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

	// 토폴로지별 창 제목 이름표
	const char* s_szTopologyNames[] = { "삼각형 리스트", "선 스트립", "점 리스트" };
}

// 메시와 토폴로지 튜토리얼을 실행한다. (같은 정점 6개를 세 가지 방법으로 해석)
void MeshAndTopology_Main()
{
	// 1. 윈도우 + 디바이스 준비
	Window window;
	if (!window.Create("08. 메시와 토폴로지 (1/2/3 전환, ESC 종료)", 800, 600))
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

	// 2. 셰이더 준비 (VertexShader/PixelShader가 별도 클래스다)
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

	// 3. 정점 6개 준비: 육각형 꾸미의 무지개 정점 (NDC 좌표, 인덱스 없음)
	VertexPTC vertices[6];
	for (_s32 i = 0; i < 6; ++i)
	{
		const _f32 angle = jc_math_pi * 2.0f * (_f32)i / 6.0f + jc_math_pi * 0.5f;
		vertices[i].position_ = vec3(cosf(angle) * 0.7f, sinf(angle) * 0.7f, 0.0f);
		vertices[i].uv_ = vec2(0.0f, 0.0f);
		vertices[i].color_ = color::FromFloat(
			0.5f + 0.5f * cosf(angle),
			0.5f + 0.5f * sinf(angle),
			1.0f - 0.5f * cosf(angle),
			1.0f);
	}

	// 4. 메시 생성: 정점버퍼 + 레이아웃 + 토폴로지를 한 덩어리로

	Mesh mesh;
	if (!mesh.Initialize(&device, vertices, 6, VertexPTC::Decl()))
	{
		jc::Console::WriteLine("메시 생성 실패!");
	g_cResourceMgr.Finalize();
		device.Finalize();
		window.Destroy();
		return;
	}

	jc::Console::WriteLine("같은 정점 6개를 1/2/3 키로 다르게 해석해보세요!");

	// 5. 렌더 루프
	PrimitiveTopology topology = PrimitiveTopology::ptTriangleList;
	_s32 topologyIndex = 0;

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}
		if (input.IsKeyPressed('1')) { topology = PrimitiveTopology::ptTriangleList; topologyIndex = 0; }
		if (input.IsKeyPressed('2')) { topology = PrimitiveTopology::ptLineStrip;    topologyIndex = 1; }
		if (input.IsKeyPressed('3')) { topology = PrimitiveTopology::ptPointList;    topologyIndex = 2; }
		input.NextFrame();

		window.SetTitle(s_szTopologyNames[topologyIndex]);

		device.BeginFrame(color(0x0F, 0x0F, 0x1A, 0xFF));
		context.InvalidateCache();	// BeginFrame이 원시 상태를 건드렸으므로 캐시를 비운다

		// 셰이더는 이 튜토리얼이 직접 바인딩 (20번부터는 Material이 대신해준다)
		context.SetVertexShader(&vs);
		context.SetPixelShader(&ps);

		// 메시 바인딩 한 줄 + 토폴로지만 덮어쓰기
		// (토폴로지는 메시의 '기본값'일 뿐, IA 상태라 언제든 바꿀 수 있다는 것이 핵심!)
		mesh.Bind(context);
		context.SetPrimitiveTopology(topology);
		mesh.Draw(context);

		device.Present(true);
	}

	// 6. 정리: 생성의 역순
	mesh.Finalize();
	ps.Finalize();
	vs.Finalize();
	g_cResourceMgr.Finalize();
	device.Finalize();
	window.Destroy();
}
