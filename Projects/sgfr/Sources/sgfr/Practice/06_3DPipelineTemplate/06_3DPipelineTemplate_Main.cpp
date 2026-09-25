/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 06. 3D 렌더링 파이프라인 템플릿 (Practice) - 기반 코드
 */

#include "Core.h"
#include "jc/Primitives/StringConvert.h"
#include "sgf/Graphics/ResourceMgr.h"
#include "sgfr/Practice/06_3DPipelineTemplate/06_3DPipelineTemplate_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
struct CbTransform
{
	mat4 worldViewProj_;
};

const char* GetShaderSource()
{
	return R"(
cbuffer CbTransform : register(b0)
{
	row_major float4x4 gWorldViewProj;
};

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
	output.position = mul(float4(_input.position, 1.0f), gWorldViewProj);
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

void Practice_3DPipelineTemplate_Main()
{
	jc::Console::WriteLine(_T("[Practice 06] 3D 렌더링 파이프라인 템플릿 - ESC 종료"));

	Window window;
	if (!window.Create(_T("Practice 06. 3D 렌더링 파이프라인 템플릿 (ESC 종료)"), 800, 600))
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
		{ vec3(-0.5f, +0.5f, -0.5f), color(0xFF, 0x00, 0x00, 0xFF) },
		{ vec3(+0.5f, +0.5f, -0.5f), color(0x00, 0xFF, 0x00, 0xFF) },
		{ vec3(-0.5f, -0.5f, -0.5f), color(0x00, 0x00, 0xFF, 0xFF) },
		{ vec3(+0.5f, -0.5f, -0.5f), color(0xFF, 0xFF, 0x00, 0xFF) },
		{ vec3(-0.5f, +0.5f, +0.5f), color(0xFF, 0x00, 0xFF, 0xFF) },
		{ vec3(+0.5f, +0.5f, +0.5f), color(0x00, 0xFF, 0xFF, 0xFF) },
		{ vec3(-0.5f, -0.5f, +0.5f), color(0xFF, 0xFF, 0xFF, 0xFF) },
		{ vec3(+0.5f, -0.5f, +0.5f), color(0x4D, 0x4D, 0x4D, 0xFF) },
	};
	const _u32 indices[] = {
		0, 1, 2, 2, 1, 3,
		5, 4, 7, 7, 4, 6,
		4, 5, 0, 0, 5, 1,
		2, 3, 6, 6, 3, 7,
		4, 0, 6, 6, 0, 2,
		1, 5, 3, 3, 5, 7,
	};

	VertexBuffer vb;
	IndexBuffer ib;
	if (!vb.Create(gd, vertices, 8, VertexPC::Decl()) ||
		!ib.Create(gd, indices, 36))
	{
		jc::Console::WriteLine(_T("버퍼 생성 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	const _u64 hVs = gc.CreateVertexShader(jc::StringConvert::FromUtf8(GetShaderSource()));
	const _u64 hPs = gc.CreatePixelShader(jc::StringConvert::FromUtf8(GetShaderSource()));
	ConstantBuffer<CbTransform> cbTransform;
	if (hVs == INVALID_RESOURCE_KEY || hPs == INVALID_RESOURCE_KEY ||
		!cbTransform.Create(gd))
	{
		jc::Console::WriteLine(_T("셰이더/상수 버퍼 생성 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	const mat4 view = mat4::LookAtLH(vec3(0.0f, 1.2f, -2.5f), vec3::Zero(), vec3::Up());
	const mat4 proj = mat4::PerspectiveFovLH(jc_math_deg2rad(60.0f), window.AspectRatio(), 0.1f, 100.0f);

	FrameTimer timer;
	timer.Reset();
	_f32 elapsed = 0.0f;

	gc.SetDepth(DepthMode::dmReadWrite);

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
		{
			break;
		}

		input.NextFrame();

		timer.Tick();
		elapsed += timer.DeltaTime();

		gd.BeginFrame(color(0x0D, 0x0D, 0x1A, 0xFF));

		const mat4 world = mat4::RotationY(elapsed * 0.8f) * mat4::RotationX(elapsed * 0.5f);

		CbTransform cb = {};
		cb.worldViewProj_ = world * view * proj;
		cbTransform.UpdateAndBind(gc, cb, 0);

		vb.Bind(gc);
		ib.Bind(gc);
		gc.SetVertexShader(hVs);
		gc.SetPixelShader(hPs);
		gc.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		gc.DrawIndexed(36, 0, 0);

		gd.Present(true);
	}

	g_cResourceMgr.Finalize();
	gd.Finalize();
	window.Destroy();
}
