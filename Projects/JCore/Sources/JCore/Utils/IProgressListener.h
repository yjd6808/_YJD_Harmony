/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 4:53:00 AM
 * =====================
 *
 *
 */


#pragma once

#include <JCore/Namespace.h>
#include <JCore/Functional.h>

NS_JC_BEGIN

struct IProgressListener
{
	virtual ~IProgressListener() = default;
	virtual void OnProgressed(int _cur, int _max) = 0;
	virtual void OnFinished(int _cur, int _max) = 0;
};

struct CallbackProgressListener : IProgressListener
{
	void OnFinished(int _cur, int _max) override
	{
		if (FinishedCallback) FinishedCallback(_cur, _max);
	}

	void OnProgressed(int _cur, int _max) override
	{
		if (ProgressedCallback) ProgressedCallback(_cur, _max);
	}

	Action<int, int> ProgressedCallback;
	Action<int, int> FinishedCallback;
};

NS_JC_END
