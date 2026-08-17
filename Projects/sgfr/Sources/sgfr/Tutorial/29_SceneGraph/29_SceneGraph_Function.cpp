/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:20:00 AM
 * 수정일: 8/9/2026 1:00:00 AM (jc::Console 사용, 씬 & 사운드 설명으로 갱신)
 * =====================
 * 29. 씬 & 사운드 - 보조 함수 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/29_SceneGraph/29_SceneGraph_Function.h"

using namespace sgf;
using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////

// 씬/디렉터/사운드 구조와 Cocos2d-x와의 차이를 콘솔에 출력한다. (학습용)
void PrintSceneGraphExplanation()
{
	jc::Console::WriteLine("");
	jc::Console::WriteLine("[씬(Scene)과 디렉터(Director)]");
	jc::Console::WriteLine(" 씬 = 게임의 \"화면 하나\" (타이틀/인게임/결과창처럼 통째로 교체되는 단위)");
	jc::Console::WriteLine(" 디렉터 = 현재 씬을 들고 있다가 매 프레임 Update/Render를 전달하고,");
	jc::Console::WriteLine("           ReplaceScene 요청이 오면 프레임 경계에서 안전하게 교체한다.");
	jc::Console::WriteLine("");
	jc::Console::WriteLine("[v2에서 바뀐 점 - Node/Sprite 제거]");
	jc::Console::WriteLine(" 부모-자식(AddChild) 계층 없이 Scene::OnRender에서");
	jc::Console::WriteLine(" Renderer2D::DrawSprite/DrawRect/DrawLine을 직접 불러 즉시 그린다.");
	jc::Console::WriteLine(" 그리는 순서 = 코드 순서라 흐름을 따라가기 쉽다.");
	jc::Console::WriteLine("");
	jc::Console::WriteLine("[Cocos2d-x에서 가져온 것]");
	jc::Console::WriteLine(" Application 생명주기 훅(ApplicationDidFinishLaunching 등),");
	jc::Console::WriteLine(" SoundEngine(AudioEngine 대응: Play2d/PlayTone/PauseAll/ResumeAll),");
	jc::Console::WriteLine(" g_c 전역 접근(g_cDirector/g_cSound/g_cInput - sgcl 스타일).");
	jc::Console::WriteLine("");
	jc::Console::WriteLine("[조작법]");
	jc::Console::WriteLine(" SPACE: 씬 교체 (효과음과 함께) / 방향키: 카메라 이동 / 휠: 줌 / ESC: 종료");
	jc::Console::WriteLine("");
}

//////////////////////////////////////////////////////////////////////////////////////////

// 단색 원 텍스처를 CPU로 직접 만든다. (경계 부드러움 포함)
// 행성/공 스프라이트용 텍스처를 외부 파일 없이 준비하기 위한 함수다.
bool CreateCircleTexture(GraphicDevice* _pDevice, Texture* _pOutTexture, _s32 _size, const color& _color)
{
	const _f32 center = _size * 0.5f;
	const _f32 radius = _size * 0.5f - 2.0f;	// 가장자리 2픽셀 여유

	jc::Vector<_u8> pixels;
	pixels.Resize(_size * _size * 4);

	for (_s32 y = 0; y < _size; ++y)
	{
		for (_s32 x = 0; x < _size; ++x)
		{
			// 원 중심으로부터의 거리로 안/밖 판정
			const _f32 dx = (x + 0.5f) - center;
			const _f32 dy = (y + 0.5f) - center;
			const _f32 dist = sqrtf(dx * dx + dy * dy);

			// 경계 1.5픽셀 구간을 부드럽게 처리 (안티에일리어싱)
			const _f32 alpha = Clamp((radius - dist) / 1.5f, 0.0f, 1.0f);

			const _s32 idx = (y * _size + x) * 4;
			pixels[idx + 0] = (_u8)(_color.Rf() * 255.0f * alpha);
			pixels[idx + 1] = (_u8)(_color.Gf() * 255.0f * alpha);
			pixels[idx + 2] = (_u8)(_color.Bf() * 255.0f * alpha);
			pixels[idx + 3] = (_u8)(255.0f * alpha);
		}
	}

	return _pOutTexture->CreateFromMemory(_pDevice, pixels.Source(), _size, _size);
}
