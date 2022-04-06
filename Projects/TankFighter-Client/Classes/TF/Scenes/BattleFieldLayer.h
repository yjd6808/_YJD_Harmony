#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class BattleFieldLayer : public Layer
{
public:
	BattleFieldLayer(){}
	bool init() override;
	CREATE_FUNC(BattleFieldLayer);


	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

private:
};
