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

	void Finalize();

	void AddNetGroup(int id, const NetGroupPtr& group);
	NetGroupPtr GetNetGroup(int id);
protected:
	JCore::HashMap<int, NetGroupPtr> m_hNetGroup;
	// JCore::Vector<JCore::Tuple<int, NetGroupPtr>> m_hNetGroup;
	bool m_bFinalized;
};

NS_JNET_END