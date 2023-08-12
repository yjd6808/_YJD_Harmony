/*
 * 작성자: 윤정도
 * 생성일: 2/2/2023 4:23:12 PM
 * =====================
 * 엑터 전용 리스너
 * - Projectile
 * - AttackBox
 * 에 사용할 듯
 *
 */


#pragma once

// 캐릭터 리스너는 CharType 값과 동일하다. 
#define CHARACTER_LISTENER_GUNNER				0

// 몬스터 리스너는 몬스터 코드와 동일하다.
#define MONSTER_LISTENER_NORMAL_GOBLIN			1
#define MONSTER_LISTENER_DARK_GOBLIN			2

// 프로젝틸 리스너는 프로젝트 코드와 개별적으로 존재한다.
// TODO: 프로젝틸 리스너와 프로젝틸을 통합하기 위한 구조 고민.
#define PROJECTILE_LISTENER_GUNNER_BULLET		1
