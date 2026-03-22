/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 4:39:09 PM
 * =====================
 *
 */


#pragma once

#include "jc/Primitives/String.h"

extern jc::String Logo_v;

extern const int LogoWidth_v;
extern const int LogoHeight_v;


void InitializeClientLogo(bool _print, int _leftPadding);
void InitializeServerCenterLogo(bool _print, int _leftPadding);
void InitializeServerAuthLogo(bool _print, int _leftPadding);
void InitializeServerGameLogo(bool _print, int _leftPadding);
void InitializeServerLobbyLogo(bool _print, int _leftPadding);
void InitializeServerQueryLogo(bool _print, int _leftPadding);
