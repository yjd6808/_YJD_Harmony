/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:20:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: Node/Sprite 제거, Application 훅 + SoundEngine + g_c 전역 접근)
 * =====================
 * 29. 씬 & 사운드 - 진입점 구현부
 *
 * [이 튜토리얼에서 배우는 것]
 *  1. Application 파생 + ApplicationDidFinishLaunching에서 첫 씬 시작 (cocos 스타일)
 *  2. Scene 파생: OnEnter/OnUpdate(jc::TimeSpan)/OnRender/OnExit 생명주기
 *  3. g_cDirector.ReplaceScene으로 씬 교체 (SPACE 키)
 *  4. g_cSound.PlayTone으로 효과음 재생 (발소리 없는 PC에서도 동작)
 *  5. 카메라 DriveDefault2D: 방향키 이동 + 휠 줌을 한 줄로 처리
 *  6. [v2.1] GetWindow()로 씬이 그려지는 창 접근 + M 키로 서브 윈도우 생성
 *
 * [비교 체험 포인트]
 *  SPACE로 "태양계 씬"과 "통통 튀는 공 씬"을 오가며 씬 교체 전/후를 눈으로 확인한다.
 */

#include "Core.h"
#include "sgfr/Tutorial/29_SceneGraph/29_SceneGraph_Main.h"
#include "sgfr/Tutorial/29_SceneGraph/29_SceneGraph_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	//////////////////////////////////////////////////////////////////////////////////////////

	// [DEBUG-DIAG] 원인 분석용 임시 로그 (수정 후 제거 예정)
	FILE* OpenDiagLog()
	{
		FILE* f = nullptr;
		fopen_s(&f, "C:\\Users\\jdyun\\AppData\\Local\\Temp\\opencode\\sgfr29_diag.log", "a");
		return f;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	void LogDiag(const char* _szFormat, ...)
	{
		FILE* f = OpenDiagLog();
		if (f == nullptr) { return; }
		va_list args;
		va_start(args, _szFormat);
		vfprintf(f, _szFormat, args);
		va_end(args);
		fprintf(f, "\n");
		fflush(f);
		fclose(f);
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	void LogDiagMat(const char* _szName, const mat4& _m)
	{
		LogDiag("%s = [%f %f %f %f | %f %f %f %f | %f %f %f %f | %f %f %f %f]",
			_szName,
			_m.m[0][0], _m.m[0][1], _m.m[0][2], _m.m[0][3],
			_m.m[1][0], _m.m[1][1], _m.m[1][2], _m.m[1][3],
			_m.m[2][0], _m.m[2][1], _m.m[2][2], _m.m[2][3],
			_m.m[3][0], _m.m[3][1], _m.m[3][2], _m.m[3][3]);
	}

	// 씬 공장 함수 선언: 두 씬이 서로를 교체하므로 앞서 선언만 해둔다.
	Scene* CreateSolarSystemScene();
	Scene* CreateBouncingBallScene();

	constexpr _f32 VIEW_WIDTH = 800.0f;	// 가상 화면 가로
	constexpr _f32 VIEW_HEIGHT = 600.0f;	// 가상 화면 세로

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 1] 태양계 - 공전 궤도를 도는 태양/지구/달
	// Node/Sprite 계층 없이 각도만 누적해서 위치를 직접 계산해 그린다.
	class SolarSystemScene : public Scene
	{
	public:
		// 씬이 무대에 오를 때 1회: 텍스처 준비 + 카메라 설정
		void OnEnter() override
		{
			LogDiag("[OnEnter] SolarSystemScene 시작 (캠 800x600 설정 전)");
			if (!CreateCircleTexture(&g_cDevice, &sunTexture_, 128, color(1.0f, 0.85f, 0.2f, 1.0f)) ||
				!CreateCircleTexture(&g_cDevice, &earthTexture_, 64, color(0.3f, 0.55f, 1.0f, 1.0f)) ||
				!CreateCircleTexture(&g_cDevice, &moonTexture_, 32, color(0.8f, 0.8f, 0.8f, 1.0f)))
			{
				jc::Console::WriteLine("원형 텍스처 생성 실패!");
				LogDiag("[OnEnter] 원형 텍스처 생성 실패!");
				return;
			}
			LogDiag("[OnEnter] 텍스처 생성 성공 - Sun valid=%d Earth valid=%d Moon valid=%d",
				(_s32)sunTexture_.IsValid(), (_s32)earthTexture_.IsValid(), (_s32)moonTexture_.IsValid());

			GetCamera().SetOrthographic2D(VIEW_WIDTH, VIEW_HEIGHT);
			elapsed_ = 0.0f;
			LogDiagMat("[OnEnter] view", GetCamera().View());
			LogDiagMat("[OnEnter] proj", GetCamera().Projection());
			LogDiagMat("[OnEnter] vp", GetCamera().ViewProjection());
			LogDiag("[OnEnter] window=%p title-설정 전 hwnd", (void*)GetWindow());

			// [v2.1] 씬은 자신이 그려지는 창을 안다. (Director가 연결해준다)
			GetWindow()->SetTitle(L"29. 태양계 씬 - SPACE: 씬 교체 / M: 서브 윈도우 (ESC로 종료)");
		}

		// 매 프레임: 경과 시간 누적 + 입력 처리 (v2: jc::TimeSpan)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			elapsed_ += static_cast<_f32>(_dt.GetTotalSeconds());

			// 방향키 이동 + 휠 줌: 카메라 표준 조작 한 줄 (v2 편의 API)
			GetCamera().DriveDefault2D(g_cInput, _dt);

			// SPACE: 효과음과 함께 다음 씬으로 교체
			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
				g_cSound.PlayTone(660, 120, 0.5f);
				g_cDirector.ReplaceScene(CreateBouncingBallScene());
			}

			// M: 서브 윈도우를 열고 그 창에 "통통 튀는 공" 씬을 띄운다. (v2.1 멀티 윈도우)
			// 메인 창과 서브 창이 각자 다른 씬을 동시에 그리는 것을 눈으로 확인하자.
			if (g_cInput.IsKeyPressed('M'))
			{
				Window* pSub = g_cApp.CreateSubWindow(L"서브 윈도우", 480, 360);
				if (pSub != nullptr)
				{
					g_cDirector.RunScene(CreateBouncingBallScene(), pSub);
				}
			}
		}

		// 매 프레임 그리기: 궤도선 -> 태양 -> 지구 -> 달 순서로 직접 그린다.
		void OnRender() override
		{
			if (diagFrames_ < 30)
			{
				LogDiag("[OnRender] frame=%d elapsed=%f sunValid=%d", diagFrames_, elapsed_, (_s32)sunTexture_.IsValid());
				LogDiagMat("[OnRender] vp", GetCamera().ViewProjection());
				++diagFrames_;
			}

			const vec2 sunPos(VIEW_WIDTH * 0.5f, VIEW_HEIGHT * 0.5f);

			// 지구/달의 공전 각도 (각속도 x 누적 시간)
			const _f32 earthAngle = elapsed_ * 0.6f;
			const _f32 moonAngle = elapsed_ * 2.4f;
			const _f32 earthOrbit = 180.0f;
			const _f32 moonOrbit = 55.0f;

			const vec2 earthPos(
				sunPos.x + cosf(earthAngle) * earthOrbit,
				sunPos.y + sinf(earthAngle) * earthOrbit);
			const vec2 moonPos(
				earthPos.x + cosf(moonAngle) * moonOrbit,
				earthPos.y + sinf(moonAngle) * moonOrbit);

			// 궤도선: 짧은 선분 48개로 원을 그린다.
			DrawOrbit(sunPos, earthOrbit, color(0.3f, 0.3f, 0.4f, 1.0f));
			DrawOrbit(earthPos, moonOrbit, color(0.3f, 0.3f, 0.4f, 1.0f));

			// 천체 그리기 (그리는 순서대로 위에 줤인다)
			g_cRenderer2D.DrawSprite(&sunTexture_, sunPos, vec2(120.0f, 120.0f));
			g_cRenderer2D.DrawSprite(&earthTexture_, earthPos, vec2(56.0f, 56.0f));
			g_cRenderer2D.DrawSprite(&moonTexture_, moonPos, vec2(26.0f, 26.0f));
		}

		// 씬이 무대에서 내려갈 때 1회: 리소스 정리는 Texture 소멸자가 처리한다.
		void OnExit() override
		{
		}

	private:
		// 중심과 반지름으로 궤도 원을 그린다. (48개 선분 근사)
		void DrawOrbit(const vec2& _center, _f32 _radius, const color& _color)
		{
			constexpr _s32 SEGMENTS = 48;
			for (_s32 i = 0; i < SEGMENTS; ++i)
			{
				const _f32 a0 = jc_math_pi2 * i / SEGMENTS;
				const _f32 a1 = jc_math_pi2 * (i + 1) / SEGMENTS;
				g_cRenderer2D.DrawLine(
					vec2(_center.x + cosf(a0) * _radius, _center.y + sinf(a0) * _radius),
					vec2(_center.x + cosf(a1) * _radius, _center.y + sinf(a1) * _radius),
					_color);
			}
		}

	private:
		Texture sunTexture_;		// 태양 텍스처
		Texture earthTexture_;		// 지구 텍스처
		Texture moonTexture_;		// 달 텍스처
		_f32 elapsed_ = 0.0f;		// 누적 시간 (공전 각도 계산용)
		_s32 diagFrames_ = 0;		// [DEBUG-DIAG]
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 2] 통통 튀는 공 - 벽에 부딪힐 때마다 효과음이 난다.
	// 씬 교체 전/후 비교: SPACE로 태양계 씬과 오가며 확인한다.
	class BouncingBallScene : public Scene
	{
	public:
		static constexpr _s32 BALL_COUNT = 8;	// 공 개수

		// 씬 진입: 공 텍스처 + 초기 위치/속도 배치
		void OnEnter() override
		{
			GetCamera().SetOrthographic2D(VIEW_WIDTH, VIEW_HEIGHT);

			// [v2.1] 이 씬이 서브 윈도우에 올라가면 그 창의 제목이 바뀝다.
			GetWindow()->SetTitle(L"29. 통통 튀는 공 씬 - SPACE: 씬 교체 (ESC로 종료)");

			// 공마다 색상을 달리해 텍스처를 만든다.
			for (_s32 i = 0; i < BALL_COUNT; ++i)
			{
				const _f32 t = static_cast<_f32>(i) / BALL_COUNT;
				const color ballColor(0.4f + 0.6f * t, 0.9f - 0.6f * t, 0.5f + 0.4f * sinf(t * jc_math_pi2), 1.0f);
				if (!CreateCircleTexture(&g_cDevice, &textures_[i], 64, ballColor))
				{
				jc::Console::WriteLine("공 텍스처 생성 실패!");
					return;
				}

				// 위치는 가로로 늘어놓고, 속도는 공마다 조금씩 다르게 준다.
				positionsX_[i] = 100.0f + 75.0f * i;
				positionsY_[i] = 150.0f + 40.0f * (i % 4);
				velocitiesX_[i] = 120.0f + 30.0f * i;
				velocitiesY_[i] = 160.0f + 25.0f * ((i * 3) % 5);
				radii_[i] = 14.0f + 3.0f * (i % 4);
			}
		}

		// 매 프레임: 공 이동 + 벽 충돌 반사 + 효과음 (v2: jc::TimeSpan)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			const _f32 dt = static_cast<_f32>(_dt.GetTotalSeconds());

			for (_s32 i = 0; i < BALL_COUNT; ++i)
			{
				positionsX_[i] += velocitiesX_[i] * dt;
				positionsY_[i] += velocitiesY_[i] * dt;

				bool bBounced = false;

				// 좌우 벽 충돌: 위치를 벽 안으로 되돌리고 속도 부호를 뒤집는다.
				if (positionsX_[i] < radii_[i])
				{
					positionsX_[i] = radii_[i];
					velocitiesX_[i] = -velocitiesX_[i];
					bBounced = true;
				}
				else if (positionsX_[i] > VIEW_WIDTH - radii_[i])
				{
					positionsX_[i] = VIEW_WIDTH - radii_[i];
					velocitiesX_[i] = -velocitiesX_[i];
					bBounced = true;
				}

				// 상하 벽 충돌
				if (positionsY_[i] < radii_[i])
				{
					positionsY_[i] = radii_[i];
					velocitiesY_[i] = -velocitiesY_[i];
					bBounced = true;
				}
				else if (positionsY_[i] > VIEW_HEIGHT - radii_[i])
				{
					positionsY_[i] = VIEW_HEIGHT - radii_[i];
					velocitiesY_[i] = -velocitiesY_[i];
					bBounced = true;
				}

				// 반사 순간 효과음: 공마다 음을 다르게 해 누가 부딪혔는지 귀로도 구분된다.
				if (bBounced)
				{
					g_cSound.PlayTone(440 + i * 40, 60, 0.35f);
				}
			}

			// 방향키/휠: 카메라 조작 (줌해서 공 하나를 따라가보자)
			GetCamera().DriveDefault2D(g_cInput, _dt);

			// SPACE: 태양계 씬으로 복귀
			if (g_cInput.IsKeyPressed(VK_SPACE))
			{
				g_cSound.PlayTone(520, 120, 0.5f);
				g_cDirector.ReplaceScene(CreateSolarSystemScene());
			}
		}

		// 매 프레임 그리기: 테두리 + 공들
		void OnRender() override
		{
			// 화면 테두리 (공이 튀는 범위를 눈으로 확인)
			const color borderColor(0.5f, 0.5f, 0.6f, 1.0f);
			g_cRenderer2D.DrawLine(vec2(0.0f, 0.0f), vec2(VIEW_WIDTH, 0.0f), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(vec2(VIEW_WIDTH, 0.0f), vec2(VIEW_WIDTH, VIEW_HEIGHT), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(vec2(VIEW_WIDTH, VIEW_HEIGHT), vec2(0.0f, VIEW_HEIGHT), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(vec2(0.0f, VIEW_HEIGHT), vec2(0.0f, 0.0f), borderColor, 3.0f);

			for (_s32 i = 0; i < BALL_COUNT; ++i)
			{
				const vec2 pos(positionsX_[i], positionsY_[i]);
				const _f32 diameter = radii_[i] * 2.0f;
				g_cRenderer2D.DrawSprite(&textures_[i], pos, vec2(diameter, diameter));
			}
		}

	private:
		Texture textures_[BALL_COUNT];		// 공 텍스처
		_f32 positionsX_[BALL_COUNT] = {};	// 공 X 위치
		_f32 positionsY_[BALL_COUNT] = {};	// 공 Y 위치
		_f32 velocitiesX_[BALL_COUNT] = {};	// 공 X 속도
		_f32 velocitiesY_[BALL_COUNT] = {};	// 공 Y 속도
		_f32 radii_[BALL_COUNT] = {};		// 공 반지름
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// 씬 공장 함수 정의
	// Director가 이전 씬을 delete하므로 반드시 new로 만든다.
	Scene* CreateSolarSystemScene()
	{
		return new SolarSystemScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	Scene* CreateBouncingBallScene()
	{
		return new BouncingBallScene();
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	// 데모 앱: Application을 상속받아 생명주기 훅만 재정의한다. (cocos AppDelegate 스타일)
	class DemoApp : public Application
	{
	protected:
		// 엔진 준비 완료 직후: 첫 씬을 시작한다.
		bool ApplicationDidFinishLaunching() override
		{
			g_cDirector.RunScene(CreateSolarSystemScene());
			return true;
		}

		// 매 프레임: ESC 종료 처리
		// (InputManager가 연결된 창이라 Window의 ESC 자동 닫기는 동작하지 않는다.
		//  종료 판정은 InputManager가 처리하고, 창 파괴는 Destroy로 유도한다)
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
void SceneGraph_Main()
{
	PrintSceneGraphExplanation();

	DemoApp app;
	if (!app.Initialize(L"29. 씬 & 사운드 - SPACE로 씬 교체 (ESC로 종료)", 800, 600))
	{
		jc::Console::WriteLine("엔진 초기화에 실패했습니다.");
		return;
	}

	app.SetClearColor(color(0.04f, 0.04f, 0.09f, 1.0f));
	app.Run();
	app.Finalize();
}
