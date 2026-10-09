/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 03. 2D 렌더링 파이프라인 연습 (Practice) - CPU 정점 굽기
 * 각 정점을 CPU에서 계산하여 GPU로 전달해본다.
 * 비효율적이지만 이런 방법은 구시대에 활용되었음.
 */

#include "Core.h"
#include "jc/Primitives/StringConvert.h"
#include "sgf/Graphics/ResourceMgr.h"

using namespace sgf;
using namespace jc;

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// : CPU 에서 이미 계산된 정점 정보를 쉐이더로 전달함 (CPU 정점 굽기)
	// CPU에서 이미 계산된 정점 정보를 전달해주기 때문에 그냥 그대로 output에 전달해주면된다.
	////////////////////////////////////////////////////////////////////////////////////////
	const char* GetShaderSource_PreTransformed()
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

	struct s2DModel
	{
		vec2 localPos_;
		vec2 localScale_ = { 1.f, 1.f };
		vec2 localDirection_ = { 0.f, 0.f };

		bool transformDirty_ = true;
		mat4 matWorld_;

		void InvalidateTransform() { transformDirty_ = true; }
		void UpdateTransform()
		{
			// srt 순으로 행렬곱한다.
			// 행 백터(row major) 기준으로 작업했기 때문.
			// scale 먼저 적용, 그 다음 회전, 마지막 이동 적용
			//                   | a  b  0  0 | 
			//                   | c  d  0  0 | 
			// [ x, y, 0, 1 ] *  | 0  0  1  0 |
			//                   | tx ty 0  1 |

			matWorld_ = mat4::Scale(localScale_)
				* mat4::RotationZ(atan2f(localDirection_.y, localDirection_.x))
				* mat4::Translation(localPos_);
		}

		Vector<vec2> verticesCPU_;
		Vector<_u32> indices_;

		Vector<VertexPC> verticesGPU_;
	};

	struct s2DCamera
	{
		vec2 localPos_;
		_f32 localRotation_ = 0.f;

		bool matViewDirty_ = true;
		mat4 matView_;
		mat4 matProjection_;
		mat4 matVp_;

		void InvalidateView() { matViewDirty_ = true; }
		void UpdateView()
		{
			// 카메라가 우측으로가면 모델은 좌측으로 이동한 것처럼 보여야한다.
			// 카메라가 회전하면 모델은 반대로 회전한 것처럼 보여야한다.
			// 때문에 (S * R)의 역행렬을 구해야한다.

			matView_ = mat4::Translation(-localPos_) * mat4::RotationZ(-localRotation_);
			matVp_ = matView_ * matProjection_;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////////////
void CPUTransformedVertex_Main()
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

	const _u64 hVs = gc.CreateVertexShader(jc::StringConvert::FromUtf8(GetShaderSource_PreTransformed()));
	const _u64 hPs = gc.CreatePixelShader(jc::StringConvert::FromUtf8(GetShaderSource_PreTransformed()));
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
	model.verticesCPU_ =
	{
		{ 0.f, 0.f, },
		{ 1.f, 1.f },
		{ 0.f, 1.f }
	};
	model.verticesGPU_.Resize(model.verticesCPU_.Size());

	model.localPos_ = { 200.f, 300.f };
	model.localScale_ = { 100.f, 100.f };
	model.InvalidateTransform();

	s2DCamera camera;
	camera.localPos_ = { 0.0f, 0.f };
	camera.localRotation_ = 0.f;

	/*
	 * 직교 투영 행렬 (2D라 원근 투영 불필요)
	 * 픽셀 좌표계(좌상단 원점, y 아래로 증가)를 NDC(중앙 원점, -1~1, y 위로 증가)로 변환한다.
	 *
	 * | 2/W  0    0  0 |
	 * | 0   -2/H  0  0 |  x_ndc = x * (2/W) - 1
	 * | 0    0    1  0 |  y_ndc = y * (-2/H) + 1
	 * | -1   1    0  1 |
	 *
	 * ┌──────────────┐    ┌──────────────┐
	 * │(0,0)         │    │(-1,1)        │ (1,1)
	 * │              │    │              │
	 * │              │ →  │     ·(0,0)   │
	 * │              │    │              │
	 * └──────────────┘    └──────────────┘
	 *            (W,H)     (-1,-1)			(1,-1)
	 * 
	 * [y축 변환] 
	 *  y_ndc = y * (-2/H) + 1
	 *
	 *  픽셀 y            NDC y
	 *    0 ───────────→   1
	 *    │                ↑
	 *	  y ───────────→ (-2/H) + 1
	 *    │                │
	 *    ↓                │
	 *    H ───────────→  -1
	 * 
	 *  0 <= y <= 1
	 *  lerp(x,y,t) = x + (y-x)t이므로
	 *  y_ndc = lerp(1, -1, y/H) = 1 + (-2)*(y/H) = y * (-2/H) + 1
	 * 
	 */
	camera.matProjection_._11 = 2.f / window.Width();
	camera.matProjection_._22 = -2.f / window.Height();
	camera.matProjection_._41 = -1.f;
	camera.matProjection_._42 = 1.f;

	VertexBuffer vb;
	vb.Create(gd, nullptr, 1024, VertexPC::Decl(), ResourceUsage::ruDynamic);

	IndexBuffer ib;
	ib.Create(gd, nullptr, 1024, ResourceUsage::ruDynamic);
	ib.Update(gc, model.indices_.Source(), model.indices_.Size());

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
		if (input.IsKeyDown('K'))
		{
			camera.localRotation_ += 1 / jc_math_pi2;
			camera.InvalidateView();
		}
		if (input.IsKeyDown('L'))
		{
			camera.localRotation_ -= 1 / jc_math_pi2;
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
		if (input.IsKeyDown('F'))
		{
			_f32 rot = atanf(model.localDirection_.y / model.localDirection_.x);
			rot -= 1 / jc_math_pi2;
			model.localDirection_.x = cosf(rot);
			model.localDirection_.y = sinf(rot);
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('G'))
		{
			_f32 rot = atanf(model.localDirection_.y / model.localDirection_.x);
			rot += 1 / jc_math_pi2;
			model.localDirection_.x = cosf(rot);
			model.localDirection_.y = sinf(rot);
			model.InvalidateTransform();
		}

		const bool viewChanged = camera.matViewDirty_;
		if (viewChanged)
		{
			// 매번 업데이트하면 성능 낭비가 크므로 변화가 발생했을 때만 행렬계산한다.
			camera.UpdateView();
			camera.matViewDirty_ = false;
		}

		const bool worldChanged = model.transformDirty_;
		if (worldChanged)
		{
			model.UpdateTransform();
			model.transformDirty_ = false;
		}

		if (viewChanged || worldChanged)
		{
			// 모델 행렬에 카메라 행렬을 곱하여 최종 변환행렬을 구한다.
			// 카메라 상에서 보여질 모델의 최종 위치를 계산하기 위해서이다.
			mat4 final = model.matWorld_ * camera.matVp_;

			// 각 정점ㅁ마다 업데이트 수행.
			for (_u32 i = 0; i < model.verticesCPU_.Size(); ++i)
			{
				vec2 g = model.verticesCPU_[i].Mul(final, 1.f);
				model.verticesGPU_[i].position_.x = g.x;
				model.verticesGPU_[i].position_.y = g.y;
				model.verticesGPU_[i].position_.z = 0;
				model.verticesGPU_[i].color_ = color::RED;
			}

			vb.Update(gc, model.verticesGPU_.Source(), model.verticesGPU_.Size());
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
