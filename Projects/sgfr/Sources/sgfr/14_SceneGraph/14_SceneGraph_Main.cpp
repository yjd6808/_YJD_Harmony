/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:20:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (v2: Node/Sprite 제거, Application 훅 + SoundEngine + g_c 전역 접근)
 * =====================
 * 14. 씬 & 사운드 - 진입점 구현부
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
#include "sgfr/14_SceneGraph/14_SceneGraph_Main.h"
#include "sgfr/14_SceneGraph/14_SceneGraph_Function.h"

using namespace sgf;
using namespace jc;

namespace
{
	// 씬 공장 함수 선언: 두 씬이 서로를 교체하므로 앞서 선언만 해둔다.
	Scene* CreateSolarSystemScene();
	Scene* CreateBouncingBallScene();

	constexpr _f32 kViewWidth_v = 800.0f;	// 가상 화면 가로
	constexpr _f32 kViewHeight_v = 600.0f;	// 가상 화면 세로

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 1] 태양계 - 공전 궤도를 도는 태양/지구/달
	// Node/Sprite 계층 없이 각도만 누적해서 위치를 직접 계산해 그린다.
	class SolarSystemScene : public Scene
	{
	public:
		// 씬이 무대에 오를 때 1회: 텍스처 준비 + 카메라 설정
		void OnEnter() override
		{
			if (!CreateCircleTexture(&g_cDevice, &m_SunTexture, 128, Color(1.0f, 0.85f, 0.2f, 1.0f)) ||
				!CreateCircleTexture(&g_cDevice, &m_EarthTexture, 64, Color(0.3f, 0.55f, 1.0f, 1.0f)) ||
				!CreateCircleTexture(&g_cDevice, &m_MoonTexture, 32, Color(0.8f, 0.8f, 0.8f, 1.0f)))
			{
				printf("원형 텍스처 생성 실패!\n");
				return;
			}

			GetCamera().SetOrthographic2D(kViewWidth_v, kViewHeight_v);
			m_Elapsed = 0.0f;

			// [v2.1] 씬은 자신이 그려지는 창을 안다. (Director가 연결해준다)
			GetWindow()->SetTitle(L"14. 태양계 씬 - SPACE: 씬 교체 / M: 서브 윈도우 (ESC로 종료)");
		}

		// 매 프레임: 경과 시간 누적 + 입력 처리 (v2: jc::TimeSpan)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			m_Elapsed += static_cast<_f32>(_dt.GetTotalSeconds());

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
			const Vec2 sunPos(kViewWidth_v * 0.5f, kViewHeight_v * 0.5f);

			// 지구/달의 공전 각도 (각속도 x 누적 시간)
			const _f32 earthAngle = m_Elapsed * 0.6f;
			const _f32 moonAngle = m_Elapsed * 2.4f;
			const _f32 earthOrbit = 180.0f;
			const _f32 moonOrbit = 55.0f;

			const Vec2 earthPos(
				sunPos.x + cosf(earthAngle) * earthOrbit,
				sunPos.y + sinf(earthAngle) * earthOrbit);
			const Vec2 moonPos(
				earthPos.x + cosf(moonAngle) * moonOrbit,
				earthPos.y + sinf(moonAngle) * moonOrbit);

			// 궤도선: 짧은 선분 48개로 원을 그린다.
			DrawOrbit(sunPos, earthOrbit, Color(0.3f, 0.3f, 0.4f, 1.0f));
			DrawOrbit(earthPos, moonOrbit, Color(0.3f, 0.3f, 0.4f, 1.0f));

			// 천체 그리기 (그리는 순서대로 위에 줤인다)
			g_cRenderer2D.DrawSprite(&m_SunTexture, sunPos, Vec2(120.0f, 120.0f));
			g_cRenderer2D.DrawSprite(&m_EarthTexture, earthPos, Vec2(56.0f, 56.0f));
			g_cRenderer2D.DrawSprite(&m_MoonTexture, moonPos, Vec2(26.0f, 26.0f));
		}

		// 씬이 무대에서 내려갈 때 1회: 리소스 정리는 Texture 소멸자가 처리한다.
		void OnExit() override
		{
		}

	private:
		// 중심과 반지름으로 궤도 원을 그린다. (48개 선분 근사)
		void DrawOrbit(const Vec2& _center, _f32 _radius, const Color& _color)
		{
			constexpr _s32 kSegments_v = 48;
			for (_s32 i = 0; i < kSegments_v; ++i)
			{
				const _f32 a0 = jc_math_pi2 * i / kSegments_v;
				const _f32 a1 = jc_math_pi2 * (i + 1) / kSegments_v;
				g_cRenderer2D.DrawLine(
					Vec2(_center.x + cosf(a0) * _radius, _center.y + sinf(a0) * _radius),
					Vec2(_center.x + cosf(a1) * _radius, _center.y + sinf(a1) * _radius),
					_color);
			}
		}

	private:
		Texture m_SunTexture;		// 태양 텍스처
		Texture m_EarthTexture;		// 지구 텍스처
		Texture m_MoonTexture;		// 달 텍스처
		_f32 m_Elapsed = 0.0f;		// 누적 시간 (공전 각도 계산용)
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// [씬 2] 통통 튀는 공 - 벽에 부딪힐 때마다 효과음이 난다.
	// 씬 교체 전/후 비교: SPACE로 태양계 씬과 오가며 확인한다.
	class BouncingBallScene : public Scene
	{
	public:
		static constexpr _s32 kBallCount_v = 8;	// 공 개수

		// 씬 진입: 공 텍스처 + 초기 위치/속도 배치
		void OnEnter() override
		{
			GetCamera().SetOrthographic2D(kViewWidth_v, kViewHeight_v);

			// [v2.1] 이 씬이 서브 윈도우에 올라가면 그 창의 제목이 바뀝다.
			GetWindow()->SetTitle(L"14. 통통 튀는 공 씬 - SPACE: 씬 교체 (ESC로 종료)");

			// 공마다 색상을 달리해 텍스처를 만든다.
			for (_s32 i = 0; i < kBallCount_v; ++i)
			{
				const _f32 t = static_cast<_f32>(i) / kBallCount_v;
				const Color color(0.4f + 0.6f * t, 0.9f - 0.6f * t, 0.5f + 0.4f * sinf(t * jc_math_pi2), 1.0f);
				if (!CreateCircleTexture(&g_cDevice, &m_Textures[i], 64, color))
				{
					printf("공 텍스처 생성 실패!\n");
					return;
				}

				// 위치는 가로로 늘어놓고, 속도는 공마다 조금씩 다르게 준다.
				m_PositionsX[i] = 100.0f + 75.0f * i;
				m_PositionsY[i] = 150.0f + 40.0f * (i % 4);
				m_VelocitiesX[i] = 120.0f + 30.0f * i;
				m_VelocitiesY[i] = 160.0f + 25.0f * ((i * 3) % 5);
				m_Radii[i] = 14.0f + 3.0f * (i % 4);
			}
		}

		// 매 프레임: 공 이동 + 벽 충돌 반사 + 효과음 (v2: jc::TimeSpan)
		void OnUpdate(const jc::TimeSpan& _dt) override
		{
			const _f32 dt = static_cast<_f32>(_dt.GetTotalSeconds());

			for (_s32 i = 0; i < kBallCount_v; ++i)
			{
				m_PositionsX[i] += m_VelocitiesX[i] * dt;
				m_PositionsY[i] += m_VelocitiesY[i] * dt;

				bool bBounced = false;

				// 좌우 벽 충돌: 위치를 벽 안으로 되돌리고 속도 부호를 뒤집는다.
				if (m_PositionsX[i] < m_Radii[i])
				{
					m_PositionsX[i] = m_Radii[i];
					m_VelocitiesX[i] = -m_VelocitiesX[i];
					bBounced = true;
				}
				else if (m_PositionsX[i] > kViewWidth_v - m_Radii[i])
				{
					m_PositionsX[i] = kViewWidth_v - m_Radii[i];
					m_VelocitiesX[i] = -m_VelocitiesX[i];
					bBounced = true;
				}

				// 상하 벽 충돌
				if (m_PositionsY[i] < m_Radii[i])
				{
					m_PositionsY[i] = m_Radii[i];
					m_VelocitiesY[i] = -m_VelocitiesY[i];
					bBounced = true;
				}
				else if (m_PositionsY[i] > kViewHeight_v - m_Radii[i])
				{
					m_PositionsY[i] = kViewHeight_v - m_Radii[i];
					m_VelocitiesY[i] = -m_VelocitiesY[i];
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
			const Color borderColor(0.5f, 0.5f, 0.6f, 1.0f);
			g_cRenderer2D.DrawLine(Vec2(0.0f, 0.0f), Vec2(kViewWidth_v, 0.0f), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(Vec2(kViewWidth_v, 0.0f), Vec2(kViewWidth_v, kViewHeight_v), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(Vec2(kViewWidth_v, kViewHeight_v), Vec2(0.0f, kViewHeight_v), borderColor, 3.0f);
			g_cRenderer2D.DrawLine(Vec2(0.0f, kViewHeight_v), Vec2(0.0f, 0.0f), borderColor, 3.0f);

			for (_s32 i = 0; i < kBallCount_v; ++i)
			{
				const Vec2 pos(m_PositionsX[i], m_PositionsY[i]);
				const _f32 diameter = m_Radii[i] * 2.0f;
				g_cRenderer2D.DrawSprite(&m_Textures[i], pos, Vec2(diameter, diameter));
			}
		}

	private:
		Texture m_Textures[kBallCount_v];		// 공 텍스처
		_f32 m_PositionsX[kBallCount_v] = {};	// 공 X 위치
		_f32 m_PositionsY[kBallCount_v] = {};	// 공 Y 위치
		_f32 m_VelocitiesX[kBallCount_v] = {};	// 공 X 속도
		_f32 m_VelocitiesY[kBallCount_v] = {};	// 공 Y 속도
		_f32 m_Radii[kBallCount_v] = {};		// 공 반지름
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	// 씬 공장 함수 정의
	// Director가 이전 씬을 delete하므로 반드시 new로 만든다.
	Scene* CreateSolarSystemScene()
	{
		return new SolarSystemScene();
	}

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
	};
}

//////////////////////////////////////////////////////////////////////////////////////////
// 튜토리얼 진입점
void SceneGraph_Main()
{
	PrintSceneGraphExplanation();

	DemoApp app;
	if (!app.Initialize(L"14. 씬 & 사운드 - SPACE로 씬 교체 (ESC로 종료)", 800, 600))
	{
		jc::Console::WriteLine("엔진 초기화에 실패했습니다.");
		return;
	}

	app.SetClearColor(Color(0.04f, 0.04f, 0.09f, 1.0f));
	app.Run();
	app.Finalize();
}
