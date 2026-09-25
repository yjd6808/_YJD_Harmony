/*
 * 작성자: 윤정도
 * 생성일: 9/24/2026
 * =====================
 * 03. 2D 씬 연습 (Practice) - 연습용 진입점
 *
 * 01번 2D 씬 템플릿을 복사한 독립 코드다.
 * 연습 내용은 Practice2DScene에 채워나간다.
 */

#include "Core.h"
#include "sgfr/Practice/03_2DScenePractice/03_2DScenePractice_Main.h"

using namespace sgf;
using namespace jc;

namespace
{
	////////////////////////////////////////////////////////////////////////////////////////
	// 2D 연습 씬: 화면 중앙 사각형 1개와 안내선 1개를 그린다.
	class Practice2DScene : public Scene2D
	{
	public:
		void OnEnter() override
		{
			Shape2D* pRect = new Shape2D(_T("Rect"));
			pRect->SetRect(rect(200.0f, 150.0f, 120.0f, 90.0f), color(0xFF, 0x6B, 0x6B));
			AddChild(pRect, 0);

			GetWindow()->SetTitle(_T("Practice 03. 2D 씬 연습 (방향키 이동, 휠 줌, ESC 종료)"));
			elapsed_ = 0.0f;
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
		}

	private:
		_f32 elapsed_ = 0.0f;
	};

	////////////////////////////////////////////////////////////////////////////////////////
	// 2D 연습 앱: 첫 씬 시작과 ESC 종료만 담당한다.
	class Practice2DApp : public Application
	{
	protected:
		bool ApplicationDidFinishLaunching() override
		{
			g_cDirector.RunScene(dbg_new Practice2DScene());
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
// 2D 씬 연습을 실행한다.
void Practice_2DScenePractice_Main()
{
	jc::Console::WriteLine(_T("[Practice 03] 2D 씬 연습 - 방향키 이동, 휠 줌, ESC 종료"));

	Practice2DApp app;
	if (!app.Initialize(_T("Practice 03. 2D 씬 연습"), 800, 600))
	{
		jc::Console::WriteLine(_T("엔진 초기화에 실패했습니다."));
		return;
	}

	app.SetClearColor(color(0x0A, 0x0A, 0x17, 0xFF));
	app.Run();
	app.Finalize();
}
