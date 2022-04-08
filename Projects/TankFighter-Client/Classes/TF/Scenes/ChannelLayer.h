/*
 * �ۼ��� : ������
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
	 *                             ��� ��Ŷ ó��
	 * ================================================================================*/

	void OnChannelClick(TextButton* textButton);
	void CmdLoadChannelInfoAck(ICommand* cmd);
	void CmdSelectChannelAck(ICommand* cmd);
private:
};


