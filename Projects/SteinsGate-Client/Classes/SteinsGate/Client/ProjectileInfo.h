/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:10:01 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Client/Struct.h>

struct AttackDataInfo;
struct EffectInfo;
struct ProjectileInfo : ConfigDataAbstract
{
	ProjectileInfo() = default;
	ProjectileInfo(const ProjectileInfo& other) = default;
	ProjectileInfo(ProjectileInfo&& other) noexcept;

	ProjectileInfo& operator=(const ProjectileInfo& other) = default;
	ProjectileInfo& operator=(ProjectileInfo&& other) noexcept;

	~ProjectileInfo() override;

	int ListenerCode;
	AttackDataInfo* AttackData;
	SGString Name;
	//int SgaIndex;
	//int ImgIndex;
	float SpawnOffsetX;
	float SpawnOffsetY;
	EffectInfo* SpawnEffect;
	float SpawnEffectOffsetX;
	float SpawnEffectOffsetY;
	EffectInfo* HitEffect;
	float Rotation;
	float RamdomRotationRangeMin;
	float RamdomRotationRangeMax;
	float Distance;
	float MoveSpeed;
	float LifeTime;
	float RehitDelay;
	bool IsSpriteDataRef;	// 오버라이딩 추가에따라 스프라이트 데이터가 원본인지 참조본인지 구분용도

	ThicknessBox ThicknessBox;
	ActorSpriteData* SpriteData;
};


