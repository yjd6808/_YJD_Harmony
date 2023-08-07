/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 5:02:45 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/Const.h>

inline constexpr int MaxSgaFileCount_v = 200;
inline constexpr int MaxSgaParallelLoadingThreadCount_v = 8;
inline constexpr int MaxMonsterPartsCount_v = 5;				// 일단 5개로 둠.
inline constexpr int MaxMapObjectGatePartsCount_v = 3;			// 문 테두리, 문, 빛 게이트는 이렇게 3개의 파츠로 구성됨

inline constexpr int ComboSequenceCount_v = 5;					// 콤보 가능키 최대 몇개?
inline constexpr float ComboSequenceDelay_v = FPS18_v;			// 콤보 입력 제한 시간

inline constexpr int MaxCommandCount_v = 3;	

inline constexpr float DefaultMoveSpeedPerSecX_v = 150.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSX_v = DefaultMoveSpeedPerSecX_v * FPS1_v;

inline constexpr float DefaultMoveSpeedPerSecY_v = 30.0f;		// 기본 이동속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultMoveSpeedPerFPSY_v = DefaultMoveSpeedPerSecX_v * FPS1_v;

inline constexpr float DefaultRunSpeedPerSecX_v = 225.0f;		// 기본 달리기 속도 (1초당 이동 가능한 픽셀 수)
inline constexpr float DefaultRunSpeedPerFPSX_v = DefaultRunSpeedPerSecX_v * FPS1_v;

inline constexpr int MaxHitList_v = 8;		// 프로젝틸 관통된 객체 기록용 컨테이너 용량


inline constexpr float DeathDownTime_v = FPS6_v;	// 바닥에 사망상태로 닿았을 때 몇초뒤 사망확정될지
inline constexpr float MinPlayHitAnimationOnTheAir_v = FPS18_v;	// 공중에서 Hit 모션 얼마나 재생할지

inline constexpr float ZReodrerDelay_v = FPS6_v;
