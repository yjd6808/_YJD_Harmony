/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/SGConfig.h>

struct SGAnimationInfo;
struct SGAttackDataInfo;

class SGActor;
class SGPhysicsActor;


struct SGThicknessBox
{
	float RelativeX;
	float RelativeY;
	float Width;
	float Height;

	SGThicknessBox() : RelativeX(0), RelativeY(0), Width(0), Height(0) {}
	SGThicknessBox(float relaitveX, float relativeY, float width, float height)
		: RelativeX(relaitveX)
		, RelativeY(relativeY)
		, Width(width)
		, Height(height) {}

	SGThicknessBox(const SGVec2& pt, const SGSize& size)
		: RelativeX(pt.x)
		, RelativeY(pt.y)
		, Width(size.width)
		, Height(size.height)
	{}
	SGThicknessBox(const SGRect& r)
		: RelativeX(r.origin.x)
		, RelativeY(r.origin.y)
		, Width(r.size.width)
		, Height(r.size.height)
	{}

	SGSize getSize() const { return SGSize{ Width, Height }; }
};


struct SGActorRect
{
	SGRect ThicknessRect;
	SGRect BodyRect;

	bool isCollide(const SGActorRect& rect, SpriteDirection_t& otherHitDirection, SGRect& hitRect);
};


struct SGVisualInfo
{
	int NpkIndex[VisualType::Max];
	int ImgIndex[VisualType::Max];

	SGVisualInfo() {
		JCore::Arrays::Fill(NpkIndex, InvalidValue_v);
		JCore::Arrays::Fill(ImgIndex, InvalidValue_v);
	}
	SGVisualInfo(const SGVisualInfo& other) {
		JCore::Memory::CopyUnsafe(NpkIndex, other.NpkIndex, sizeof(int) * VisualType::Max);
		JCore::Memory::CopyUnsafe(ImgIndex, other.ImgIndex, sizeof(int) * VisualType::Max);
	}
};

struct SGCharacterInfo
{
	SGVisualInfo VisualInfo;
};

struct SGActorPartSpriteData
{
	int ZOrder;
	int NpkIndex;
	int ImgIndex;
};



struct SGActorSpriteData
{
	SGActorSpriteData(int partCount, int animationCount)
		: Parts(partCount)
		, Animations(animationCount) {}

	SGVector<SGActorPartSpriteData> Parts;
	SGVector<SGAnimationInfo*> Animations;
};

using SGActorSpriteDataPtr = JCore::SharedPtr<SGActorSpriteData>;

struct SGHitInfo
{
	SGActor* Attacker;
	SGPhysicsActor* HitTarget;
	SpriteDirection_t HitDirection;
	SGRect HitRect;
	SGAttackDataInfo* AttackDataInfo;
};