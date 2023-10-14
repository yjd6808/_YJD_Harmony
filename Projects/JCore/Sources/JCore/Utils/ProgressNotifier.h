/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 4:49:31 AM
 * =====================
 *
 */


#pragma once

#include <JCore/Utils/IProgressListener.h>
#include <JCore/Functional.h>

NS_JC_BEGIN

class ProgressNotifier
{
public:
	ProgressNotifier(int maxStep);
	virtual ~ProgressNotifier();

	virtual void Progress(int step) = 0;

	// transferOwnership: 인자로 전달한 리스너의 소유권을 Notifier로 이전할지 여부 (이전시 소멸자에서 해당 리스너를 자동 삭제처리함)
	void SetListener(IProgressListener* listener, bool transferOwnership = false);
protected:
	void NotifyPrgressed(int step);
	void NotifyFinished(int step);

	IProgressListener* m_pListener;
	int m_iMaxStep;
	int m_iMaxProgressedStep;
	bool m_bListenerOwner;
};

// 각 스탭마다 알림
class EachProgressNotifier : public ProgressNotifier
{
public:
	EachProgressNotifier(int maxStep);
	void Progress(int step) override;
};

// 몇퍼센트 진행률마다 알림줄지
class PercentProgressNotifier : public ProgressNotifier
{
public:
	PercentProgressNotifier(int maxStep, float notificationStepPercent);
	void Progress(int step) override;
private:
	float m_fNotificationStepPercent;
	float m_fNotificationStepQuantity;
	float m_fNextNotificationStepQuantity;	// 알람을 줄 다음 스탭 갯수
};

// 몇번 작업마다 알림줄지
class QuantityProgressNotifier : public ProgressNotifier
{
public:
	QuantityProgressNotifier(int maxStep, int notificationStepQuantity);
	void Progress(int step) override;
private:
	int m_iNotificationStepQuantity;
	int m_iNextNotificationStepQuantity;	// 알람을 줄 다음 스탭 갯수
};


NS_JC_END