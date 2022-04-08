/*
 * 작성자 : 윤정도
 */

#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class ChannelLayer : public Layer
{
public:
	bool init() override;
	void onEnterTransitionDidFinish() override;
	CREATE_FUNC(ChannelLayer);


	/* =================================================================================
	 *                             통신 패킷 처리
	 * ================================================================================*/

	void OnChannelClick(TextButton* textButton);
	void CmdLoadChannelInfoAck(ICommand* cmd);
	void CmdSelectChannelAck(ICommand* cmd);
private:
};


