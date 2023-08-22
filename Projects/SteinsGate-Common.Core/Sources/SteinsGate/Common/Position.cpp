/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:49:35 AM
 * =====================
 *
 */

#include "Core.h"
#include "Position.h"

Position::Position()
	: X(0)
	, Y(0)
	, Z(0)
{}

Position::Position(const Position& other) {
	X = other.X;
	Y = other.Y;
	Z = other.Z;
}

Position& Position::operator=(const Position& other) {
	X = other.X;
	Y = other.Y;
	Z = other.Z;
	return *this;
}

Position& Position::operator+=(const Position& other) {
	X += other.X;
	Y += other.Y;
	Z += other.Z;
	return *this;
}

Position& Position::operator-=(const Position& other) {
	X -= other.X;
	Y -= other.Y;
	Z -= other.Z;
	return *this;
}

Position& Position::operator*=(const Position& other) {
	X *= other.X;
	Y *= other.Y;
	Z *= other.Z;
	return *this;
}

Position& Position::operator/=(const Position& other) {
	X /= other.X;
	Y /= other.Y;
	Z /= other.Z;
	return *this;
}

Position& Position::operator+=(float other) {
	X += other;
	Y += other;
	Z += other;
	return *this;
}

Position& Position::operator-=(float other) {
	X -= other;
	Y -= other;
	Z -= other;
	return *this;
}

Position& Position::operator*=(float other) {
	X *= other;
	Y *= other;
	Z *= other;
	return *this;
}

Position& Position::operator/=(float other) {
	X /= other;
	Y /= other;
	Z /= other;
	return *this;
}



Position Position::operator+(const Position& other) {
	Position Value(*this);
	Value.X += other.X;
	Value.Y += other.Y;
	Value.Z += other.Z;
	return Value;
}

Position Position::operator-(const Position& other) {
	Position Value(*this);
	Value.X -= other.X;
	Value.Y -= other.Y;
	Value.Z -= other.Z;
	return Value;
}

Position Position::operator*(const Position& other) {
	Position Value(*this);
	Value.X *= other.X;
	Value.Y *= other.Y;
	Value.Z *= other.Z;
	return Value;
}

Position Position::operator/(const Position& other) {
	Position Value(*this);
	Value.X /= other.X;
	Value.Y /= other.Y;
	Value.Z /= other.Z;
	return Value;
}


Position Position::operator+(float other) {
	Position Value(*this);
	Value.X += other;
	Value.Y += other;
	Value.Z += other;
	return Value;
}

Position Position::operator-(float other) {
	Position Value(*this);
	Value.X -= other;
	Value.Y -= other;
	Value.Z -= other;
	return Value;
}

Position Position::operator*(float other) {
	Position Value(*this);
	Value.X *= other;
	Value.Y *= other;
	Value.Z *= other;
	return Value;
}

Position Position::operator/(float other) {
	Position Value(*this);
	Value.X /= other;
	Value.Y /= other;
	Value.Z /= other;
	return Value;
}