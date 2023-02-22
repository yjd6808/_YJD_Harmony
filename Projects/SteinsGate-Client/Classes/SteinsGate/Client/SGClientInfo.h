/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:54 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

class SGClientInfo : public ConfigDataAbstract
{
public:
	~SGClientInfo() override = default;
	float GameScale;
	float ResolutionWidth;
	float ResolutionHeight;
	bool FullScreen;
};