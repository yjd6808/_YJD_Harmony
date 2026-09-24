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
