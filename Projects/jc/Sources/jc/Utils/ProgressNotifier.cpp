/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 5:12:08 AM
 * =====================
 *
 */

#include "jc/Utils/ProgressNotifier.h"

NS_JC_BEGIN

//////////////////////////////////////////////////////////////////////////////////////////
ProgressNotifier::ProgressNotifier(int _maxStep)
	: m_pListener(nullptr)
	, m_iMaxStep(_maxStep)
	, m_iMaxProgressedStep(0)
	, m_bListenerOwner(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ProgressNotifier::~ProgressNotifier()
{
	if (m_bListenerOwner && m_pListener)
		JC_DELETE_SAFE(m_pListener);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProgressNotifier::SetListener(IProgressListener* _pListener, bool _transferOwnership)
{
	m_bListenerOwner = _transferOwnership;
	m_pListener = _pListener;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProgressNotifier::NotifyPrgressed(int _step)
{
	if (m_pListener)
		m_pListener->OnProgressed(_step, m_iMaxStep);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ProgressNotifier::NotifyFinished(int _step)
{
	if (m_pListener)
		m_pListener->OnFinished(_step, m_iMaxStep);
}

//////////////////////////////////////////////////////////////////////////////////////////
EachProgressNotifier::EachProgressNotifier(int _maxStep)
	: ProgressNotifier(_maxStep)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void EachProgressNotifier::Progress(int _step)
{
	NotifyPrgressed(_step);

	if (_step > m_iMaxProgressedStep)
	{
		m_iMaxProgressedStep = _step;
	}

	if (_step >= m_iMaxStep)
	{
		NotifyFinished(_step);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
PercentProgressNotifier::PercentProgressNotifier(int _maxStep, float _notificationStepPercent)
	: ProgressNotifier(_maxStep)
	, m_fNotificationStepPercent(_notificationStepPercent)
	, m_fNextNotificationStepQuantity(0.0f)
{
	if (_notificationStepPercent < 0.1f)
	{
		m_fNotificationStepPercent = 0.1f;
		jc_assert_msg(false, "알림 퍼센트가 0.1%보다 작으면 안됩니다.");
	}

	// Ex) 30개 -> 1% -> 0.3개마다 알림
	m_fNotificationStepQuantity = float(m_iMaxStep) * m_fNotificationStepPercent * 0.01f;
}

//////////////////////////////////////////////////////////////////////////////////////////
void PercentProgressNotifier::Progress(int _step)
{
	if (_step > m_iMaxProgressedStep)
	{
		m_iMaxProgressedStep = _step;
	}

	if (_step >= m_iMaxStep)
	{
		NotifyPrgressed(_step);
		NotifyFinished(_step);
		return;
	}

	if (float(_step) <= m_fNextNotificationStepQuantity)
	{
		return;
	}

	NotifyPrgressed(_step);
	m_fNextNotificationStepQuantity += m_fNotificationStepQuantity;
}

//////////////////////////////////////////////////////////////////////////////////////////
QuantityProgressNotifier::QuantityProgressNotifier(int _maxStep, int _notificationStepQuantity)
	: ProgressNotifier(_maxStep)
	, m_iNotificationStepQuantity(_notificationStepQuantity)
	, m_iNextNotificationStepQuantity(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
void QuantityProgressNotifier::Progress(int _step)
{
	if (_step > m_iMaxProgressedStep)
	{
		m_iMaxProgressedStep = _step;
	}

	if (_step >= m_iMaxStep)
	{
		NotifyPrgressed(_step);
		NotifyFinished(_step);
		return;
	}

	if (float(_step) <= m_iNotificationStepQuantity)
	{
		return;
	}

	NotifyPrgressed(_step);
	m_iNextNotificationStepQuantity += m_iNextNotificationStepQuantity;
}

NS_END
