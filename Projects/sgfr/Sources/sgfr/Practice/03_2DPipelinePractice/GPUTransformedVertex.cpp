/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 03. 2D 렌더링 파이프라인 연습 (Practice) - GPU 정점 굽기
 */

#include "Core.h"
#include "jc/Primitives/StringConvert.h"
#include "sgf/Graphics/ResourceMgr.h"

using namespace sgf;
using namespace jc;

namespace
{
	
	////////////////////////////////////////////////////////////////////////////////////////
	// : GPU 계산으로 정점을 업데이트함
	////////////////////////////////////////////////////////////////////////////////////////
	const char* GetShaderSource_Transformed()
	{
		return R"(
		cbuffer cb1 : register(b0)
		{
			row_major float4x4 matVp_;		// view * projection 행렬
		};

		cbuffer cb2 : register(b1)
		{
			row_major float4x4 matWorld_;	// 오브젝트 월드 행렬
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
			output.position = mul(float4(_input.position, 1.0f), mul(matWorld_, matVp_));
			output.color = _input.color;
			return output;
		}

		float4 PSMain(VSOutput _input) : SV_TARGET
		{
			return _input.color;
		}

		)";
	}

	struct s2DModel
	{
		vec2 localPos_;
		vec2 localScale_ = { 1.f, 1.f };
		vec2 localDirection_ = { 0.f, 0.f };

		bool transformDirty_;

		void InvalidateTransform() { transformDirty_ = true; }
		void UpdateTransform()
		{
			matWorld_ = mat4::Scale(localScale_)
				* mat4::RotationZ(atan2f(localDirection_.y, localDirection_.x))
				* mat4::Translation(localPos_);
		}
		mat4 matWorld_;

		Vector<_u32> indices_;
		Vector<VertexPC> verticesGPU_;
	};

	struct s2DCamera
	{
		vec2 localPos_;
		_f32 localRotation_ = 0.f;

		void InvalidateView() { matViewDirty_ = true; }
		void UpdateView()
		{
			matView_ = mat4::Translation(-localPos_) * mat4::RotationZ(-localRotation_);
			matVp_ = matView_ * matProjection_;
		}

		bool matViewDirty_;
		mat4 matView_;
		mat4 matProjection_;
		mat4 matVp_;
	};
}

////////////////////////////////////////////////////////////////////////////////////////
void GPUTransformedVertex_Main()
{
	jc::Console::WriteLine(_T("[Practice 03] 2D 렌더링 파이프라인 연습 - ESC 종료"));

	Window window;
	if (!window.Create(_T("Practice 03. 2D 렌더링 파이프라인 연습 (ESC 종료)"), 800, 600))
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

	const _u64 hVs = gc.CreateVertexShader(jc::StringConvert::FromUtf8(GetShaderSource_Transformed()));
	const _u64 hPs = gc.CreatePixelShader(jc::StringConvert::FromUtf8(GetShaderSource_Transformed()));
	if (hVs == INVALID_RESOURCE_KEY || hPs == INVALID_RESOURCE_KEY)
	{
		jc::Console::WriteLine(_T("리소스 생성 실패!"));
		g_cResourceMgr.Finalize();
		gd.Finalize();
		window.Destroy();
		return;
	}

	s2DModel model;
	model.indices_ = { 0, 1, 2 };
	model.verticesGPU_ =
	{
		{ { 0.f, 0.f }, color(0xffff0000) },
		{ { 1.f, 1.f }, color(0xffff0000) },
		{ { 0.f, 1.f }, color(0xffff0000) }
	};

	model.localPos_ = { 200.f, 300.f };
	model.localScale_ = { 100.f, 100.f };
	model.transformDirty_ = true;

	s2DCamera camera;
	camera.localPos_ = { 0.0f, 0.f };
	camera.localRotation_ = 0.f;
	camera.InvalidateView();

	camera.matProjection_._11 = 2.f / window.Width();
	camera.matProjection_._22 = -2.f / window.Height();
	camera.matProjection_._41 = -1.f;
	camera.matProjection_._42 = 1.f;

	VertexBuffer vb;
	vb.Create(gd, model.verticesGPU_.Source(), model.verticesGPU_.Size(), VertexPC::Decl(), ResourceUsage::ruImmutable);

	IndexBuffer ib;
	ib.Create(gd, nullptr, 1024, ResourceUsage::ruDynamic);
	ib.Update(gc, model.indices_.Source(), model.indices_.Size());

	ConstantBuffer<mat4> cbMatVp;
	cbMatVp.Create(gd);

	ConstantBuffer<mat4> cbMatWorld;
	cbMatWorld.Create(gd);

	while (window.PumpMessage())
	{
		if (input.IsKeyPressed(VK_ESCAPE))
			break;

		if (input.IsKeyDown(VK_LEFT))
		{
			camera.localPos_.x -= 1.f;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_RIGHT))
		{
			camera.localPos_.x += 1.f;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_UP))
		{
			camera.localPos_.y -= 1.f;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_DOWN))
		{
			camera.localPos_.y += 1.f;
			camera.InvalidateView();
		}
			
		if (input.IsKeyDown('A'))
		{
			model.localPos_.x -= 1.f;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('D'))
		{
			model.localPos_.x += 1.f;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('W'))
		{
			model.localPos_.y -= 1.f;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('S'))
		{
			model.localPos_.y += 1.f;
			model.InvalidateTransform();
		}

		if (camera.matViewDirty_)
		{
			camera.matViewDirty_ = false;
			camera.UpdateView();
			cbMatVp.UpdateAndBind(gc, camera.matVp_, 0);
		}
		
		if (model.transformDirty_)
		{
			model.transformDirty_ = false;
			model.UpdateTransform();
			cbMatWorld.UpdateAndBind(gc, model.matWorld_, 1);
		}

		gd.BeginFrame(color::CORNFLOWER_BLUE);

		gc.SetVertexShader(hVs);
		gc.SetPixelShader(hPs);
		gc.SetVertexBuffer(&vb);
		gc.SetIndexBuffer(&ib);
		gc.SetPrimitiveTopology(PrimitiveTopology::ptTriangleList);
		gc.SetRasterizer(CullMode::cmNone, FillMode::fmSolid, FrontFace::ffClockwise);
		gc.SetDepth(DepthMode::dmDisabled);
		gc.SetBlend(BlendMode::bmNone);
		gc.DrawIndexed(model.indices_.Size(), 0, 0);

		gd.Present(true);
		input.NextFrame();
	}

	g_cResourceMgr.Finalize();
	gd.Finalize();
	window.Destroy();
}
