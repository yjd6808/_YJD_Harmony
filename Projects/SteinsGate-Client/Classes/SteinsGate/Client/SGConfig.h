/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 5:02:45 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/SGConfig.h>

inline constexpr int MaxNpkFileCount_v = 200;
inline constexpr int MaxNpkParallelLoadingThreadCount_v = 8;
inline constexpr int MaxMonsterPartsCount_v = 5;

inline constexpr int ComboSequenceCount_v = 5;				// 콤보 가능키 최대 몇개?
inline constexpr float ComboSequenceDelay_v = FPS18_v;			// 콤보 입력 제한 시간

inline constexpr int MaxCommandCount_v = 3;	

inline constexpr float DefaultMoveSpeedPerSecX_v = 150.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSX_v = DefaultMoveSpeedPerSecX_v * FPS1_v;

inline constexpr float DefaultMoveSpeedPerSecY_v = 30.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSY_v = DefaultMoveSpeedPerSecX_v * FPS1_v;

inline constexpr float DefaultRunSpeedPerSecX_v = 225.0f;		// 기본 달리기 속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultRunSpeedPerFPSX_v = DefaultRunSpeedPerSecX_v * FPS1_v;


inline constexpr int TileWidth_v = 224;		// 맵 타일들 모두 가로길이가 224픽셀임
inline constexpr int TileHeight_v= 184;		// 내가 자른 타일 높이
inline constexpr int BlockSize_v = 20;		// 16 고려해볼 것 타일 가로길이가 다 224임. 일단 테스트 끝나고 수정

inline constexpr int MaxHitList_v = 8;		// 프로젝틸 관통된 객체 기록용 컨테이너 용량


inline constexpr int DeathDownTime_v = FPS6_v;	// 바닥에 사망상태로 닿았을 때 몇초뒤 사망확정될지
inline constexpr int MinPlayHitAnimationOnTheAir_v = FPS18_v;	// 공중에서 Hit 모션 얼마나 재생할지

inline constexpr float ZReodrerDelay_v = FPS6_v;

inline constexpr int ItemQuestMinCode_v = 0;
inline constexpr int ItemEtcMinCode_v = 10'000;
inline constexpr int ItemConsumeMinCode_v = 20'000;
inline constexpr int ItemEquipMinCode_v = 30'000;
inline constexpr int ItemAvatarMinCode_v = 40'000;