﻿/*
 * 작성자: 윤정도
 * 생성일: 12/23/2023 9:19:50 PM
 * =====================
 *
 */


#pragma once


NS_BUFFERING_WITH_NAGLE_BEGIN

struct ClientSide
{
	static void Initialize(bool nagle);
	static void StartTest();
	static void Finalize();
};

NS_BUFFERING_WITH_NAGLE_END