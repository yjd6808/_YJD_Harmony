/*
 * 작성자: 윤정도
 * 생성일: 9/24/2026
 * =====================
 * 01. 2D 씬 템플릿 (Practice) - 기반 코드
 *
 * Scene2D를 상속한 최소 씬과 Application 파생 실행기로 구성한다.
 * 이후 03번 연습은 이 템플릿을 복사해서 내용을 채운다.
 */

#include "Core.h"
#include "sgfr/Practice/01_2DSceneTemplate/01_2DSceneTemplate_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// 2D 템플릿 씬: 화면 중앙 사각형 1개와 안내선 1개를 그린다.
	class Template2DScene : public Scene2D
	{
	public:
		void OnEnter() override
		{
			GetCamera2D()->SetOrthographic2D(800.0f, 600.0f);
			GetWindow()->SetTitle(_T("Practice 01. 2D 씬 템플릿 (방향키 이동, 휠 줌, ESC 종료)"));
			elapsed_ = 0.0f;

			Shape2D* pPanel = dbg_new Shape2D(_T("Panel"));
			pPanel->SetRect(rect(60.0f, 400.0f, 200.0f, 140.0f), color(0x2A, 0x4D, 0x8F, 0xFF));
			AddChild(pPanel, 0);

			Shape2D* pCircle = dbg_new Shape2D(_T("Circle"));
			pCircle->SetCircle(vec2(640.0f, 460.0f), 60.0f, color(0x33, 0x99, 0x66, 0xFF));
			AddChild(pCircle, 1);

			Shape2D* pLine = dbg_new Shape2D(_T("Guide"));
			pLine->SetLine(vec2(60.0f, 120.0f), vec2(740.0f, 120.0f), 4.0f, color(0x66, 0x66, 0x66, 0xFF));
			AddChild(pLine, 2);

			Shape2D* pTri = dbg_new Shape2D(_T("Tri"));
			pTri->SetTriangle(vec2(640.0f, 200.0f), vec2(580.0f, 110.0f), vec2(700.0f, 110.0f), color(0x99, 0x33, 0x66, 0xFF));
			AddChild(pTri, 3);
		}

		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			elapsed_ += static_cast<_f32>(_dt.GetTotalSeconds());
			GetCamera2D()->DriveDefault2D(g_cInput, _dt);
		}

		void OnRender() override
		{
			const vec2 center(400.0f, 300.0f);
			g_cRenderer2D.DrawRect(center, vec2(160.0f, 160.0f), color(0x4D, 0x8C, 0xFF, 0xFF));

			const _f32 offsetX = cosf(elapsed_) * 120.0f;
			g_cRenderer2D.DrawRect(vec2(center.x + offsetX, center.y), vec2(48.0f, 48.0f), color(0xFF, 0xD9, 0x33, 0xFF));
			g_cRenderer2D.DrawLine(vec2(40.0f, 40.0f), vec2(760.0f, 40.0f), color(0x66, 0x66, 0x66, 0xFF));

			g_cRenderer2D.DrawCircle(vec2(center.x - offsetX, 300.0f), 24.0f, color(0xFF, 0x66, 0x33, 0xFF));
			g_cRenderer2D.DrawRect(vec2(400.0f, 560.0f), vec2(220.0f, 20.0f), color(0x33, 0x33, 0x33, 0xFF), elapsed_ * 0.5f);
		}

	private:
		_f32 elapsed_ = 0.0f;
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 2D 템플릿 앱: 첫 씬 시작과 ESC 종료만 담당한다.
	class Template2DApp : public Application
	{
	protected:
		bool ApplicationDidFinishLaunching() override
		{
			g_cDirector.RunScene(dbg_new Template2DScene());
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
// 2D 씬 템플릿을 실행한다.
void Practice_2DSceneTemplate_Main()
{
	jc::Console::WriteLine(_T("[Practice 01] 2D 씬 템플릿 - 방향키 이동, 휠 줌, ESC 종료"));

	Template2DApp app;
	if (!app.Initialize(_T("Practice 01. 2D 씬 템플릿"), 800, 600))
	{
		jc::Console::WriteLine(_T("엔진 초기화에 실패했습니다."));
		return;
	}

	app.SetClearColor(color(0x0A, 0x0A, 0x17, 0xFF));
	app.Run();
	app.Finalize();
}
