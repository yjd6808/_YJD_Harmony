/*
 * 작성자: 윤정도
 * 생성일: 8/7/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

struct MapPhysicsInfo : ConfigDataAbstract
{
    float Gravity;
    float Friction;
    float ElasticityDividedForce;
};



