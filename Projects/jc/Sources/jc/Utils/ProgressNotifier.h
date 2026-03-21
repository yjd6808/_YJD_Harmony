/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 4:49:31 AM
 * =====================
 *
 */
 
 
#pragma once

#include "jc/Utils/IProgressListener.h"
#include "jc/Functional.h"

NS_JC_BEGIN

class ProgressNotifier
{
public:
	ProgressNotifier(int _maxStep);
	virtual ~ProgressNotifier();

	virtual void Progress(int _step) = 0;

	// transferOwnership: 인자로 전달한 리스너의 소유권을 Notifier로 이전할지 여부 (이전시 소멸자에서 해당 리스너를 자동 삭제처리함)
	void SetListener(IProgressListener* _pListener, bool _transferOwnership = false);
protected:
	void NotifyPrgressed(int _step);
	void NotifyFinished(int _step);

	IProgressListener* m_pListener;
	int m_iMaxStep;
	int m_iMaxProgressedStep;
	bool m_bListenerOwner;
};

// 각 스탭마다 알림
class EachProgressNotifier : public ProgressNotifier
{
public:
	EachProgressNotifier(int _maxStep);
	void Progress(int _step) override;
};

// 몇퍼센트 진행률마다 알림줄지
class PercentProgressNotifier : public ProgressNotifier
{
public:
	PercentProgressNotifier(int _maxStep, float _notificationStepPercent);
	void Progress(int _step) override;
private:
	float m_fNotificationStepPercent;
	float m_fNotificationStepQuantity;
	float m_fNextNotificationStepQuantity;	// 알람을 줄 다음 스탭 갯수
};

// 몇번 작업마다 알림줄지
class QuantityProgressNotifier : public ProgressNotifier
{
public:
	QuantityProgressNotifier(int _maxStep, int _notificationStepQuantity);
	void Progress(int _step) override;
private:
	int m_iNotificationStepQuantity;
	int m_iNextNotificationStepQuantity;	// 알람을 줄 다음 스탭 갯수
};


NS_END
