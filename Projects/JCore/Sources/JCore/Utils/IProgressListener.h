/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 4:53:00 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Namespace.h>
#include <JCore/Functional.h>

NS_JC_BEGIN

struct IProgressListener
{
	virtual ~IProgressListener() = default;
	virtual void OnProgressed(int cur, int max) = 0;
	virtual void OnFinished(int cur, int max) = 0;
};

struct CallbackProgressListener : IProgressListener
{
	void OnFinished(int cur, int max) override { if (FinishedCallback) FinishedCallback(cur, max); }
	void OnProgressed(int cur, int max) override { if (ProgressedCallback) ProgressedCallback(cur, max); }

	Action<int, int> ProgressedCallback;
	Action<int, int> FinishedCallback;
};

NS_JC_END