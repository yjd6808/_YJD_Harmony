#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class BattleFieldLayer : public Layer
{
public:
	BattleFieldLayer(){}
	bool init() override;
	void onEnterTransitionDidFinish() override;
	CREATE_FUNC(BattleFieldLayer);


	/* =================================================================================
	 *                             ��� ��Ŷ ó��
	 * ================================================================================*/

private:
};
