/*
 * 작성자: 윤정도
 * 생성일: 8/7/2023 11:32:58 AM
 * =====================
 *
 */


#pragma once

#include <sg/DescBase.h>

struct MapPhysicsInfo : SDescBase
{
	float gravity_;
	float friction_;
	float elasticityDividedForce_;
};
