/*
 * 작성자: 윤정도
 * 생성일: 9/25/2026
 * =====================
 * 04. 3D 렌더링 파이프라인 연습 (Practice) - CPU 정점 굽기
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
			//                   | r00 r01 r02 0 | 
			//                   | r10 r11 r12 0 | 
			// [ x, y, z, 1 ] *  | r20 r21 r22 0 |
			//                   | tx  ty  tz  1 |

			matWorld_ = mat4::Scale(localScale_)
				* (mat4::RotationZ(localRotation_.z) * mat4::RotationY(localRotation_.y) * mat4::RotationX(localRotation_.x))
				* mat4::Translation(localPos_);
		}

		Vector<vec3> verticesCPU_;
		Vector<color> colorsCPU_;
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
			#pragma region 튜토리얼 - 로직과 무관한 코드
			{
				// ----------------------------------------------------------------------
				// 튜토리얼: 좌표계 변환과 View 행렬
				//
				// <기호·용어>
				// World 좌표계 : 아래 점들의 좌표가 처음 표현된 좌표계.
				// c            : World 좌표로 표현한 카메라 위치.
				// o            : 카메라가 바라보는 점.
				// t            : 카메라 좌표계로 변환할 점.
				// forward      : World에서 표현한 카메라 +Y축 방향의 단위 벡터.
				// right        : World에서 표현한 카메라 +X축 방향의 단위 벡터.
				// 기저 벡터    : 좌표를 표현하는 기준 벡터. 여기서는 right와 forward.
				// 정규직교 기저: 기저 벡터들이 서로 수직이고 각각 길이가 1인 기저.
				//
				// 이 예시는 XY 평면의 2D 좌표계이며, 카메라 +Y축을 forward로 삼는다.
				// 실제 3D 카메라에서 사용하는 forward 축과는 구분한다.
				//
				// <좌표계 변환의 의미>
				// 같은 점이라도 기준 원점과 좌표축이 달라지면 좌표값이 달라진다.
				// 좌표계 변환은 점 자체를 움직이는 것이 아니라,
				// 같은 점을 다른 좌표계 기준으로 표현하는 과정이다.
				//
				// <카메라 World 행렬과 View 행렬>
				// CameraWorld : 카메라 좌표 → World 좌표 변환.
				// View        : World 좌표 → 카메라 좌표 변환.
				// 카메라 월드 행렬은 카메라 좌표를 월드 좌표로 변환한다
				// 
				// 예를들어 카메라가 월드상 2, 1에 있다고하자.
				// 카메라 좌표 0,0은 월드상 2, 1로 변환되는걸 알 수 있다.
				// 마찬가지로 회전도 동일하다.
				// 카메라가 월드상 -45도 왼쪽으로 회전해 있다고 하자.
				// 카메라 기저 y는 월드상 -45도 왼쪽으로 회전해있는 방향이란 것을 알 수 있다.
				// 당연한 것이지만 가장 직관적인 뜻이다.
				// 
				// 두 변환은 서로 반대이므로:
				//
				//     View = Inverse(CameraWorld)

				// Docs\sgfr\02_Practice\2d-좌표계변환행렬.html
				// 위 html 파일로 그림을 보면서 ㄱㄱ
				// 뷰 행렬을 이해하기 위해선 좌표계 변환 행렬을 먼저 이해하면 좋다.
				// 이해하기 쉽도록 2d 좌표계 변환을 예시로 들어보자.
				constexpr vec2 t = { 2, 2 };
				constexpr vec2 o = { 0, 0 };
				constexpr vec2 c = { 2, -2 };


				// ----------------------------------------------------------------------
				// 1. 카메라 좌표계의 기저 벡터를 구한다.
				// t점을 c가원점이고 c -> o 방향이 y축인 좌표계로 변환을 했을 때의 행렬이 뭘지 생각해보자.
				// 이를 위해 벡터 Cross/Dot에 대한 이해가 먼저 필요하다.

				// <사전 정보>
				// Cross로 수직 벡터 정보를 얻어낼 수 있고
				// Dot으로 c좌표계 기준에서 t점이 right/foward 방향으로 얼만큼 가지고 있는지 알 수 있다.

				constexpr vec2 forward = (o - c).Normalized();
				constexpr vec2 right = (vec3(forward, 0).Cross({ 0, 0, 1 })).ToVec2(); // Y X Z = X가 된다. 3차원 벡터로 임시 변경해서 처리.

				// ----------------------------------------------------------------------
				// 2. 점 t를 World 좌표에서 카메라 좌표로 변환한다.
				//
				// 먼저 카메라 위치 c를 빼서 c → t 벡터를 구한다.
				// 그다음 각 기저 벡터와 Dot하여 카메라 좌표 성분을 얻는다.
				constexpr vec2 tPos = t - c;
				constexpr vec2 localPos = { right.Dot(tPos), forward.Dot(tPos) };

				// ----------------------------------------------------------------------
				// 3. 점 t가 바라보는 방향(t → o 방향)을 World 좌표계에서 카메라 좌표계로 변환한다.
				// 
				// 그림이 단순하므로 머릿속으로 그려보면서 결과를 예상해볼 수 있다.
				// c가 o를 바라보고 있을 때 y축 방향 기준 135도 방향이 원점이다.
				// 좌표계가 c로 옮겨가면 직관적으로 봤을 때 c 좌표계 기준 y축 방향(forward)기준 90도 방향으로 회전한 모습일 것이다.
				// 즉 c가 원점인 좌표계에서 (-1, 0) 방향이 되어야한다.
				// 회전도 단순하게 벡터로 점을 좌표계 변환하는것처럼 단순화해서 생각하면 편하다.
				constexpr vec2 tDir = o - t;
				constexpr vec2 localDir = vec2(right.Dot(tDir), forward.Dot(tDir)).Normalized(); // -1, 0


				// ----------------------------------------------------------------------
				// 4. 2~3번의 변환 방법으로 View 행렬을 구성한다.
				//
				// 기존 localPos와 localDir은 특정 점의 위치와 방향을 카메라 좌표계 기준으로 변환하는 과정이고
				// 모든 변환에 대응 가능한 행렬을 계산하기 위해선 기저에 대해서 적용해줘야한다.
				// 같은 변환 방법을 다음 세 입력에 적용한다.
				//
				// A : World +X 방향 (1, 0)을 카메라 좌표계로 변환한 결과.
				// B : World +Y 방향 (0, 1)을 카메라 좌표계로 변환한 결과.
				// T : World 원점   (0, 0)을 카메라 좌표계로 변환한 결과.
				//
				// A, B는 3번의 방향 변환 방법으로,
				// T는 2번의 위치 변환 방법으로 계산한다.
				// 방향 변환 결과는 정규화하지 않는다.
				// ----------------------------------------------------------------------

				const auto ToCameraPosition = [&](vec2 worldPos) -> vec2
				{
					const vec2 relative = worldPos - c;

					return {
						right.Dot(relative),
						forward.Dot(relative)
					};
				};

				const auto ToCameraDirection = [&](vec2 worldDir) -> vec2
				{
					return {
						right.Dot(worldDir),
						forward.Dot(worldDir)
					};
				};

				const vec2 A = ToCameraDirection(vec2{ 1.f, 0.f });
				const vec2 B = ToCameraDirection(vec2{ 0.f, 1.f });
				const vec2 T = ToCameraPosition(vec2{ 0.f, 0.f });

				// <계산 결과를 행렬에 배치한다>
				// A를 첫째 행, B를 둘째 행, T를 마지막 행에 배치한다.
				//
				//         | A.x  A.y  0 |
				//     V = | B.x  B.y  0 |
				//         | T.x  T.y  1 |
				//
				mat4 viewMat;
				viewMat._11 = A.x; viewMat._12 = A.y;
				viewMat._21 = B.x; viewMat._22 = B.y;
				viewMat._41 = T.x; viewMat._42 = T.y;
			}
			#pragma endregion

			// 아래 코드 이해안가면 pragma region 튜토리얼 참고 할 것
			const _f32 upLength = up_.Length();
			vec3 upHint = up_;
			if (upLength > 1) upHint.Normalize();
			jc_assert(upLength > 0);

			// 3d도 위 튜토리얼과 동일하게 적용하면 된다.
			vec3 f = (targetPos_ - localPos_).Normalized(); // camera forward (camera z)
			vec3 r = (f.Cross(upHint) * -1).Normalized(); // camera right (camera x), Y X Z = X 따라서 Z X Y = -X = up_.Cross(f)와 같음. 일단 이렇게 둠
			vec3 u = f.Cross(r); // camera up, (camera y) Z X X = Y

			// translation 적용
			// world(0, 0, 0) - cameraPos
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
void CPUTransformedVertex3D_Main()
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

	// 정육면체 정점 8개 (각 꼭짓점마다 다른 색 -> 면 색이 그라데이션으로 보간된다)
	//
	//	4 -------- 5
	//	/| /|
	//	0 -------- 1 | (z가 작은 쪽이 앞면: 0,1,2,3)
	//	| 6 -------|-7
	//	|/ |/
	//	2 -------- 3
	s3DModel model;
	model.verticesCPU_ =
	{
		{ -0.5f, +0.5f, -0.5f },	// 0: 앞-왼위
		{ +0.5f, +0.5f, -0.5f },	// 1: 앞-오른위
		{ -0.5f, -0.5f, -0.5f },	// 2: 앞-왼아래
		{ +0.5f, -0.5f, -0.5f },	// 3: 앞-오른아래
		{ -0.5f, +0.5f, +0.5f },	// 4: 뒤-왼위
		{ +0.5f, +0.5f, +0.5f },	// 5: 뒤-오른위
		{ -0.5f, -0.5f, +0.5f },	// 6: 뒤-왼아래
		{ +0.5f, -0.5f, +0.5f },	// 7: 뒤-오른아래
	};
	model.colorsCPU_ =
	{
		color(0xFF, 0x00, 0x00, 0xFF),
		color(0x00, 0xFF, 0x00, 0xFF),
		color(0x00, 0x00, 0xFF, 0xFF),
		color(0xFF, 0xFF, 0x00, 0xFF),
		color(0xFF, 0x00, 0xFF, 0xFF),
		color(0x00, 0xFF, 0xFF, 0xFF),
		color(0xFF, 0xFF, 0xFF, 0xFF),
		color(0x4D, 0x4D, 0x4D, 0xFF),
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
	model.verticesGPU_.Resize(model.verticesCPU_.Size());

	model.localPos_ = { 0.f, 0.f, 0.f };
	model.localScale_ = { 1.f, 1.f, 1.f };
	model.InvalidateTransform();

	// 카메라를 원점 뒤쪽(z = -2.5)에 두고 원점을 바라본다. (LH: +z가 화면 안쪽)
	s3DCamera camera;
	camera.localPos_ = { 0.f, 0.f, -2.5f };
	camera.targetPos_ = { 0.f, 0.f, 0.f };
	camera.up_ = vec3::UP;

	/*
	 * 원근 투영 행렬 (Perspective Projection) - 3D라 원근감을 적용한다.
	 * 카메라 공간의 정점을 원근감이 적용된 클립 좌표로 변환한다.
	 * 멀수록 작게, 가까울수록 크게 보이게 하고, near ~ far 깊이를 0 ~ 1로 매핑한다.
	 *
	 * [왜 나눗셈을 w에 넣는가]
	 *  y_ndc = y1 / (z1 * tan(fov/2)) 처럼 z1로 나누는 식은 행렬의 곱/합만으로 표현할 수 없다.
	 *  그래서 나눗셈의 분모(z1)를 w 성분에 그대로 담아두고, GPU가 클립 좌표를 w로 나눈다.
	 *
	 *  [x1 y1 z1 1] * M_proj = [x_clip y_clip z_clip w_clip]   (w_clip = z1)
	 *  rasterizer가 x_clip/w, y_clip/w, z_clip/w 로 나눠 NDC를 만든다.
	 *
	 * [near 평면에 투영 (닮은 삼각형)]
	 *  y_n / n = y1 / z1   =>   y_n = n * y1 / z1
	 *
	 * [화면 경계로 정규화]
	 *  near 평면의 반높이 h_n = n * tan(fovY/2)
	 *  y_ndc = y_n / h_n = y1 / (z1 * tan(fovY/2))     (n이 약분된다)
	 *  x_ndc = x1 / (z1 * tan(fovY/2) * aspect)
	 *
	 * [행렬 원소]
	 *  sy = 1 / tan(fovY/2)          세로 스케일
	 *  sx = sy / aspect              가로 스케일 (aspect = W / H)
	 *  A  = far / (far - near)       깊이: near -> 0, far -> 1   (z_ndc = A + B / z1)
	 *  B  = -near * far / (far - near)
	 *
	 *      | sx  0   0   0 |
	 *      | 0   sy  0   0 |
	 *      | 0   0   A   1 |
	 *      | 0   0   B   0 |
	 *
	 *  [x1 y1 z1 1] * M_proj = [sx*x1  sy*y1  A*z1+B  z1]
	 *
	 * 이 설명만 봐도 이해 안되면 Docs/sgfr/02_Practice/projection-matrix.html 파일 참고해서 볼 것
	 */
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
	vb.Create(gd, nullptr, 1024, VertexPC::Decl(), ResourceUsage::ruDynamic);

	IndexBuffer ib;
	ib.Create(gd, nullptr, 1024, ResourceUsage::ruDynamic);
	ib.Update(gc, model.indices_.Source(), model.indices_.Size());

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

			// 각 정점마다 업데이트 수행.
			for (_u32 i = 0; i < model.verticesCPU_.Size(); ++i)
			{
				// 원근 투영은 w로 나눈 뒤에야 NDC가 나오므로 vec4로 클립 좌표를 구한 뒤 w로 나눈다.
				vec4 clip = vec4(model.verticesCPU_[i], 1.f).Mul(final);
				vec3 ndc = clip.ToVec3() / clip.w;

				model.verticesGPU_[i].position_ = ndc;
				model.verticesGPU_[i].color_ = model.colorsCPU_[i];
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
