/*
 * 작성자: 윤정도
 * 생성일: 2/21/2023 3:29:13 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

class CommonInfo : public ConfigDataAbstract
{
public:
	SGString ConfigPath;
	SGString DataPath;
};