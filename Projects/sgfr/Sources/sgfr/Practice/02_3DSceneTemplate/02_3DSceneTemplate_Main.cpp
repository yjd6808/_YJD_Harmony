/*
 * 작성자: 윤정도
 * 생성일: 9/24/2026
 * =====================
 * 02. 3D 씬 템플릿 (Practice) - 기반 코드
 *
 * Scene3D를 상속한 최소 씬과 Application 파생 실행기로 구성한다.
 * 이후 04번 연습은 이 템플릿을 복사해서 내용을 채운다.
 */

#include "Core.h"
#include "sgfr/Practice/02_3DSceneTemplate/02_3DSceneTemplate_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// 3D 템플릿 씬: 그리드와 축, 큐브 1개를 그린다.
	class Template3DScene : public Scene3D
	{
	public:
		void OnEnter() override
		{
			GetWindow()->SetTitle(_T("Practice 02. 3D 씬 템플릿 (방향키 공전, 휠 접근, ESC 종료)"));
			elapsed_ = 0.0f;

			Shape3D* pCube = dbg_new Shape3D(_T("Box"));
			pCube->SetShape(PrimitiveMesh3DType::Cube, color(0x4D, 0x8C, 0xFF, 0xFF));
			pCube->SetPosition(vec3(-1.5f, 1.0f, 0.0f));
			AddChild(pCube, 0);
			jc_assert(pCube->GetMaterial() != nullptr);

			Shape3D* pBall = dbg_new Shape3D(_T("Ball"));
			pBall->SetShape(PrimitiveMesh3DType::Sphere, color(0xFF, 0x66, 0x33, 0xFF));
			pBall->SetPosition(vec3(1.5f, 1.0f, 0.0f));
			pBall->SetScale(0.75f);
			AddChild(pBall, 1);
			jc_assert(pBall->GetMaterial() != nullptr);

			Shape3D* pPillar = dbg_new Shape3D(_T("Pillar"));
			pPillar->SetShape(PrimitiveMesh3DType::Cylinder, color(0x33, 0x99, 0x66, 0xFF));
			pPillar->SetPosition(vec3(0.0f, 1.0f, 2.0f));
			pPillar->SetScale(vec3(0.5f, 1.0f, 0.5f));
			AddChild(pPillar, 2);
			jc_assert(pPillar->GetMaterial() != nullptr);

			MaterialDesc signMaterial;
			signMaterial.vertexShaderKey_ = g_cResourceMgr.GetDefaultVertexShader3DKey();
			signMaterial.pixelShaderKey_ = g_cResourceMgr.GetDefaultPixelShader3DKey();
			signMaterial.baseColor_ = color(0xFF, 0xD9, 0x33, 0xFF);
			const _u64 signMaterialKey = g_cResourceMgr.CreateMaterial(signMaterial);

			GameObject* pSign = dbg_new GameObject(_T("Sign"));
			pSign->SetMeshKey(g_cResourceMgr.GetPrimitiveMesh3DKey(PrimitiveMesh3DType::Quad));
			pSign->SetMaterialKey(signMaterialKey);
			pSign->GetTransform()->SetLocalPosition(vec3(0.0f, 2.6f, 1.0f));
			pSign->GetTransform()->SetLocalScale(vec3(3.0f, 1.6f, 1.0f));
			jc_assert(pSign->GetMesh() != nullptr);
			AddChild(pSign, 3);

			MaterialDesc decalMaterial;
			decalMaterial.vertexShaderKey_ = g_cResourceMgr.GetDefaultVertexShader3DKey();
			decalMaterial.pixelShaderKey_ = g_cResourceMgr.GetDefaultPixelShader3DKey();
			decalMaterial.baseColor_ = color(0x33, 0x99, 0x99, 0xFF);
			const _u64 decalMaterialKey = g_cResourceMgr.CreateMaterial(decalMaterial);

			GameObject* pDecal = dbg_new GameObject(_T("Decal"));
			pDecal->SetMeshKey(g_cResourceMgr.GetPrimitiveMesh3DKey(PrimitiveMesh3DType::Quad));
			pDecal->SetMaterialKey(decalMaterialKey);
			pDecal->GetTransform()->SetLocalPosition(vec3(3.2f, 0.02f, 2.0f));
			pDecal->GetTransform()->SetLocalRotationEuler(-90.0f, 0.0f, 0.0f);
			pDecal->GetTransform()->SetLocalScale(vec3(2.0f, 2.0f, 1.0f));
			jc_assert(pDecal->GetMesh() != nullptr);
			AddChild(pDecal, 4);
		}

		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			elapsed_ += static_cast<_f32>(_dt.GetTotalSeconds());
			GetCamera3D()->DriveDefault3D(g_cInput, _dt);
		}

		void OnRender() override
		{
			g_cRenderer3D.DrawGrid(10, 1.0f, color(0x33, 0x33, 0x33, 0xFF));
			g_cRenderer3D.DrawAxis(5.0f);

			const _f32 offsetY = 1.0f + sinf(elapsed_ * 2.0f) * 0.25f;
			g_cRenderer3D.DrawCube(vec3(0.0f, offsetY, 0.0f), vec3(1.0f, 1.0f, 1.0f), color(0x4D, 0x8C, 0xFF, 0xFF));

			g_cRenderer3D.DrawTriangle(vec3(-1.0f, 2.5f, 0.0f), vec3(1.0f, 2.5f, 0.0f), vec3(-1.0f, 0.5f, 0.0f), color(0xFF, 0xD9, 0x33, 0xFF));
			g_cRenderer3D.DrawLine3D(vec3(0.0f, 0.0f, -2.0f), vec3(0.0f, 4.0f, -2.0f), color(0x66, 0x66, 0x66, 0xFF));
			g_cRenderer3D.DrawCube(vec3(0.0f, 0.5f, -2.0f), vec3(0.5f, 0.5f, 0.5f), color(0x99, 0x33, 0x66, 0xFF));

			g_cRenderer3D.DrawCube(vec3(-3.0f, 1.2f, 1.0f), vec3(2.4f, 1.2f, 0.05f), color(0x66, 0x33, 0x99, 0xFF));
			g_cRenderer3D.DrawLine3D(vec3(-4.2f, 0.6f, 1.0f), vec3(-1.8f, 0.6f, 1.0f), color(0xFF, 0xFF, 0xFF, 0xFF));
			g_cRenderer3D.DrawLine3D(vec3(-1.8f, 0.6f, 1.0f), vec3(-1.8f, 1.8f, 1.0f), color(0xFF, 0xFF, 0xFF, 0xFF));
			g_cRenderer3D.DrawLine3D(vec3(-1.8f, 1.8f, 1.0f), vec3(-4.2f, 1.8f, 1.0f), color(0xFF, 0xFF, 0xFF, 0xFF));
			g_cRenderer3D.DrawLine3D(vec3(-4.2f, 1.8f, 1.0f), vec3(-4.2f, 0.6f, 1.0f), color(0xFF, 0xFF, 0xFF, 0xFF));
		}

	private:
		_f32 elapsed_ = 0.0f;
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 3D 템플릿 앱: 첫 씬 시작과 ESC 종료만 담당한다.
	class Template3DApp : public Application
	{
	protected:
		bool ApplicationDidFinishLaunching() override
		{
			g_cDirector.RunScene(dbg_new Template3DScene());
			return true;
		}

		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			(void)_dt;
			if (g_cInput.IsKeyPressed(VK_ESCAPE))
			{
				g_cWindow.Destroy();
			}
		}
	};
}

////////////////////////////////////////////////////////////////////////////////////////
// 3D 씬 템플릿을 실행한다.
void Practice_3DSceneTemplate_Main()
{
	jc::Console::WriteLine(_T("[Practice 02] 3D 씬 템플릿 - 방향키 공전, 휠 접근, ESC 종료"));

	Template3DApp app;
	if (!app.Initialize(_T("Practice 02. 3D 씬 템플릿"), 800, 600))
	{
		jc::Console::WriteLine(_T("엔진 초기화에 실패했습니다."));
		return;
	}

	app.SetClearColor(color(0x0A, 0x0A, 0x17, 0xFF));
	app.Run();
	app.Finalize();
}
