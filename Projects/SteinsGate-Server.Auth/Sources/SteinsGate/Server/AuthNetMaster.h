/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 3:51:46 PM
 * =====================
 *
 */


#pragma once

#include <JNetwork/NetMaster.h>

class AuthNetMaster : public JNetwork::NetMaster
{
private:
	AuthNetMaster();
public:
	void Initialize() override;
	void MainLoop();

	static AuthNetMaster* Get() {
		static AuthNetMaster* pInst;
		if (pInst == nullptr) {
			pInst = dbg_new AuthNetMaster();
		}
		return pInst;
	}

	
};



