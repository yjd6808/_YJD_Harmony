/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 07. 2D 렌더링 파이프라인 연습 (Practice) - 연습용 진입점
 */

#include "Core.h"
#include "jc/Primitives/StringConvert.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Practice/07_2DPipelinePractice/07_2DPipelinePractice_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	const char* GetShaderSource()
	{

		return R"(
		struct VSInput
		{
			float3 position : POSITION;
			float4 color    : COLOR0;
		};

		struct VSOutput
		{
			float4 position : SV_POSITION;
			float4 color    : COLOR0;
		};

		VSOutput VSMain(VSInput _input)
		{
			VSOutput output;
			output.position = float4(_input.position, 1.0f);
			output.color = _input.color;
			return output;
		}

		float4 PSMain(VSOutput _input) : SV_TARGET
		{
			return _input.color;
		}

		)";
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void Practice_2DPipelinePractice_Main()
{
	jc::Console::WriteLine(_T("[Practice 07] 2D 렌더링 파이프라인 연습 - ESC 종료"));

	Window window;
	if (!window.Create(_T("Practice 07. 2D 렌더링 파이프라인 연습 (ESC 종료)"), 800, 600))
	{
		jc::Console::WriteLine(_T("윈도우 생성 실패!"));
		return;
	}

	InputManager input;
	window.ConnectInput(&input);

	GraphicDevice gd;
	if (!gd.Initialize())
	{
		jc::Console::WriteLine(_T("그래픽 디바이스 초기화 실패!"));
		window.Destroy();
		return;
	}
	if (!g_cResourceMgr.Initialize(&gd))
	{
		jc::Console::WriteLine(_T("리소스 매니저 초기화 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	if (!gd.CreateSwapChain(window.Handle(), window.Width(), window.Height(), PixelFormat::pfRgba8))
	{
		jc::Console::WriteLine(_T("스왑체인 생성 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}
	GraphicContext& gc = gd.Context();

	const VertexPC vertices[] = {
		{ vec3(-1.f, +1.f), color(0xFF, 0x00, 0x00, 0xFF) },
		{ vec3(+1.f, -1.f), color(0xFF, 0x00, 0x00, 0xFF) },
		{ vec3(-1.f, 0.f), color(0xFF, 0x00, 0x00, 0xFF) },
	};
	const _u32 indices[] = { 0, 1, 2 };

	const _u64 hVs = gc.CreateVertexShader(jc::StringConvert::FromUtf8(GetShaderSource()));
	const _u64 hPs = gc.CreatePixelShader(jc::StringConvert::FromUtf8(GetShaderSource()));
	const _u64 hVb = gd.CreateVertexBuffer<VertexPC>(vertices, 3);
	const _u64 hIb = gd.CreateIndexBuffer(indices, 3);
	if (hVs == INVALID_RESOURCE_KEY || hPs == INVALID_RESOURCE_KEY || hVb == INVALID_RESOURCE_KEY || hIb == INVALID_RESOURCE_KEY)
	{
		jc::Console::WriteLine(_T("리소스 생성 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	vec3 pos = { 0.f, 1.f, 3.f };
	mat4 translation = mat4::Translation(pos);
	pos.Mul(translation, 1.f);

	// view matrix - 카메라
	mat4 view = mat4::Identity();

	// projection matrix - 직교 투영
	mat4 projection = mat4::Identity();
	
	// world matrix - 모델 변환
	mat4 world = mat4::Identity();

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		gd.BeginFrame(color::CORNFLOWER_BLUE);

		gc.SetVertexShader(hVs);
		gc.SetPixelShader(hPs);
		gc.SetVertexBuffer(hVb);
		gc.SetIndexBuffer(hIb);
		gc.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		gc.SetRasterizer(CullMode::cmNone, FillMode::fmSolid, FrontFace::ffClockwise);
		gc.SetDepth(DepthMode::dmDisabled);
		gc.SetBlend(BlendMode::bmNone);
		gc.DrawIndexed(3, 0, 0);

		gd.Present(true);
		input.NextFrame();
	}

	g_cResourceMgr.Finalize();
	gd.Finalize();
	window.Destroy();
}
