/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 12:10:01 PM
 * =====================
 *
 */

#pragma once

struct AttackDataInfo;
struct EffectInfo;

struct ProjectileInfo : SDescBase
{
	ProjectileInfo() = default;
	ProjectileInfo(const ProjectileInfo& _other) = default;
	ProjectileInfo(ProjectileInfo&& _other) noexcept;

	ProjectileInfo& operator=(const ProjectileInfo& _other) = default;
	ProjectileInfo& operator=(ProjectileInfo&& _other) noexcept;

	~ProjectileInfo() override;

	int listenerCode_;
	AttackDataInfo* attackData_;
	jc::String name_;
	//int SgaIndex;
	//int ImgIndex;
	float spawnOffsetX_;
	float spawnOffsetY_;
	EffectInfo* spawnEffect_;
	float spawnEffectOffsetX_;
	float spawnEffectOffsetY_;
	EffectInfo* hitEffect_;
	float rotation_;
	float ramdomRotationRangeMin_;
	float ramdomRotationRangeMax_;
	float distance_;
	float moveSpeed_;
	float lifeTime_;
	float rehitDelay_;
	bool isSpriteDataRef_; // 오버라이딩 추가에따라 스프라이트 데이터가 원본인지 참조본인지 구분용도

	ThicknessBox thicknessBox_;
	ActorSpriteData* spriteData_;
};
