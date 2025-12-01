/*
 * 작성자: 윤정도
 * 생성일: 12/1/2025 12:26:35 PM
 * =====================
 *
 */


#pragma once

class CStackingBuffer
{
public:
	CStackingBuffer(
		Int32 _id,
		float _deltaPerSec,
		Int32U _duration,
		Int32U _type,
		bool _isPercentStacking = false,
		float _percentBase = 0.f);

	CStackingBuffer(const CStackingBuffer&);
	CStackingBuffer& operator=(const CStackingBuffer&);

	float	CalcDelta();

	JCORE_GETTER(Int32U, Id, id_)
	JCORE_GETTER(Int32U, Type, type_)
	JCORE_GETTER(float, DeltaPerSec, deltaPerSec_)
	JCORE_GETTER(Int32U, Duration, duration_)
	JCORE_GETTER(bool, IsPercentStacking, isPercentStacking_)
private:
	Int32U	id_;					// 아이디
	Int32U	type_;					// 타입
	float	deltaPerSec_;			// 초당 변화률
	Int32U	duration_;				// 적용 시간
	float	percentBase_;			// 퍼센트변화률일때 사용되는 베이스 수치

	bool	isPercentStacking_;		// %(퍼센트) 변화률인가를 마크
	bool	isDirty_;				// 변경사항이 있는지 여부
	Int32U	lastCalcTime_;			// 마지막으로 계산한 시간
};