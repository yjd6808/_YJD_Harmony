/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

struct ChannelInfo;
class TextButton;

class ChannelScene : public BaseScene
{
public:
	ChannelScene();
	~ChannelScene() override;

	CREATE_FUNC(ChannelScene)

	bool init() override;
	void onEnterTransitionDidFinish() override;

	void refreshChannelList(ChannelInfo* channelList, int count);

	void onClickedChannelButton(TextButton* textButton);
	void onClickedLogoutButton(TextButton* textButton);

	Type getType() const override { return Type::Login; }

private:
	TextButton* m_pLogoutButton;
};