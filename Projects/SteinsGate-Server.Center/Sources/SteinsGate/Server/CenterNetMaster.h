/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetMaster.h>

class CenterNetMaster : public JNetwork::NetMaster
{
private:
	CenterNetMaster();
public:
	void Initialize() override;
	void MainLoop();

	static CenterNetMaster* Get() {
		static CenterNetMaster* pInst;
		if (pInst == nullptr) {
			pInst = dbg_new CenterNetMaster();
		}
		return pInst;
	}
};



