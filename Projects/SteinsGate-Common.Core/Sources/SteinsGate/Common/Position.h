/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:37:30 AM
 * =====================
 *
 */


#pragma once


struct Position
{
	float X;	// 액터 가로축
	float Y;	// 액터 높이
	float Z;	// 액터 세로축

	Position();
	Position(const Position& other);

	Position& operator=(const Position& other);
	Position& operator+=(const Position& other);
	Position& operator-=(const Position& other);
	Position& operator*=(const Position& other);
	Position& operator/=(const Position& other);

	Position& operator+=(float other);
	Position& operator-=(float other);
	Position& operator*=(float other);
	Position& operator/=(float other);

	Position operator+(const Position& other);
	Position operator-(const Position& other);
	Position operator*(const Position& other);
	Position operator/(const Position& other);

	Position operator+(float other);
	Position operator-(float other);
	Position operator*(float other);
	Position operator/(float other);
	
};


