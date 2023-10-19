/*
 * 작성자: 윤정도
 * 생성일: 10/17/2023 12:33:54 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Declspec.h>

struct IOpenCloseable
{
	virtual ~IOpenCloseable() = default;
	virtual void Open() = 0;
	virtual void Close() = 0;
	virtual bool IsClosed() = 0;
	virtual bool IsOpened() = 0;
};