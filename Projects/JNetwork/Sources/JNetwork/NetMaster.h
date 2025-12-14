/*
 * 작성자: 윤정도
 * 생성일: 2/9/2023 1:32:55 PM
 * =====================
 * 
 */


#pragma once

#include <JNetwork/NetGroup.h>

NS_JNET_BEGIN

class JCORE_NOVTABLE NetMaster
{
public:
	NetMaster();
	virtual ~NetMaster();

	virtual void Initialize() = 0;
	virtual void Finalize();

	void SetName(const JCore::String& _name);

	void AddNetGroup(int _groupId, const NetGroupPtr& _pGroupPtr);
	NetGroupPtr GetNetGroup(int _id);

	static constexpr const char* TypeName() { return "네트마스터"; }

protected:
	JCore::HashMap<int, NetGroupPtr> netGroupMap_;
	bool finalized_;
	JCore::String name_;
};

NS_JNET_END
