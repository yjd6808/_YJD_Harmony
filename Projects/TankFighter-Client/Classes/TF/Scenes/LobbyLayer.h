#pragma once

#include <TF/SourceHeader.h>
#include <TF/UI/TextButton.h>

class LobbyLayer : public Layer
{
public:
	bool init() override;
	void OnChannelClick(TextButton* textButton);
	CREATE_FUNC(LobbyLayer);

	void CmdLoadChannelInfoAck(ICommand* cmd);
private:
};


