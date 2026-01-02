/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:37:30 AM
 * =====================
 *
 */

#pragma once


struct Position
{
	float x_; // 액터 가로축
	float y_; // 액터 높이
	float z_; // 액터 세로축

	Position();
	Position(const Position& _position);

	Position& operator=(const Position& _position);
	Position& operator+=(const Position& _position);
	Position& operator-=(const Position& _position);
	Position& operator*=(const Position& _position);
	Position& operator/=(const Position& _position);

	Position& operator+=(float _value);
	Position& operator-=(float _value);
	Position& operator*=(float _value);
	Position& operator/=(float _value);

	Position operator+(const Position& _position);
	Position operator-(const Position& _position);
	Position operator*(const Position& _position);
	Position operator/(const Position& _position);

	Position operator+(float _value);
	Position operator-(float _value);
	Position operator*(float _value);
	Position operator/(float _value);
};
