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
	Character(CharBaseInfo* _pCharInfo, const VisualInfo& _visualInfo);
	~Character() override;

	static Character* create(CharBaseInfo* _pCharInfo, const VisualInfo& _visualInfo);

	void initialize() override;
	void initActorSpriteData(const VisualInfo& _visualInfo);
	void initActorSprite() override;
	void initListeners() override;
	void initComponents() override;

	CharBaseInfo* getBaseInfo();
	int getCode() override { return m_pBaseInfo->code_; }
	ActorType_t getType() const override { return ActorType::Character; }

protected:
	CharBaseInfo* m_pBaseInfo;
	VisualInfo m_VisualInfo;
	ActorSpriteData* m_pSpriteData;
};
