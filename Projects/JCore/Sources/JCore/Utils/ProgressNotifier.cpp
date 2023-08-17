/*
 * 작성자: 윤정도
 * 생성일: 8/18/2023 5:12:08 AM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Utils/ProgressNotifier.h>

NS_JC_BEGIN

ProgressNotifier::ProgressNotifier(int maxStep)
	: m_pListener(nullptr)
	, m_iMaxStep(maxStep)
	, m_iMaxProgressedStep(0)
	, m_bListenerOwner(false)
{}

ProgressNotifier::~ProgressNotifier() {
	if (m_bListenerOwner && m_pListener)
		JCORE_DELETE_SAFE(m_pListener);
}

void ProgressNotifier::SetListener(IProgressListener* listener, bool transferOwnership) {
	m_bListenerOwner = transferOwnership;
	m_pListener = listener;
}

void ProgressNotifier::NotifyPrgressed(int step) {
	if (m_pListener)
		m_pListener->OnProgressed(step, m_iMaxStep);
}

void ProgressNotifier::NotifyFinished(int step) {
	if (m_pListener)
		m_pListener->OnFinished(step, m_iMaxStep);
}

EachProgressNotifier::EachProgressNotifier(int maxStep)
	: ProgressNotifier(maxStep)
{} 

void EachProgressNotifier::Progress(int step) {
	NotifyPrgressed(step);

	if (step > m_iMaxProgressedStep) {
		m_iMaxProgressedStep = step;
	}

	if (step >= m_iMaxStep) {
		NotifyFinished(step);
	}
}

PercentProgressNotifier::PercentProgressNotifier(int maxStep, float notificationStepPercent)
	: ProgressNotifier(maxStep)
	, m_fNotificationStepPercent(notificationStepPercent)
	, m_fNextNotificationStepQuantity(0.0f)
{

	if (notificationStepPercent < 0.1f) {
		m_fNotificationStepPercent = 0.1f;
		DebugAssertMsg(false, "알림 퍼센트가 0.1%보다 작으면 안됩니다.");
	}

	// Ex) 30개 -> 1% -> 0.3개마다 알림
	m_fNotificationStepQuantity = float(m_iMaxStep) * m_fNotificationStepPercent * 0.01f;
}

void PercentProgressNotifier::Progress(int step) {
	if (step > m_iMaxProgressedStep) {
		m_iMaxProgressedStep = step;
	}

	if (step >= m_iMaxStep) {
		NotifyPrgressed(step);
		NotifyFinished(step);
		return;
	}

	if (float(step) <= m_fNextNotificationStepQuantity) {
		return;
	}

	NotifyPrgressed(step);
	m_fNextNotificationStepQuantity += m_fNotificationStepQuantity;
}

QuantityProgressNotifier::QuantityProgressNotifier(int maxStep, int notificationStepQuantity)
	: ProgressNotifier(maxStep)
	, m_iNotificationStepQuantity(notificationStepQuantity)
	, m_iNextNotificationStepQuantity(0)
{}

void QuantityProgressNotifier::Progress(int step) {
	if (step > m_iMaxProgressedStep) {
		m_iMaxProgressedStep = step;
	}

	if (step >= m_iMaxStep) {
		NotifyPrgressed(step);
		NotifyFinished(step);
		return;
	}

	if (float(step) <= m_iNotificationStepQuantity) {
		return;
	}

	NotifyPrgressed(step);
	m_iNextNotificationStepQuantity += m_iNextNotificationStepQuantity;
}

NS_JC_END
