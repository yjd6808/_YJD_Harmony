/*
 * 작성자: 윤정도
 * 생성일: 8/17/2026
 * =====================
 * 33. 프리미티브 셰이프 (Shape2D/Shape3D) - 구현부
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. Shape2D — 2D 프리미티브 편의 노드: SetRect/SetCircle/SetLine/SetTriangle만 하면
 *    AddChild 직후 OnEnter에서 DeclareStaticXX로 1회 bake → 이후 자동 드로우. (정적 UI용 — A 방식)
 * 2. Shape3D — 3D 프리미티브 편의 노드: SetShape(타입, 색) + 위치/크기/회전만 하면
 *    OnEnter에서 메시+머티리얼을 1회 구성하고 OnRender에서 메시 자동 드로우. (움직이는 도형용 — B 방식)
 * 3. Shape3D는 Transform으로 자유롭게 움직인다 — 씬 2의 6종이 각자 회전한다. (GPU 변환이므로 정점 무변형)
 * 4. 2D 셰이프(Shape2D)는 CPU 배칭 경로 — 전부 흰색 텍스처로 배칭되어 드로우콜이 1번만 발생한다.
 * (창 제목에 프레임당 드로우콜 수를 실시간 표시해 검증한다. Mesh+GameObject는 오브젝트당 1콜)
 * 5. 참고 — Mesh+GameObject는 B(다이나믹)가 기본이고, SetMeshStaticLevel(slStatic) 명시 호출로
 *    A(스태틱 = 월드 행렬 고정) 전환이 가능하다. (튜토리얼 32 씬 1 참고) Shape2D는 A 전용 편의 노드이다.
 *
 * [조작]
 * SPACE: 2D/3D 씬 전환, ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/33_PrimitiveShape/33_PrimitiveShape_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	constexpr _f32 VIEW_WIDTH = 800.0f;		// 가상 화면 가로
	constexpr _f32 VIEW_HEIGHT = 600.0f;	// 가상 화면 세로

	// 창 제목에 프레임당 드로우콜 수를 실시간으로 표시한다. (배칭 검증용)
	void SetTitleDrawCall(Window* _pWindow, const char* _pFormat, _u32 _drawCalls)
	{
		jc::String title = jc::StringUtil::Format(_pFormat, _drawCalls);
		_pWindow->SetTitle(title);
	}

	// 씬 공장 함수 선언: 두 씬이 서로를 교체하므로 앞서 선언만 해둔다.
	Scene* CreateShape2DScene();
	Scene* CreateShape3DScene();

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 1] Shape2D — A(스태틱) 방식 편의 노드. 정적 2D 셰이프 4종을 절대좌표로 bake해 배치한다.
	// (움직이는 2D 오브젝트가 필요하면 Mesh+GameObject — B 기본, 튜토리얼 32 씬 1 참고)
	// 4개가 모두 같은 흰색 텍스처로 배칭되어 실제 GPU 드로우콜이 1번만 발생한다.
	class Shape2DScene : public Scene2D
	{
	public:
		~Shape2DScene()
		{
			// root_ 트리는 자식을 소유하지 않으므로(분리만) 직접 정리한다.
			for (GameObject* pObj : createdObjects_)
			{
			RemoveChild(pObj);
			delete pObj;
			}
		}

		// 씬 진입: 카메라 + Shape2D 4종 배치 (AddChild 직후 1회 bake)
		void OnEnter() override
		{
			GetCamera2D()->SetOrthographic2D(VIEW_WIDTH, VIEW_HEIGHT);
			GetWindow()->SetTitle("33. Shape2D - SPACE: 3D 씬 (ESC: 종료)");

			// 사각형 — 채운 사각형 패널 (절대좌표: 왼쪽 위 200,150, 크기 120x90)
			Shape2D* pRect = new Shape2D("Rect");
			pRect->SetRect(rect(200.0f, 150.0f, 120.0f, 90.0f), color(0xFF, 0x6B, 0x6B));
			AddChild(pRect, 0);
			createdObjects_.PushBack(pRect);

			// 원 — 중심 (380, 300), 반지름 60, 기본 32분할
			Shape2D* pCircle = new Shape2D("Circle");
			pCircle->SetCircle(vec2(380.0f, 300.0f), 60.0f, color(0xFF, 0xCC, 0x4D));
			AddChild(pCircle, 0);
			createdObjects_.PushBack(pCircle);

			// 선 — 두께 6의 선분 (절대좌표 두 점)
			Shape2D* pLine = new Shape2D("Line");
			pLine->SetLine(vec2(500.0f, 180.0f), vec2(650.0f, 420.0f), 6.0f, color(0x8A, 0xFF, 0x8A));
			AddChild(pLine, 0);
			createdObjects_.PushBack(pLine);

			// 삼각형 — 절대좌표 세 점
			Shape2D* pTriangle = new Shape2D("Triangle");
			pTriangle->SetTriangle(vec2(200.0f, 420.0f), vec2(260.0f, 300.0f), vec2(330.0f, 440.0f), color(0x6B, 0xD0, 0xFF));
			AddChild(pTriangle, 0);
			createdObjects_.PushBack(pTriangle);

			_LogInfo_("[33] Shape2DScene::OnEnter — Shape2D 4종 배치 완료");
		}

		// 매 프레임: 지난 프레임의 실제 GPU 드로우콜 수를 창 제목에 표시 (배칭이 잘 되면 1)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			(void)_dt;
			SetTitleDrawCall(GetWindow(),
			"33. Shape2D 4종 - DrawCall: %u (SPACE: 3D 씬, ESC: 종료)",
			g_cRenderer2D.GetDrawCallCount());

			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
			g_cDirector.ReplaceScene(CreateShape3DScene());
			}
		}

	private:
		jc::Vector<GameObject*> createdObjects_;	// 소멸자에서 정리할 오브젝트 목록
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 2] Shape3D — 3D 프리미티브 6종을 일렬로 배치하고, 각자 자전시킨다.
	// Shape2D와 달리 bake가 없으므로 Transform(회전/위치)을 프레임마다 바꿔도 자동 반영된다.
	class Shape3DScene : public Scene3D
	{
	public:
		~Shape3DScene()
		{
			for (GameObject* pObj : createdObjects_)
			{
			RemoveChild(pObj);
			delete pObj;
			}
		}

		// 씬 진입: 카메라 + Shape3D 6종 배치 (AddChild 직후 메시/머티리얼 1회 구성)
		void OnEnter() override
		{
			GetCamera3D()->SetPerspectiveDegrees(60.0f, GetWindow()->AspectRatio(), 0.1f, 1000.0f);
			GetCamera3D()->SetLookAt(vec3(0.0f, 2.5f, -8.0f), vec3::Zero());
			GetWindow()->SetTitle("33. Shape3D 6종 (SPACE: 2D 씬, ESC: 종료)");

			const _f32 xs[] = { -5.5f, -3.3f, -1.1f, 1.1f, 3.3f, 5.5f };
			CreateShape("Cube", PrimitiveMesh3DType::Cube, vec3(xs[0], 0.0f, 0.0f), 1.1f, color(0xFF, 0x6B, 0x6B));
			CreateShape("Sphere", PrimitiveMesh3DType::Sphere, vec3(xs[1], 0.0f, 0.0f), 1.0f, color(0xFF, 0xCC, 0x4D));
			CreateShape("Capsule", PrimitiveMesh3DType::Capsule, vec3(xs[2], 0.0f, 0.0f), 1.0f, color(0x6B, 0xD0, 0xFF));
			CreateShape("Cylinder", PrimitiveMesh3DType::Cylinder, vec3(xs[3], 0.0f, 0.0f), 1.0f, color(0x8A, 0xFF, 0x8A));
			CreateShape("Plane", PrimitiveMesh3DType::Plane, vec3(xs[4], 0.0f, 0.0f), 1.2f, color(0xFF, 0x8A, 0xD0));
			CreateShape("Quad", PrimitiveMesh3DType::Quad, vec3(xs[5], 0.0f, 0.0f), 1.2f, color(0xC0, 0xA0, 0xFF));
			_LogInfo_("[33] Shape3DScene::OnEnter — Shape3D 6종 배치 완료");
		}

		// 매 프레임: 각 도형을 제자리에서 자전시킨다. (bake가 없는 Shape3D의 자유도)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			rotateAngle_ += static_cast<_f32>(_dt.GetTotalSeconds()) * 45.0f;	// 초당 45도

			for (_s32 i = 0; i < createdObjects_.Size(); ++i)
			{
			Transform* pTransform = createdObjects_[i]->GetTransform();
			pTransform->SetLocalRotationEuler(
				rotateAngle_ * (0.4f + 0.2f * (_f32)i),
				rotateAngle_ * (0.5f + 0.15f * (_f32)i),
				0.0f);
			}

			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
			g_cDirector.ReplaceScene(CreateShape2DScene());
			}
		}

	private:
		Shape3D* CreateShape(const char* _pName, PrimitiveMesh3DType _type,
			const vec3& _position, _f32 _scale, const color& _color)
		{
			Shape3D* pShape = new Shape3D(_pName);
			pShape->SetShape(_type, _color);
			pShape->SetPosition(_position);
			pShape->SetScale(_scale);
			AddChild(pShape, 0);
			createdObjects_.PushBack(pShape);
			return pShape;
		}

		jc::Vector<GameObject*> createdObjects_;	// 소멸자에서 정리할 오브젝트 목록
		_f32 rotateAngle_ = 0.0f;				// 자전 각도 누적 (도)
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// 씬 공장 함수 정의 — Director가 이전 씬을 delete하므로 반드시 new로 만든다.
	Scene* CreateShape2DScene()
	{
		return new Shape2DScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	Scene* CreateShape3DScene()
	{
		return new Shape3DScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 데모 앱: Application을 상속받아 생명주기 훅만 재정의한다. (cocos AppDelegate 스타일)
	class DemoApp : public Application
	{
	protected:
		// 엔진 준비 완료 직후: 첫 씬을 시작한다.
		bool ApplicationDidFinishLaunching() override
		{
			_LogInfo_("[33] DemoApp::ApplicationDidFinishLaunching — 첫 씬 시작");
			g_cDirector.RunScene(CreateShape2DScene());
			return true;
		}

		// 매 프레임: ESC 종료 처리
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			if (g_cInput.IsKeyPressed(VK_ESCAPE))
			{
			g_cWindow.Destroy();
			}
		}
	};
}

//////////////////////////////////////////////////////////////////////////////////////////
// 튜토리얼 진입점
void PrimitiveShape_Main()
{
	_LogInfo_("[33] 프리미티브 셰이프 튜토리얼 시작");

	DemoApp app;
	if (!app.Initialize("33. 프리미티브 셰이프 (SPACE: 씬 전환, ESC: 종료)", 800, 600))
	{
		jc::Console::WriteLine("엔진 초기화에 실패했습니다.");
		return;
	}

	app.SetClearColor(color(0x0A, 0x0A, 0x17));
	app.Run();
	app.Finalize();
}