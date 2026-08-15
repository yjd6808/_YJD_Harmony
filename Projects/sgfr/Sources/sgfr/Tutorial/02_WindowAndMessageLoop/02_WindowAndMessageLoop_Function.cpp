/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:20:00 AM
 * =====================
 * 02. 윈도우와 메시지 루프 - 보조 함수 구현부
 */

#include "Core.h"
#include "sgfr/Tutorial/02_WindowAndMessageLoop/02_WindowAndMessageLoop_Function.h"

// 가상 키코드(VK_*)를 사람이 읽기 쉬운 이름으로 바꿔 반환한다.
// [가상 키코드란?]
//  키보드 종류와 상관없이 OS가 약속한 키 번호다.
//  숫자/알파벳 키는 ASCII 코드와 값이 같다. ('A' == 0x41 == VK_A)
const char* VkCodeToName(_s32 _vkCode)
{
	switch (_vkCode)
	{
	case VK_ESCAPE:  return "ESC";
	case VK_SPACE:   return "SPACE";
	case VK_RETURN:  return "ENTER";
	case VK_LEFT:    return "LEFT";
	case VK_RIGHT:   return "RIGHT";
	case VK_UP:      return "UP";
	case VK_DOWN:    return "DOWN";
	case VK_SHIFT:   return "SHIFT";
	case VK_CONTROL: return "CTRL";
	default:
		break;
	}

	// 알파벳/숫자는 문자 그대로 보여준다. (정적 버퍼 사용: 단순 로그용)
	static _s8 s_szName[2] = { 0, 0 };
	if ((_vkCode >= '0' && _vkCode <= '9') || (_vkCode >= 'A' && _vkCode <= 'Z'))
	{
		s_szName[0] = (_s8)_vkCode;
		return s_szName;
	}
	return "(기타)";
}
