/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:32:55 PM
 * =====================
 * 
 */


#pragma once

#include "jnet/NetGroup.h"

NS_JNET_BEGIN

class JC_NOVTABLE NetGroupMgr
{
public:
	NetGroupMgr();
	virtual ~NetGroupMgr();

	virtual bool Initialize() = 0;
	virtual void Finalize();

	void SetName(const jc::String& _name);
	void SetValidator(const jc::Predicate<NetGroupPtr>& _validator) { validator_ = _validator; }
	void SetValidator(jc::Predicate<NetGroupPtr>&& _validator) { validator_ = std::move(_validator); }

	void AddNetGroup(int _groupId, const NetGroupPtr& _pGroupPtr);
	NetGroupPtr GetNetGroup(int _id);

	static constexpr const char* TypeName() { return "네트마스터"; }

protected:
	jc::HashMap<int, NetGroupPtr> netGroupMap_;
	jc::Predicate<NetGroupPtr> validator_;

	bool finalized_;
	jc::String name_;
};

NS_END
