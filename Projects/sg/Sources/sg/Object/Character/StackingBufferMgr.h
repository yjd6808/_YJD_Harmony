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
	void Add(_u32 _id, _u32 _type, float _deltaPerSec, _u32 _duration, bool _isPercentStacking = false,
	         float _percentBase = 0.f);
	bool Remove(_u32 _id);

	bool IsExist(_u32 _id) const;
	bool IsDirty() const;

	CStackingBuffer& Get(_u32 _id);
	const CStackingBuffer& Get(_u32 _id) const;

	void ResetDirtyAll();

private:
	jc::HashMap<_u32, CStackingBuffer> map_;
};
