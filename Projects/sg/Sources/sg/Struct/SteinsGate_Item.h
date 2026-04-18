/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include "sg/Util/DescBase.h"
#include "sg/Struct/SteinsGate_CharBase.h"


JC_SENUM_BEGIN(InvenItemType)
		Quest,
		Etc,
		Consume,
		Equip,
		Avatar,
		Max
	JC_SENUM_MIDDLE(InvenItemType)

	static constexpr bool Stackable[Max]{
		true,
		true,
		true,
		false,
		false
	};

	static constexpr const char* Name[Max]{
		"quest",
		"etc",
		"consume",
		"equip",
		"avatar"
	};
JC_SENUM_MIDDLE_END(InvenItemType)


// ==========================================================================================
// https://wiki.dfo-world.com/view/Armor
JC_SENUM_BEGIN(ItemType)
	Shoulder, // 어깨
	Begin = Shoulder,
	BeginEquip = Begin,
	Top, // 상의
	Bottom, // 하의
	Shoes, // 신발
	Belt, // 벨트
	Ring, // 반지
	Neck, // 목걸이
	Bracelet, // 팔찌
	Weapon, // 무기
	Title, // 칭호
	Avatar, // 아바타
	EndEquip = Avatar,
	MaxInvenEquip = EndEquip,
	MaxEquip = EndEquip + 1,
	Consume = MaxEquip, // 소모품
	Etc, // 기타
	Quest, // 퀘스트
	End,
	Max = End
JC_SENUM_MIDDLE(ItemType)

inline static constexpr bool IsEquip[Max]{
	true, // 어깨
	true, // 상의
	true, // 하의
	true, // 신발
	true, // 벨트
	true, // 반지
	true, // 목걸이
	true, // 팔찌
	true, // 무기
	true, // 칭호
	true, // 아바타
	false, // 소모품
	false, // 기타
	false, // 퀘스트
};

inline static constexpr bool IsCommonEquip[Max]{
	true, // 어깨
	true, // 상의
	true, // 하의
	true, // 신발
	true, // 벨트
	true, // 반지
	true, // 목걸이
	true, // 팔찌
	false, // 무기
	true, // 칭호
	false, // 아바타
	false, // 소모품
	false, // 기타
	false, // 퀘스트
};

inline static constexpr bool HasDetailType[Max]{
	true, // 어깨
	true, // 상의
	true, // 하의
	true, // 신발
	true, // 벨트
	false, // 반지
	false, // 목걸이
	false, // 팔찌
	true, // 무기
	false, // 칭호
	true, // 아바타
	true, // 소모품
	false, // 기타
	false, // 퀘스트
};

inline static constexpr const char* Name[Max]{
	"shoulder", // 어깨
	"top", // 상의
	"bottom", // 하의
	"shoes", // 신발
	"belt", // 벨트
	"ring", // 반지
	"neck", // 목걸이
	"bracelet", // 팔찌
	"weapon", // 무기
	"title", // 칭호
	"avatar", // 아바타
	"consume", // 소모품
	"etc", // 기타
	"quest" // 퀘스트
};

JC_SENUM_MIDDLE_END(ItemType)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(EquipArmorType)
	Cloth, // 천
	Begin = Cloth,
	Leather, // 가죽
	Light, // 경갑
	Heavy, // 중갑
	Place, // 판금
	End,
	Max = End
JC_SENUM_MIDDLE(EquipArmorType)

static constexpr const char* Name[Max]{
	"cloth",
	"leather",
	"light",
	"heavy",
	"plate"
};

JC_SENUM_MIDDLE_END(EquipArmorType)


//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(RarityType)
	Normal,
	Begin = Normal,
	Magic,
	Rare,
	Legacy,
	Unique,
	Epic,
	End = Epic,
	Max
JC_SENUM_MIDDLE(RarityType)

static constexpr const char* Name[Max]{
	"Normal",
	"Magic",
	"Rare",
	"Legacy",
	"Unique",
	"Epic",
};

static constexpr int OptMinCount[Max]{
	0,
	1,
	2,
	3,
	3,
	3,
};

static constexpr int OptMaxCount[Max]{
	1,
	3,
	5,
	7,
	6,
	0
};

JC_SENUM_MIDDLE_END(RarityType)

//////////////////////////////////////////////////////////////////////////////////////////
JC_SENUM_BEGIN(ItemOptType)
	Begin = 1,
	Str = Begin, // 1
	Dex, // 2
	_s32, // 3
	Vit, // 4
	Energe, // 5
	Lift, // 6
	Mana, // 7 
	DamagePhysical, // 8
	DamageMagic, // 9
	DamagePhysicalAdditional, // 10
	DamageMagicAdditional, // 11
	DamagePhysiclReduce, // 12
	DamageMagicReduce, // 13
	DamagePhysiclReducePercent, // 14
	DamageMagicReducePercent, // 15
	ArmorPhysical, // 16
	ArmorMagic, // 17
	ArmorPhysicalPercent, // 18
	ArmorMagicPercent, // 19
	ChanceMagic, // 20
	ChanceGold, // 21
	AttackSpeed, // 22
	CastSpeed, // 23
	RegenLifePerMin, // 24
	RegenManaPerMin, // 25
	MoveSpeedInDungeon, // 26
	MoveSpeedInTown, // 27
	End = MoveSpeedInTown,
	Max
JC_SENUM_MIDDLE(ItemOptType)

JC_SENUM_MIDDLE_END(ItemOptType)
// ==========================================================================================

// 31                               0
// +-------+-------+-------+--------+
// |  Type |Detail2|Detail1|  Code  |
// +-------+-------+-------+--------+

union ItemCode
{
	inline static constexpr int BIT_CODE = 16; // 65535
	inline static constexpr int BIT_DETAIL1 = 6; // 64
	inline static constexpr int BIT_DETAIL2 = 4; // 16
	inline static constexpr int BIT_TYPE = 6; // 64

	inline static constexpr int BIT_CODE_SHIFT = 0;
	inline static constexpr int BIT_DETAIL1_SHIFT = 16;
	inline static constexpr int BIT_DETAIL2_SHIFT = 22;
	inline static constexpr int BIT_TYPE_SHIFT = 26;

	inline static constexpr int BIT_CODE_MASK = 0x0000ffff;
	inline static constexpr int BIT_DETAIL1_MASK = jc::FillBitRight32<22>() & ~0x0000ffff;
	inline static constexpr int BIT_DETAIL2_MASK = jc::FillBitRight32<26>() & ~jc::FillBitRight32<22>();
	inline static constexpr int BIT_TYPE_MASK = 0xffffffff & jc::FillBitRight32<26>();


	ItemCode()
	: Code(InvalidValue_v)
	{
	}

	ItemCode(int _total); // 얘는 전체 값
	ItemCode(int _code, ItemType_t _type);
	ItemCode(int _code, int _detail1, ItemType_t _type);
	ItemCode(int _code, int _detail1, int _detail2, ItemType_t _type);

	void InitAvatarCode(CharType_t _charType, AvatarType_t _avatarType, int _code);
	void InitWeaponCode(CharType_t _charType, WeaponType_t _weaponType, int _code);
	void InitArmorCode(ItemType_t _itemType, EquipArmorType_t _armorType, int _code);

	// 이렇게 유니온 많이 달아도 일반 유니온 1개짜리랑 성능차이가 있을려나?
	// TODO: 어셈코드 확인해볼 것
	//		TODO: 코드가 다르면 벤치 테스트 해볼 것

	struct
	{
		int code_ : BIT_CODE;
		int detail1_ : BIT_DETAIL1; // 아바타 부위 || 갑옷 종류 || 무기 종류
		int detail2_ : BIT_DETAIL2; // 캐릭터 타입
		ItemType_t type_ : BIT_TYPE; // 소모품, 아바타, 무기
	} commonUn_;

	struct
	{
		int code_ : BIT_CODE;
		AvatarType_t partType_ : BIT_DETAIL1;
		CharType_t charType_ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} avatarUn_;

	struct
	{
		int code_ : BIT_CODE;
		WeaponType_t weaponType_ : BIT_DETAIL1;
		CharType_t charType_ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} weaponUn_;

	struct
	{
		int code_ : BIT_CODE;
		EquipArmorType_t armorType_ : BIT_DETAIL1;
		int _ : BIT_DETAIL2;
		ItemType_t itemType_ : BIT_TYPE;
	} armorUn_;


	int Code;
};

struct ItemOpt
{
	ItemOpt()
	: code_(InvalidValue_v)
	{
	}

	int code_{};
};

struct ItemOptVal : ItemOpt
{
	int value_{};
};

struct ItemOptRangeVal : ItemOpt
{
	int minValue_{};
	int maxValue_{};
};


struct InvenItem
{
	ItemCode code_;
	int quantity_;
};

struct InvenItemEquip : InvenItem
{
	int armorPhisical_;
	int armorMagic_;
	int attackPhysical_;
	int attackMagic_;

	int optCount_;
	ItemOptVal opt_[Const::Item::MaxOptCount];
};

//////////////////////////////////////////////////////////////////////////////////////////
struct ItemInfo : SDescBase
{
	~ItemInfo() override = default;

	jc::String name_;
	jc::String nameEng_;
	RarityType_t rarity_;

	int icon_;
	int sellPrice_;

	ItemType_t GetItemType()
	{
		return ItemCode(code_).commonUn_.type_;
	}
};


struct ItemStackableInfo : ItemInfo
{
	~ItemStackableInfo() override = default;

	int maxStack_;
};

struct ItemConsumeInfo : ItemStackableInfo
{
	~ItemConsumeInfo() override = default;

	int reqLv_;
	int buff_;
};

struct ItemEtcInfo : ItemStackableInfo
{
	~ItemEtcInfo() override = default;
};

struct ItemQuestInfo : ItemStackableInfo
{
	~ItemQuestInfo() override = default;
};

struct ItemEquipInfo : ItemInfo
{
	ItemEquipInfo()
	: reqLv_(1)
	, setCode_(InvalidValue_v)
	{
	}

	~ItemEquipInfo() override = default;

	int reqLv_;
	int setCode_;
};

struct ItemArmorInfo : ItemEquipInfo
{
	~ItemArmorInfo() override = default;

	int armorPhysical_;
	int armorMagical_;

	EquipArmorType_t GetArmorType()
	{
		return ItemCode(code_).armorUn_.armorType_;
	}
};

struct ItemVisualInfo : ItemEquipInfo
{
	~ItemVisualInfo() override = default;

	jc::String shape_;
	jc::String shapeAlpha_;

	CharType_t GetCharType()
	{
		return ItemCode(code_).avatarUn_.charType_;
	}

	AvatarType_t GetAvatarType()
	{
		return ItemCode(code_).avatarUn_.partType_;
	}

	WeaponType_t GetWeaponType()
	{
		return ItemCode(code_).weaponUn_.weaponType_;
	}

	VisualType_t GetVisualType()
	{
		ItemCode itemCode(code_);

		if (itemCode.commonUn_.type_ == ItemType::Avatar)
		{
			return (VisualType_t)itemCode.avatarUn_.partType_;
		}

		if (itemCode.commonUn_.type_ == ItemType::Weapon)
		{
			return VisualType::Weapon;
		}

		jc_assert_msg(false, "비주얼 타입이 아닌 녀석이 비주얼 타입으로 설정되어있습니다.");
		return (VisualType_t)-1;
	}
};

struct ItemAvatarInfo : ItemVisualInfo
{
	~ItemAvatarInfo() override = default;
};

struct ItemWeaponInfo : ItemVisualInfo
{
	~ItemWeaponInfo() override = default;

	int attackPhysical_;
	int attackMagic_;
};
