/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 7:20:01 PM
 * =====================
 *
 */


#pragma once

#include "Core.h"
#include <SteinsGate/Common/ConfigDataAbstract.h>

struct ItemBaseInfo : ConfigDataAbstract
{
	~ItemBaseInfo() override = default;

	SGString Name;
	SGString Explain;
	ItemType_t Type;
	RarityType_t Rarity;

	bool Sellable;
	int SellPrice;
};

struct ItemStackableInfo : ItemBaseInfo
{
	~ItemStackableInfo() override = default;

	int MaxStack;
};

struct ItemConsumeBaseInfo : ItemStackableInfo
{
	~ItemConsumeBaseInfo() override = default;

	int ReqLv;
	int Buff;
};

struct ItemEtcBaseInfo : ItemStackableInfo
{
	~ItemEtcBaseInfo() override = default;
};

struct ItemQuestBaseInfo : ItemStackableInfo
{
	~ItemQuestBaseInfo() override = default;
};

struct ItemAvatarBaseInfo : ItemBaseInfo
{
	~ItemAvatarBaseInfo() override = default;

	int ReqLv;
	int BaseOptCount;
	int BaseOpt[MaxOptCount_v];
};

struct ItemEquipBaseInfo : ItemBaseInfo
{
	~ItemEquipBaseInfo() override = default;

	int ReqLv;
	int BaseOptCount;
	int BaseOpt[MaxOptCount_v];
};


