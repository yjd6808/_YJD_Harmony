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

inline constexpr float FPS1_v = 1.0f / 60.0f;
inline constexpr float FPS2_v = 2.0f / 60.0f;
inline constexpr float FPS3_v = 3.0f / 60.0f;
inline constexpr float FPS4_v = 4.0f / 60.0f;
inline constexpr float FPS5_v = 5.0f / 60.0f;
inline constexpr float FPS6_v = 6.0f / 60.0f;	// 0.1
inline constexpr float FPS7_v = 7.0f / 60.0f;
inline constexpr float FPS8_v = 8.0f / 60.0f;
inline constexpr float FPS9_v = 9.0f / 60.0f;
inline constexpr float FPS10_v = 10.0f / 60.0f;
inline constexpr float FPS11_v = 11.0f / 60.0f;
inline constexpr float FPS12_v = 12.0f / 60.0f;	// 0.2
inline constexpr float FPS13_v = 13.0f / 60.0f;
inline constexpr float FPS14_v = 14.0f / 60.0f;
inline constexpr float FPS15_v = 15.0f / 60.0f;
inline constexpr float FPS16_v = 16.0f / 60.0f;
inline constexpr float FPS17_v = 17.0f / 60.0f;
inline constexpr float FPS18_v = 18.0f / 60.0f; // 0.3


inline const char* DataDirectory_v = "D:/Programming/MyProjects/_DevCpp/UltimateHarmony/Projects/SteinsGate-Data.Resource";
inline const char* ConfigDirectory_v = "D:/Programming/MyProjects/_DevCpp/UltimateHarmony/Projects/SteinsGate-Data.Config/";
inline constexpr int MaxNpkFileCount_v = 200;
inline constexpr int MaxNpkParallelLoadingThreadCount_v = 8;
inline constexpr int MaxMonsterPartsCount_v = 5;


inline constexpr int ComboSequenceCount_v = 5;				// 콤보 가능키 최대 몇개?
inline constexpr float ComboSequenceDelay_v = 0.4f;			// 콤보 입력 제한 시간

inline constexpr int MaxCommandCount_v = 3;	
inline constexpr int InvalidValue_v = -1;

inline constexpr float DefaultMoveSpeedPerSecX_v = 150.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSX_v = DefaultMoveSpeedPerSecX_v / 60.0f;

inline constexpr float DefaultMoveSpeedPerSecY_v = 30.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSY_v = DefaultMoveSpeedPerSecX_v / 60.0f;

inline constexpr float DefaultRunSpeedPerSecX_v = 225.0f;		// 기본 달리기 속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultRunSpeedPerFPSX_v = DefaultRunSpeedPerSecX_v / 60.0f;

