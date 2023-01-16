/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Tutturu.h>

#include <SteinsGate/Common/Core/Npk/NpkPackage.h>
#include <SteinsGate/Common/Core/Npk/NpkSpriteRect.h>

#include <SteinsGate/Research/Enums.h>
#include <SteinsGate/Research/FrameTexture.h>


class PackLoader
{
public:
	static PackLoader* GetInstance() {
		static PackLoader* loader;
		if (loader == nullptr) {
			loader = new PackLoader;
			loader->Initialize();
		}
		return loader;
	}

	void Initialize();

	NpkDataPtr CreateTextureData(int packIndex, int imgIndex, int frameIndex);
	cocos2d::Texture2D* CreateTexture(int packIndex, int imgIndex, int frameIndex, NpkSpriteRect& canvas);
	FrameTexturePtr CreateFrameTexture(int packIndex, int imgIndex, int frameIndex);

	NpkPackagePtr Packs[20];
	inline static JCore::HashMap<int, JCore::String> PackPathMap
	{
		{ CharacterPartType::Skin    , "gunner/sprite_character_gunner_equipment_avatar_skin.NPK"  },
		{ CharacterPartType::Shoes	, "gunner/sprite_character_gunner_equipment_avatar_shoes.NPK" },
		{ CharacterPartType::Pants	, "gunner/sprite_character_gunner_equipment_avatar_pants.NPK" },
		{ CharacterPartType::Neck 	, "gunner/sprite_character_gunner_equipment_avatar_neck.NPK"  },
		{ CharacterPartType::Hair 	, "gunner/sprite_character_gunner_equipment_avatar_hair.NPK"  },
		{ CharacterPartType::Face 	, "gunner/sprite_character_gunner_equipment_avatar_face.NPK"  },
		{ CharacterPartType::Coat 	, "gunner/sprite_character_gunner_equipment_avatar_coat.NPK"  },
		{ CharacterPartType::Cap 	, "gunner/sprite_character_gunner_equipment_avatar_cap.NPK"   },
		{ CharacterPartType::Belt	, "gunner/sprite_character_gunner_equipment_avatar_belt.NPK"  },
		{ CharacterPartType::Weapon  , "gunner/sprite_character_gunner_equipment_weapon_auto.NPK"  },
		{ 11, "gunner/sprite_character_gunner_effect_bullet_exs.NPK" }
	};
};


