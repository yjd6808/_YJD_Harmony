/*
	작성자 : 윤정도
*/

#pragma once

#include <JCore/Type.h>

template <typename T>
struct Vec2Template
{
private:
	using TVec = Vec2Template<T>;
public:
	TVec operator+(const TVec& other) {
		TVec vec{X, Y};
		vec += other;
		return vec;
	}

	TVec operator-(const TVec& other) {
		TVec vec{ X, Y };
		vec -= other;
		return vec;
	}

	TVec& operator+=(const T& other) {
		X += other;
		Y += other;
		return *this;
	}

	TVec& operator-=(const T& other) {
		X -= other;
		Y -= other;
		return *this;
	}

	TVec& operator+=(const TVec& other) {
		X += other.X;
		Y += other.Y;
		return *this;
	}

	TVec& operator-=(const TVec& other) {
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	T X{};
	T Y{};
};

using Vec2Int	= Vec2Template<Int32>;
using Vec2Short	= Vec2Template<Int16>;
using Vec2Char	= Vec2Template<Int8>;

