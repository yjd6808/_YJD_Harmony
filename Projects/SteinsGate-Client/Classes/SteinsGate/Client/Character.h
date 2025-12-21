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

	static Character* Create(CharBaseInfo* _pCharInfo, const VisualInfo& _visualInfo);

	void Initialize() override;
	void InitActorSpriteData(const VisualInfo& _visualInfo);
	void InitActorSprite() override;
	void InitListeners() override;
	void InitComponents() override;

	CharBaseInfo* GetBaseInfo();
	int GetCode() override { return m_pBaseInfo->code_; }
	ActorType_t GetType() const override { return ActorType::Character; }

protected:
	CharBaseInfo* m_pBaseInfo;
	VisualInfo m_VisualInfo;
	ActorSpriteData* m_pSpriteData;
};
