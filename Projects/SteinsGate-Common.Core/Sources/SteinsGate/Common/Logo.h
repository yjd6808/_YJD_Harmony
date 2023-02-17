/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 4:39:09 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Primitives/String.h>

extern JCore::String Logo_v;

extern const int LogoWidth_v;
extern const int LogoHeight_v;


void InitializeClientLogo();
void InitializeServerCenterLogo();
void InitializeServerAuthLogo();
void InitializeServerGameLogo();
void InitializeServerLobbyLogo();
