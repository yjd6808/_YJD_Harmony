/*
 * 작성자: 윤정도
 * 생성일: 10/13/2023 2:30:51 PM
 * =====================
 *
 */


#pragma once

#include <TF/Client/Game/Scene/BaseScene.h>

class RoomScene : public BaseScene
{
public:
	RoomScene();
	~RoomScene() override;

	CREATE_FUNC(RoomScene)

	bool init() override;
	void onEnterTransitionDidFinish() override;

	Type getType() const override { return Room; }
private:
};