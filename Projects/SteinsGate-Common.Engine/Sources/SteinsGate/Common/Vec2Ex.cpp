/*
 * 작성자: 윤정도
 * 생성일: 8/20/2023 3:31:28 PM
 * =====================
 *
 */

#include "Engine.h"
#include "Vec2Ex.h"

////////////////////////////////////////////////////////////////////////////////////////
void Vec2Ex::Log(const SGVec2& _vec)
{
	JCore::Console::WriteLine(":: 위치 [%d, %d]", int(_vec.x), int(_vec.y));
}
