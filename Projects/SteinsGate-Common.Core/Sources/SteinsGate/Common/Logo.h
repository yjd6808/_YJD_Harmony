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


void InitializeClientLogo(bool print, int leftPadding);
void InitializeServerCenterLogo(bool print, int leftPadding);
void InitializeServerAuthLogo(bool print, int leftPadding);
void InitializeServerGameLogo(bool print, int leftPadding);
void InitializeServerLobbyLogo(bool print, int leftPadding);
