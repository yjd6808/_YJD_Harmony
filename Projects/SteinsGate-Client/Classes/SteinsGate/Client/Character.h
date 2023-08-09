/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:29:59 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Actor.h>
#include <SteinsGate/Client/CharInfo.h>

class Character : public Actor
{
public:
	Character();
	Character(int code);
	~Character() override;

	static Character* create(int code, const VisualInfo& info);

	void initActorId();
	void initInfo(int code, const VisualInfo& visualInfo);
	void initBaseInfo(int code);
	void initActorSpriteData(const VisualInfo& visualInfo);
	void initActorSprite() override;
	void initListener();
	void initComponents() override;

	CharBaseInfo* getBaseInfo();
	int getCode() override { return m_pBaseInfo->Code; }
protected:
	CharBaseInfo* m_pBaseInfo;
	ActorSpriteData* m_pSpriteData;
};


