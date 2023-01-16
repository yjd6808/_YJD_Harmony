/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 5:02:45 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Research/Enums.h>

#include <JCore/Container/Arrays.h>
#include <JCore/TypeTraits.h>
#include <JCore/Tuple.h>

inline constexpr float WorldFPS_v = 1.0f / 60.0f;

inline constexpr float CharacterSpriteFrameDelay_v = 0.1f;	// 0.1초

inline constexpr int CharacterFrameCount_v[CharacterType::Max]{
	203
};

inline constexpr int CharacterPartZOrders_v[CharacterPartType::Max]
{
	0, // CharacterPartType::Skin
	1, // CharacterPartType::Shoes
	2, // CharacterPartType::Pants
	5, // CharacterPartType::Neck
	2, // CharacterPartType::Hair
	1, // CharacterPartType::Face
	4, // CharacterPartType::Coat
	3, // CharacterPartType::Cap
	6, // CharacterPartType::Belt
	7  // CharacterPartType::Weapon
};


struct CharacterPartDefaultImg {
	static int Get(CharacterType_t characterType, CharacterPartType_t partType) { 
		return ImgIndex[characterType][partType];
	}

	inline constexpr static int ImgIndex[CharacterType::Max][CharacterPartType::Max] =
	{
		{	
			1, // CharacterPartType::Skin
			0, // CharacterPartType::Shoes
			0, // CharacterPartType::Pants
			0, // CharacterPartType::Neck
			0, // CharacterPartType::Hair
			4, // CharacterPartType::Face
			0, // CharacterPartType::Coat
			27,// CharacterPartType::Cap
			8, // CharacterPartType::Belt
			1  // CharacterPartType::Weapon
		},

	};
};


inline constexpr JCore::Tuple<int, int> MotionStatePairs_v[]
{
	{ MotionStateBegin::IdleBreath		     , MotionStateEnd::IdleBreath			},
	{ MotionStateBegin::IdleReload		   	 , MotionStateEnd::IdleReload			},
	{ MotionStateBegin::IdleGunRolling 	   	 , MotionStateEnd::IdleGunRolling		},
	{ MotionStateBegin::ShotBegin 		   	 , MotionStateEnd::ShotBegin			},
	{ MotionStateBegin::ShotShot 		   	 , MotionStateEnd::ShotShot				},
	{ MotionStateBegin::ShotEnd 			 , MotionStateEnd::ShotEnd				},
	{ MotionStateBegin::ShotOtherBegin 	   	 , MotionStateEnd::ShotOtherBegin		},
	{ MotionStateBegin::ShotOtherShot 	   	 , MotionStateEnd::ShotOtherShot		},
	{ MotionStateBegin::ShotOtherEnd 	   	 , MotionStateEnd::ShotOtherEnd			},
	{ MotionStateBegin::ShotDownBegin 	   	 , MotionStateEnd::ShotDownBegin		},
	{ MotionStateBegin::ShotDownShot 	   	 , MotionStateEnd::ShotDownShot			},
	{ MotionStateBegin::ShotDownEnd 		 , MotionStateEnd::ShotDownEnd			},
	{ MotionStateBegin::ShotOtherDownBegin 	 , MotionStateEnd::ShotOtherDownBegin	},
	{ MotionStateBegin::ShotOtherDownShot  	 , MotionStateEnd::ShotOtherDownShot	},
	{ MotionStateBegin::ShotOtherDownEnd   	 , MotionStateEnd::ShotOtherDownEnd		},
	{ MotionStateBegin::Walk 				 , MotionStateEnd::Walk					},
	{ MotionStateBegin::JumpBegin 		   	 , MotionStateEnd::JumpBegin			},
	{ MotionStateBegin::JumpingUp 		   	 , MotionStateEnd::JumpingUp			},
	{ MotionStateBegin::JumpingDown 		 , MotionStateEnd::JumpingDown			},
	{ MotionStateBegin::JumpingDownGround  	 , MotionStateEnd::JumpingDownGround	},
	{ MotionStateBegin::JumpingShotBegin   	 , MotionStateEnd::JumpingShotBegin		},
	{ MotionStateBegin::JumpingShot 		 , MotionStateEnd::JumpingShot			},
	{ MotionStateBegin::Run				   	 , MotionStateEnd::Run					},
	{ MotionStateBegin::Sliding 			 , MotionStateEnd::Sliding				},
	{ MotionStateBegin::Windmill 			 , MotionStateEnd::Windmill				},
	{ MotionStateBegin::Hit1 				 , MotionStateEnd::Hit1					},
	{ MotionStateBegin::Hit2 				 , MotionStateEnd::Hit2					},
	{ MotionStateBegin::AirHit 			   	 , MotionStateEnd::AirHit				},
	{ MotionStateBegin::AirHitGround 		 , MotionStateEnd::AirHitGround			},
	{ MotionStateBegin::Sit 				 , MotionStateEnd::Sit					},
	{ MotionStateBegin::JackSpike 		   	 , MotionStateEnd::JackSpike			},
	{ MotionStateBegin::MahaKick 			 , MotionStateEnd::MahaKick				}
};


inline constexpr int MotionDetail_IndexMap[MotionState::Max] {
	MotionStateBegin::IdleBreath		  ,
	MotionStateBegin::IdleReload		  ,
	MotionStateBegin::IdleGunRolling 	  ,
	MotionStateBegin::ShotBegin 		  ,
	MotionStateBegin::ShotShot 		  	  ,
	MotionStateBegin::ShotEnd 			  ,
	MotionStateBegin::ShotOtherBegin 	  ,
	MotionStateBegin::ShotOtherShot 	  ,
	MotionStateBegin::ShotOtherEnd 	  	  ,
	MotionStateBegin::ShotDownBegin 	  ,
	MotionStateBegin::ShotDownShot 	  	  ,
	MotionStateBegin::ShotDownEnd 		  ,
	MotionStateBegin::ShotOtherDownBegin  ,
	MotionStateBegin::ShotOtherDownShot   ,
	MotionStateBegin::ShotOtherDownEnd    ,
	MotionStateBegin::Walk 				  ,
	MotionStateBegin::JumpBegin 		  ,
	MotionStateBegin::JumpingUp 		  ,
	MotionStateBegin::JumpingDown 		  ,
	MotionStateBegin::JumpingDownGround   ,
	MotionStateBegin::JumpingShotBegin    ,
	MotionStateBegin::JumpingShot 		  ,
	MotionStateBegin::Run				  ,
	MotionStateBegin::Sliding 			  ,
	MotionStateBegin::Windmill 			  ,
	MotionStateBegin::Hit1 				  ,
	MotionStateBegin::Hit2 				  ,
	MotionStateBegin::AirHit 			  ,
	MotionStateBegin::AirHitGround 		  ,
	MotionStateBegin::Sit 				  ,
	MotionStateBegin::JackSpike 		  ,
	MotionStateBegin::MahaKick 		
};

inline int MotionDetail_TypeMap[MotionStateBegin::Max] {};
inline auto MotionDetail_TypeMapInitializer = []()->int {
	for (int i = 0; i < MotionState::Max; ++i) {
		const int iBegin = MotionStatePairs_v[i].item1;
		const int iEnd = MotionStatePairs_v[i].item2;

		for (int j = iBegin; j <= iEnd; j++) {
			MotionDetail_TypeMap[j] = i;
		}
	}
	return 0;
}();



inline constexpr int ComboSequenceCount_v = 5;			// 키 입력 순서를 최대 몇개까지 기록할지 (콤보 인지용)
inline constexpr float ComboSequenceDelay_v = 0.4f;


inline constexpr float DefaultMoveSpeedPerSecX_v = 150.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSX_v = DefaultMoveSpeedPerSecX_v / 60.0f;

inline constexpr float DefaultMoveSpeedPerSecY_v = 30.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSY_v = DefaultMoveSpeedPerSecX_v / 60.0f;

inline constexpr float DefaultRunSpeedPerSecX_v = 225.0f;		// 기본 달리기 속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultRunSpeedPerFPSX_v = DefaultRunSpeedPerSecX_v / 60.0f;

inline constexpr float AttackInputWaitingDelay_v = 0.4f;	// 공격 후 다음 공격으로 이어지기까지 대기시간
inline constexpr float RunningInputWaitingDelay_v = 0.5f;	// 걷기에서 달리기 전환 키 입력 대기시간

inline constexpr float SlidingBeforeDelay_v = 0.1f;			// 슬라이딩전 경직 몇초동안 줄지
inline constexpr float SlidingMoveByDistance_v = 400.0f;	// 슬라이딩 거리
inline constexpr float SlidingDuration_v = 0.8f;			// SlidingMoveByDistance_v 거리를 몇초만에 갈지

inline constexpr float ThicknessBoxWidth_v = 80.0f;			// 두께박스 가로길이
inline constexpr float ThicknessBoxHeight_v = 20.0f;		// 두께박스 세로길이
inline constexpr float ThicknessRelativeY_v = -130.0f;		// 캐릭터 캔버스 중심에서부터 얼마나 떨어져 있는지(양수는 위로, 음수는 아래로)

inline constexpr float BulletSpeed_v = 400.0f;
inline constexpr float BulletSpeedFPS_v = BulletSpeed_v / 60.0f;