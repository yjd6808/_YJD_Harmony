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
		_s32 _id,
		float _deltaPerSec,
		_u32 _duration,
		_u32 _type,
		bool _isPercentStacking = false,
		float _percentBase = 0.f);

	CStackingBuffer(const CStackingBuffer&);
	CStackingBuffer& operator=(const CStackingBuffer&);

	float CalcDelta();

	JC_GETTER(_u32, Id, id_)
	JC_GETTER(_u32, Type, type_)
	JC_GETTER(float, DeltaPerSec, deltaPerSec_)
	JC_GETTER(_u32, Duration, duration_)
	JC_GETTER(bool, IsPercentStacking, isPercentStacking_)

private:
	_u32 id_; // 아이디
	_u32 type_; // 타입
	float deltaPerSec_; // 초당 변화률
	_u32 duration_; // 적용 시간
	float percentBase_; // 퍼센트변화률일때 사용되는 베이스 수치

	bool isPercentStacking_; // %(퍼센트) 변화률인가를 마크
	bool isDirty_; // 변경사항이 있는지 여부
	_u32 lastCalcTime_; // 마지막으로 계산한 시간
};
