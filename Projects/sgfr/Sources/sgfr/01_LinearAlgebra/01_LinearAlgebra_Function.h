/*
 * 작성자: 윤정도
 * 생성일: 8/5/2026 9:10:00 AM
 * =====================
 * 01. 선형대수 - 보조 함수 (콘솔 출력 헬퍼)
 */

#pragma once

// Vec2를 "이름: (x, y)" 형태로 콘솔에 출력한다.
void PrintVec2(const char* _szName, const jc::vec2& _v);

// Vec3를 "이름: (x, y, z)" 형태로 콘솔에 출력한다.
void PrintVec3(const char* _szName, const jc::vec3& _v);

// Mat4를 4x4 격자 형태로 콘솔에 출력한다.
void PrintMat4(const char* _szName, const jc::mat4& _m);
