/*
 * 작성자: 윤정도
 * 생성일: 8/27/2023 7:21:29 PM
 * =====================
 *
 */


#pragma once

#include <jc/Primitives/Range.h>
#include <sg/Namepsace.h>

NS_CONST_BEGIN
namespace Timeout
{
namespace Authentication
{
constexpr int LobbyWait = 15;
constexpr int Lobby = 900;
constexpr int GameWait = 15;
constexpr int Game = 1800;
};
}


NS_CONST_END
