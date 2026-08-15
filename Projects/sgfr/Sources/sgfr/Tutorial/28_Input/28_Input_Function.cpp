/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 11:10:00 AM
 * =====================
 * 28. 입력 처리 - 보조 함수/셰이더 소스 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/28_Input/28_Input_Function.h"

using namespace sgf;
using namespace jc;

//////////////////////////////////////////////////////////////////////////////////////////

// 색상 정점을 그대로 통과시키는 HLSL 셰이더 소스를 반환한다. (05번과 동일 구조)
const char* InputDemoShaderSource()
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

//////////////////////////////////////////////////////////////////////////////////////////

// 폴링과 이벤트 방식의 차이를 콘솔에 출력한다. (학습용)
void PrintInputModelExplanation()
{
	jc::Console::WriteLine("\n[입력을 읽는 두 가지 방법]");
	jc::Console::WriteLine(" 1. 폴링(Polling): 매 프레임 '지금 눌린 상태인가?'를 직접 묻는다.");
	jc::Console::WriteLine("    - IsKeyDown / IsKeyPressed / MouseX / MouseY");
	jc::Console::WriteLine("    - 이동처럼 매 프레임 연속적으로 반응할 때 적합");
	jc::Console::WriteLine(" 2. 이벤트(Event): '눌리는 순간' 등록해둔 함수가 호출된다.");
	jc::Console::WriteLine("    - onKeyPressed / onMousePressed / onMouseWheel 등 (jc::Event 기반)");
	jc::Console::WriteLine("    - 버튼 클릭, 메뉴 열기처럼 '발생 순간' 1번만 반응할 때 적합");
	jc::Console::WriteLine(" Cocos2d-x의 EventListener가 2번 방식, 유니티의 Input.GetKey가 1번 방식이다.");
	jc::Console::WriteLine(" sgf는 둘 다 지원하므로 상황에 맞게 골라 쓰면 된다!\n");
}

//////////////////////////////////////////////////////////////////////////////////////////

// 픽셀 좌표(왼쪽위 기준)를 NDC 좌표(-1~+1, y 위쪽 +)로 변환한다.
// 마우스 좌표를 화면에 그리려면 반드시 필요한 변환이다!
//
// [왜 변환이 필요한가?]
//  마우스: (0,0)이 왼쪽 위, y가 아래로 갈수록 커진다. (픽셀 단위)
//  NDC   : (0,0)이 화면 중앙, y가 위로 갈수록 커진다. (-1 ~ +1)
//  서로 원점과 y축 방향이 다르므로 공식으로 맞춰줘야 한다.
vec2 PixelToNdc(_s32 _pixelX, _s32 _pixelY, _s32 _screenWidth, _s32 _screenHeight)
{
	// x: 0~width 를 0~2로 늘리고 1을 빼면 -1~+1
	const _f32 ndcX = (_pixelX / (_f32)_screenWidth) * 2.0f - 1.0f;

	// y: 방향이 반대라서 부호를 뒤집는다
	const _f32 ndcY = 1.0f - (_pixelY / (_f32)_screenHeight) * 2.0f;

	return vec2(ndcX, ndcY);
}
