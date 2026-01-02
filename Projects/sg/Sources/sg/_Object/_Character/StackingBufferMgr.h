/*
 * 작성자: 윤정도
 * 생성일: 12/1/2025 12:26:58 PM
 * =====================
 *
 */


#pragma once

#include "StackingBuffer.h"

class CStackingBufferMgr
{
public:
	void Add(Int32U _id, Int32U _type, float _deltaPerSec, Int32U _duration, bool _isPercentStacking = false,
	         float _percentBase = 0.f);
	bool Remove(Int32U _id);

	bool IsExist(Int32U _id) const;
	bool IsDirty() const;

	CStackingBuffer& Get(Int32U _id);
	const CStackingBuffer& Get(Int32U _id) const;

	void ResetDirtyAll();

private:
	jc::HashMap<Int32U, CStackingBuffer> map_;
};
