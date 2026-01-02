/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */

#pragma once
#include <jnet_gbench/Namespace.h>

NS_BUFFERING_WITH_NAGLE_BEGIN

struct ClientSide
{
	static void Initialize(bool _nagle);
	static void StartTest();
	static void Finalize();
};

NS_BUFFERING_WITH_NAGLE_END