#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class RoomLayer : public Layer
{
public:
	RoomLayer(){}
	bool init() override;
	CREATE_FUNC(RoomLayer);


	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

private:
};
