/*
 * 작성자: 윤정도
 * 생성일: 2/5/2023 9:51:36 AM
 * =====================
 *
 */


#pragma once

#include "Tutturu.h"
#include "SGStruct.h"

#include <SteinsGate/Client/SGSceneManager.h>
#include <SteinsGate/Client/SGSceneBase.h>

class SGWorldScene : public SGScene
{
private:
	SGWorldScene();
public:
	~SGWorldScene() override;

	bool init() override;
	void update(float dt) override;
	void updateScene(float dt);
	void onKeyPressed(SGEventKeyboard::KeyCode keyCode, SGEvent* event);
	void onKeyReleased(SGEventKeyboard::KeyCode keyCode, SGEvent* event);

	void reserveScene(SceneType_t sceneType);
	void changeScene(SceneType_t sceneType);

	static SGWorldScene* get();
private:
	SGSceneBase* m_pRunningScene;
	SceneType_t m_eReservedScene;
};

