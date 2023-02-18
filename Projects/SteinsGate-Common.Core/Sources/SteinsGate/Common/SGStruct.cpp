/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 4:15:22 PM
 * =====================
 *
 */

#include "Core.h"
#include "SGStruct.h"

#include <SteinsGate/Common/SGConfig.h>

USING_NS_JC;

VisualInfo::VisualInfo() {
	JCore::Arrays::Fill(NpkIndex, InvalidValue_v);
	JCore::Arrays::Fill(ImgIndex, InvalidValue_v);
}

VisualInfo::VisualInfo(const VisualInfo& other) {
	JCore::Memory::CopyUnsafe(NpkIndex, other.NpkIndex, sizeof(int) * VisualType::Max);
	JCore::Memory::CopyUnsafe(ImgIndex, other.ImgIndex, sizeof(int) * VisualType::Max);
}

// 스킨이 설정안된 녀석은 유효하지 않은 정보임
bool VisualInfo::isValid() const {
	return NpkIndex[VisualType::Skin] != InvalidValue_v;
}

ActorPartSpriteData::ActorPartSpriteData()
	: NpkIndex(InvalidValue_v)
	, ZOrder(0)
	, ImgIndex(InvalidValue_v)
{}

ActorPartSpriteData::ActorPartSpriteData(int zorder, int npk, int img)
	: NpkIndex(npk)
	, ZOrder(zorder)
	, ImgIndex(img)
{}

NpkResourceIndex::NpkResourceIndex() : Value(0) {}


AccountData::AccountData()
	: Name{""}
	, AccountId(InvalidValue_v)
	, Pass{""}
	, LastLogin(0)
	, LastServer(0)
{}

PlayerData::PlayerData()
	: Name{""}
	, CharId(InvalidValue_v)
	, CharType(CharType::Gunner)
	, Gold(0)
	, Sera(0)
	, Str(0)
	, Dex(0)
	, Int(0)
	, Vit(0)
	, Life(0)
	, Mana(0)
	, Level(0)
{}
