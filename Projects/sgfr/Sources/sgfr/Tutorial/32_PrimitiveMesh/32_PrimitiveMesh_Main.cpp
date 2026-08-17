/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026
 * =====================
 * 32. 프리미티브 메시 (리소스 매니저) - 구현부
 *
 * [이 튜토리얼에서 배우는 것]
 * 1. g_cResourceMgr.FindPrimitiveMesh2D / FindPrimitiveMesh3D로 디폴트 프리미티브 메시를 꺼낸다.
 * - 2D: Rect / Circle / Triangle / Line
 * - 3D: Cube / Sphere / Capsule / Cylinder / Plane / Quad (Unity 기본 메시 제공)
 * 2. PrimitiveMesh2DType은 3D 메시(Cube 등)를 포함한 슈퍼셋이다.
 * 2D enum으로 3D 메시를 받아올 수 있고, 3D enum으로는 2D 메시를 받아올 수 없다. (컴파일 타임 보장)
 * 3. GameObject에 SetMesh만 하면 렌더 트래버설 중 자동으로 그려진다. (메시 자동 드로우)
 * 4. 2D 프리미티브들은 전부 흰색 텍스처로 배칭된다. → 실제 GPU 드로우콜이 정확히 1번만 발생한다.
 * (창 제목에 프레임당 드로우콜 수를 실시간 표시해 검증한다)
 *
 * [조작]
 * SPACE: 2D/3D 씬 전환, ESC: 종료
 */

#include "Core.h"
#include "sgfr/Tutorial/32_PrimitiveMesh/32_PrimitiveMesh_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	constexpr _f32 VIEW_WIDTH = 800.0f;		// 가상 화면 가로
	constexpr _f32 VIEW_HEIGHT = 600.0f;	// 가상 화면 세로

	// 창 제목에 프레임당 드로우콜 수를 실시간으로 표시한다. (배칭 검증용)
	void SetTitleDrawCall(Window* _pWindow, const wchar_t* _pFormat, _u32 _drawCalls)
	{
		wchar_t title[160];
		swprintf_s(title, _countof(title), _pFormat, _drawCalls);
		_pWindow->SetTitle(title);
	}

	// 씬 공장 함수 선언: 두 씬이 서로를 교체하므로 앞서 선언만 해둔다.
	Scene* CreatePrimitive2DScene();
	Scene* CreatePrimitive3DScene();

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 1] 2D 프리미티브 — 리소스 매니저에서 2D 메시 4종을 꺼내 GameObject로 배치한다.
	// 4개가 모두 같은 흰색 텍스처로 배칭되어 실제 GPU 드로우콜이 1번만 발생한다.
	class Primitive2DScene : public Scene2D
	{
	public:
		~Primitive2DScene()
		{
			// root_ 트리는 자식을 소유하지 않으므로(분리만) 직접 정리한다.
			for (GameObject* pObj : createdObjects_)
			{
				RemoveChild(pObj);
				delete pObj;
			}
		}

		// 씬 진입: 카메라 + 리소스 매니저에서 메시를 꺼내 4개의 GameObject를 배치
		void OnEnter() override
		{
			GetCamera2D()->SetOrthographic2D(VIEW_WIDTH, VIEW_HEIGHT);
			GetWindow()->SetTitle(L"32. 2D 프리미티브 - SPACE: 3D 씬 (ESC: 종료)");

			Mesh* pRect = g_cResourceMgr.FindPrimitiveMesh2D(PrimitiveMesh2DType::Rect);
			Mesh* pCircle = g_cResourceMgr.FindPrimitiveMesh2D(PrimitiveMesh2DType::Circle);
			Mesh* pTriangle = g_cResourceMgr.FindPrimitiveMesh2D(PrimitiveMesh2DType::Triangle);
			Mesh* pLine = g_cResourceMgr.FindPrimitiveMesh2D(PrimitiveMesh2DType::Line);

			Create2DObject("Rect", pRect, vec2(200.0f, 300.0f), vec3(150.0f, 100.0f, 1.0f), 0.0f, color(0xFF, 0x6B, 0x6B));
			Create2DObject("Circle", pCircle, vec2(350.0f, 300.0f), vec3(80.0f, 80.0f, 1.0f), 0.0f, color(0xFF, 0xCC, 0x4D));
			Create2DObject("Triangle", pTriangle, vec2(500.0f, 300.0f), vec3(140.0f, 120.0f, 1.0f), 0.0f, color(0x6B, 0xD0, 0xFF));
			Create2DObject("Line", pLine, vec2(660.0f, 300.0f), vec3(180.0f, 10.0f, 1.0f), 0.5f, color(0x8A, 0xFF, 0x8A));
			_LogInfo_("[32] Primitive2DScene::OnEnter — 2D 프리미티브 4종 배치 완료");
		}

		// 매 프레임: 지난 프레임의 실제 GPU 드로우콜 수를 창 제목에 표시 (배칭이 잘 되면 1)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			(void)_dt;
			SetTitleDrawCall(GetWindow(),
				L"32. 2D 프리미티브 - DrawCall: %u (SPACE: 3D 씬, ESC: 종료)",
				g_cRenderer2D.GetDrawCallCount());

			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
				g_cDirector.ReplaceScene(CreatePrimitive3DScene());
			}
		}

	private:
		GameObject* Create2DObject(const char* _pName, Mesh* _pMesh, const vec2& _position,
			const vec3& _scale, _f32 _radian, const color& _tint)
		{
			GameObject* pObj = new GameObject(_pName);
			pObj->SetMesh(_pMesh);
			pObj->GetMaterial()->SetBaseColor(_tint);
			pObj->GetTransform()->SetLocalPosition(_position);
			pObj->GetTransform()->SetLocalScale(_scale);
			pObj->GetTransform()->SetLocalRotationRad(_radian);
			AddChild(pObj, 0);
			createdObjects_.PushBack(pObj);
			return pObj;
		}

		jc::Vector<GameObject*> createdObjects_;	// 소멸자에서 정리할 오브젝트 목록
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 2] 3D 프리미티브 — Unity 기본 메시 6종을 일렬로 배치한다.
	// Cube 1개는 FindPrimitiveMesh2D(PrimitiveMesh2DType::Cube)로 받아 "2D enum = 슈퍼셋"을 시연한다.
	class Primitive3DScene : public Scene3D
	{
	public:
		~Primitive3DScene()
		{
			for (GameObject* pObj : createdObjects_)
			{
				RemoveChild(pObj);
				delete pObj;
			}
		}

		// 씬 진입: 카메라 + 리소스 매니저에서 3D 메시 6종을 꺼내 일렬 배치
		void OnEnter() override
		{
			GetCamera3D()->SetPerspectiveDegrees(60.0f, GetWindow()->AspectRatio(), 0.1f, 1000.0f);
			GetCamera3D()->SetLookAt(vec3(0.0f, 2.5f, -8.0f), vec3::Zero());
			GetWindow()->SetTitle(L"32. 3D 프리미티브 6종 (SPACE: 2D 씬, ESC: 종료)");

			// Cube 1개는 2D enum(슈퍼셋), 나머지 5종은 3D enum으로 꺼낸다.
			Mesh* pCube = g_cResourceMgr.FindPrimitiveMesh2D(PrimitiveMesh2DType::Cube);
			Mesh* pSphere = g_cResourceMgr.FindPrimitiveMesh3D(PrimitiveMesh3DType::Sphere);
			Mesh* pCapsule = g_cResourceMgr.FindPrimitiveMesh3D(PrimitiveMesh3DType::Capsule);
			Mesh* pCylinder = g_cResourceMgr.FindPrimitiveMesh3D(PrimitiveMesh3DType::Cylinder);
			Mesh* pPlane = g_cResourceMgr.FindPrimitiveMesh3D(PrimitiveMesh3DType::Plane);
			Mesh* pQuad = g_cResourceMgr.FindPrimitiveMesh3D(PrimitiveMesh3DType::Quad);

			const _f32 xs[] = { -5.5f, -3.3f, -1.1f, 1.1f, 3.3f, 5.5f };
			Create3DObject("Cube", pCube, vec3(xs[0], 0.0f, 0.0f), vec3(1.1f, 1.1f, 1.1f), color(0xFF, 0x6B, 0x6B));
			Create3DObject("Sphere", pSphere, vec3(xs[1], 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), color(0xFF, 0xCC, 0x4D));
			Create3DObject("Capsule", pCapsule, vec3(xs[2], 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), color(0x6B, 0xD0, 0xFF));
			Create3DObject("Cylinder", pCylinder, vec3(xs[3], 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), color(0x8A, 0xFF, 0x8A));
			Create3DObject("Plane", pPlane, vec3(xs[4], 0.0f, 0.0f), vec3(1.2f, 1.2f, 1.2f), color(0xFF, 0x8A, 0xD0));
			Create3DObject("Quad", pQuad, vec3(xs[5], 0.0f, 0.0f), vec3(1.2f, 1.2f, 1.2f), color(0xC0, 0xA0, 0xFF));
			_LogInfo_("[32] Primitive3DScene::OnEnter — 3D 프리미티브 6종 배치 완료");
		}

		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			(void)_dt;
			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
				g_cDirector.ReplaceScene(CreatePrimitive2DScene());
			}
		}

	private:
		GameObject* Create3DObject(const char* _pName, Mesh* _pMesh, const vec3& _position,
			const vec3& _scale, const color& _tint)
		{
			GameObject* pObj = new GameObject(_pName);
			pObj->SetMesh(_pMesh);

			// 3D 렌더러(Renderer3D)는 재질의 셰이더 키를 해석해 파이프라인을 구성한다.
			// GameObject 기본 재질에는 키가 없으므로 디폴트 3D 셰이더를 지정해준다.
			Material* pMat = pObj->GetMaterial();
			pMat->SetVertexShaderKey(g_cResourceMgr.GetDefaultVertexShader3DKey());
			pMat->SetPixelShaderKey(g_cResourceMgr.GetDefaultPixelShader3DKey());
			pMat->SetBaseColor(_tint);
			pMat->Initialize(&g_cDevice);		// GPU 상태(파이프라인 상태/상수 버퍼) 초기화

			pObj->GetTransform()->SetLocalPosition(_position);
			pObj->GetTransform()->SetLocalScale(_scale);
			AddChild(pObj, 0);
			createdObjects_.PushBack(pObj);
			return pObj;
		}

		jc::Vector<GameObject*> createdObjects_;	// 소멸자에서 정리할 오브젝트 목록
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// 씬 공장 함수 정의 — Director가 이전 씬을 delete하므로 반드시 new로 만든다.
	Scene* CreatePrimitive2DScene()
	{
		return new Primitive2DScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	Scene* CreatePrimitive3DScene()
	{
		return new Primitive3DScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 데모 앱: Application을 상속받아 생명주기 훅만 재정의한다. (cocos AppDelegate 스타일)
	class DemoApp : public Application
	{
	protected:
		// 엔진 준비 완료 직후: 첫 씬을 시작한다.
		bool ApplicationDidFinishLaunching() override
		{
			_LogInfo_("[32] DemoApp::ApplicationDidFinishLaunching — 첫 씬 시작");
			g_cDirector.RunScene(CreatePrimitive2DScene());
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
void PrimitiveMesh_Main()
{
	_LogInfo_("[32] 프리미티브 메시 튜토리얼 시작");

	DemoApp app;
	if (!app.Initialize(L"32. 프리미티브 메시 (SPACE: 씬 전환, ESC: 종료)", 800, 600))
	{
		jc::Console::WriteLine("엔진 초기화에 실패했습니다.");
		return;
	}

	app.SetClearColor(color(0x0A, 0x0A, 0x17));
	app.Run();
	app.Finalize();
}