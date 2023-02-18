/*
 * 작성자: 윤정도
 * 생성일: 1/21/2023 9:14:24 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGEnums.h>
#include <SteinsGate/Common/SGConfig.h>

#include <JCore/Time.h>
#include <JCore/Primitives/StaticString.h>

template <Int32U Size>
using SGStaticString = JCore::StaticString<Size>;
using SGString = JCore::String;
using SGDateTime = JCore::DateTime;

struct ThicknessBox
{
	float RelativeX;
	float RelativeY;
	float Width;
	float Height;

	ThicknessBox() : RelativeX(0), RelativeY(0), Width(0), Height(0) {}
	ThicknessBox(float relaitveX, float relativeY, float width, float height)
		: RelativeX(relaitveX)
		, RelativeY(relativeY)
		, Width(width)
		, Height(height) {}
};




struct VisualInfo
{
	int NpkIndex[VisualType::Max];
	int ImgIndex[VisualType::Max];

	VisualInfo();
	VisualInfo(const VisualInfo& other);

	bool isValid() const;
};

struct ActorPartSpriteData
{
	ActorPartSpriteData();
	ActorPartSpriteData(int zorder, int npk, int img);

	int ZOrder;
	int NpkIndex;
	int ImgIndex;
};


union NpkResourceIndex
{
	NpkResourceIndex();
	NpkResourceIndex(int npkIndex, int imgIndex, int frameIndex)
		: Un{frameIndex, imgIndex, npkIndex}
	{}

	struct
	{
		int FrameIndex  : 12;	// 4096
		int ImgIndex	: 11;	// 2048
		int NpkIndex	: 9;	// 512	
	} Un;

	Int32U Value;
};

struct AccountData
{
	AccountData();

	int AccountId;	// 계정 고유 아이디
	SGStaticString<AccountIdLen_v> Name;
	SGStaticString<AccountPassLen_v> Pass;
	SGDateTime LastLogin;
	int LastServer;
};

struct PlayerData
{
	PlayerData();

	int CharId;	// 캐릭터 고유 아이디
	SGStaticString<CharNameLen_v> Name;
	CharType_t CharType;
	int Gold;
	int Sera;
	int Str;
	int Dex;
	int Int;
	int Vit;
	int Life;
	int Mana;
	int Level;
};


struct ItemOptPair
{
	ItemOptType_t Type;
	int Value;
};

struct InvenItem
{
	int Code;
	int Quantity;
};

struct InvenItemEquip : InvenItem
{
	int ArmorPhysical;
	int ArmorMagic;
	int DamagePhysical;
	int DamageMagic;

	int OptCount;
	ItemOptPair Opt[MaxOptCount_v];
};



