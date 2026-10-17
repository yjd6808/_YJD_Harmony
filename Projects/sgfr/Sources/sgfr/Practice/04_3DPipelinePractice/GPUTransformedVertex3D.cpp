/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 04. 3D 렌더링 파이프라인 연습 (Practice) - GPU 정점 굽기
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

	struct s3DModel
	{
		vec3 localPos_;
		vec3 localScale_ = { 1.f, 1.f, 1.f };
		vec3 localRotation_ = { 0.f, 0.f, 0.f };	// x = pitch, y = yaw, z = roll

		bool transformDirty_ = true;
		mat4 matWorld_;

		void InvalidateTransform() { transformDirty_ = true; }
		void UpdateTransform()
		{
			// srt 순으로 행렬곱한다.
			// 행 백터(row major) 기준으로 작업했기 때문.
			// scale 먼저 적용, 그 다음 회전, 마지막 이동 적용
			matWorld_ = mat4::Scale(localScale_)
				* (mat4::RotationZ(localRotation_.z) * mat4::RotationY(localRotation_.y) * mat4::RotationX(localRotation_.x))
				* mat4::Translation(localPos_);
		}

		Vector<_u32> indices_;
		Vector<VertexPC> verticesGPU_;
	};

	struct s3DCamera
	{
		vec3 localPos_;			// eye
		vec3 targetPos_;		// target
		vec3 up_ = vec3::UP;	// up

		bool matViewDirty_ = true;
		mat4 matView_;
		mat4 matProjection_;
		mat4 matVp_; // matViewProjection_

		void InvalidateView() { matViewDirty_ = true; }
		void UpdateView()
		{
			// LookAt 방식으로 View 행렬을 구성한다. (자세한 설명은 CPU 정점 굽기 파일 참고)
			const _f32 upLength = up_.Length();
			vec3 upHint = up_;
			if (upLength > 1) upHint.Normalize();
			jc_assert(upLength > 0);

			vec3 f = (targetPos_ - localPos_).Normalized(); // camera forward (camera z)
			vec3 r = (f.Cross(upHint) * -1).Normalized(); // camera right (camera x)
			vec3 u = f.Cross(r); // camera up, (camera y)

			// translation 적용
			matView_._41 = -r.Dot(localPos_);
			matView_._42 = -u.Dot(localPos_);
			matView_._43 = -f.Dot(localPos_);

			// rotation 적용
			matView_._11 = r.Dot(vec3::RIGHT);
			matView_._12 = u.Dot(vec3::RIGHT);
			matView_._13 = f.Dot(vec3::RIGHT);

			matView_._21 = r.Dot(vec3::UP);
			matView_._22 = u.Dot(vec3::UP);
			matView_._23 = f.Dot(vec3::UP);

			matView_._31 = r.Dot(vec3::FORWARD);
			matView_._32 = u.Dot(vec3::FORWARD);
			matView_._33 = f.Dot(vec3::FORWARD);

			matVp_ = matView_ * matProjection_;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////////////
void GPUTransformedVertex3D_Main()
{
	jc::Console::WriteLine(_T("[Practice 04] 3D 렌더링 파이프라인 연습 - ESC 종료"));

	Window window;
	if (!window.Create(_T("Practice 04. 3D 렌더링 파이프라인 연습 (ESC 종료)"), 800, 600))
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

	// 정육면체 정점 8개 (각 꼭짓점마다 다른 색 -> 면 색이 그라데이션으로 보간된다)
	s3DModel model;
	model.verticesGPU_ =
	{
		{ { -0.5f, +0.5f, -0.5f }, color(0xFF, 0x00, 0x00, 0xFF) },	// 0: 앞-왼위
		{ { +0.5f, +0.5f, -0.5f }, color(0x00, 0xFF, 0x00, 0xFF) },	// 1: 앞-오른위
		{ { -0.5f, -0.5f, -0.5f }, color(0x00, 0x00, 0xFF, 0xFF) },	// 2: 앞-왼아래
		{ { +0.5f, -0.5f, -0.5f }, color(0xFF, 0xFF, 0x00, 0xFF) },	// 3: 앞-오른아래
		{ { -0.5f, +0.5f, +0.5f }, color(0xFF, 0x00, 0xFF, 0xFF) },	// 4: 뒤-왼위
		{ { +0.5f, +0.5f, +0.5f }, color(0x00, 0xFF, 0xFF, 0xFF) },	// 5: 뒤-오른위
		{ { -0.5f, -0.5f, +0.5f }, color(0xFF, 0xFF, 0xFF, 0xFF) },	// 6: 뒤-왼아래
		{ { +0.5f, -0.5f, +0.5f }, color(0x4D, 0x4D, 0x4D, 0xFF) },	// 7: 뒤-오른아래
	};
	// 인덱스 36개 (6면 x 삼각형 2개 x 3). 바깥에서 볼 때 시계 방향.
	model.indices_ =
	{
		0, 1, 2,  2, 1, 3,	// 앞면 (z-)
		5, 4, 7,  7, 4, 6,	// 뒷면 (z+)
		4, 5, 0,  0, 5, 1,	// 윗면 (y+)
		2, 3, 6,  6, 3, 7,	// 아랫면 (y-)
		4, 0, 6,  6, 0, 2,	// 왼면 (x-)
		1, 5, 3,  3, 5, 7,	// 오른면 (x+)
	};

	model.localPos_ = { 0.f, 0.f, 0.f };
	model.localScale_ = { 1.f, 1.f, 1.f };
	model.InvalidateTransform();

	// 카메라를 원점 뒤쪽(z = -2.5)에 두고 원점을 바라본다. (LH: +z가 화면 안쪽)
	s3DCamera camera;
	camera.localPos_ = { 0.f, 0.f, -2.5f };
	camera.targetPos_ = { 0.f, 0.f, 0.f };
	camera.up_ = vec3::UP;
	camera.InvalidateView();

	// 원근 투영 행렬 (자세한 유도는 CPU 정점 굽기 파일과 Docs/sgfr/02_Practice/projection-matrix.html 참고)
	const _f32 fovY = jc_math_deg2rad(60.0f);
	const _f32 aspect = window.AspectRatio();
	const _f32 nearZ = 0.1f;
	const _f32 farZ = 100.0f;

	const _f32 sy = 1.0f / tanf(fovY * 0.5f);
	const _f32 sx = sy / aspect;
	const _f32 depthA = farZ / (farZ - nearZ);
	const _f32 depthB = -nearZ * farZ / (farZ - nearZ);

	camera.matProjection_._11 = sx;
	camera.matProjection_._22 = sy;
	camera.matProjection_._33 = depthA;
	camera.matProjection_._34 = 1.0f;
	camera.matProjection_._43 = depthB;
	camera.matProjection_._44 = 0.0f;

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

		const _f32 moveStep = 0.03f;
		const _f32 rotateStep = 0.03f;

		// 카메라 이동 (eye와 target을 함께 이동시켜 바라보는 방향은 유지)
		if (input.IsKeyDown(VK_LEFT))
		{
			camera.localPos_.x -= moveStep;
			camera.targetPos_.x -= moveStep;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_RIGHT))
		{
			camera.localPos_.x += moveStep;
			camera.targetPos_.x += moveStep;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_UP))
		{
			camera.localPos_.y += moveStep;
			camera.targetPos_.y += moveStep;
			camera.InvalidateView();
		}
		if (input.IsKeyDown(VK_DOWN))
		{
			camera.localPos_.y -= moveStep;
			camera.targetPos_.y -= moveStep;
			camera.InvalidateView();
		}

		// 모델 이동 (x, y, z)
		if (input.IsKeyDown('A'))
		{
			model.localPos_.x -= moveStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('D'))
		{
			model.localPos_.x += moveStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('Q'))
		{
			model.localPos_.y += moveStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('E'))
		{
			model.localPos_.y -= moveStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('W'))
		{
			model.localPos_.z += moveStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('S'))
		{
			model.localPos_.z -= moveStep;
			model.InvalidateTransform();
		}

		// 모델 회전 (y = yaw, x = pitch, z = roll)
		if (input.IsKeyDown('F'))
		{
			model.localRotation_.y -= rotateStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('G'))
		{
			model.localRotation_.y += rotateStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('R'))
		{
			model.localRotation_.x -= rotateStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('T'))
		{
			model.localRotation_.x += rotateStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('V'))
		{
			model.localRotation_.z -= rotateStep;
			model.InvalidateTransform();
		}
		if (input.IsKeyDown('B'))
		{
			model.localRotation_.z += rotateStep;
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
		gc.SetDepth(DepthMode::dmReadWrite);
		gc.SetBlend(BlendMode::bmNone);
		gc.DrawIndexed(model.indices_.Size(), 0, 0);

		gd.Present(true);
		input.NextFrame();
	}

	g_cResourceMgr.Finalize();
	gd.Finalize();
	window.Destroy();
}
