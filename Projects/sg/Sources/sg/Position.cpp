/*
 * 작성자: 윤정도
 * 생성일: 8/21/2023 9:49:35 AM
 * =====================
 *
 */

#include "Core.h"
#include "Position.h"

//////////////////////////////////////////////////////////////////////////////////////////
Position::Position()
: x_(0)
, y_(0)
, z_(0)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
Position::Position(const Position& _position)
{
	x_ = _position.x_;
	y_ = _position.y_;
	z_ = _position.z_;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator=(const Position& _position)
{
	x_ = _position.x_;
	y_ = _position.y_;
	z_ = _position.z_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator+=(const Position& _position)
{
	x_ += _position.x_;
	y_ += _position.y_;
	z_ += _position.z_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator-=(const Position& _position)
{
	x_ -= _position.x_;
	y_ -= _position.y_;
	z_ -= _position.z_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator*=(const Position& _position)
{
	x_ *= _position.x_;
	y_ *= _position.y_;
	z_ *= _position.z_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator/=(const Position& _position)
{
	x_ /= _position.x_;
	y_ /= _position.y_;
	z_ /= _position.z_;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator+=(float _value)
{
	x_ += _value;
	y_ += _value;
	z_ += _value;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator-=(float _value)
{
	x_ -= _value;
	y_ -= _value;
	z_ -= _value;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator*=(float _value)
{
	x_ *= _value;
	y_ *= _value;
	z_ *= _value;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position& Position::operator/=(float _value)
{
	x_ /= _value;
	y_ /= _value;
	z_ /= _value;
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator+(const Position& _position)
{
	Position value(*this);
	value.x_ += _position.x_;
	value.y_ += _position.y_;
	value.z_ += _position.z_;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator-(const Position& _position)
{
	Position value(*this);
	value.x_ -= _position.x_;
	value.y_ -= _position.y_;
	value.z_ -= _position.z_;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator*(const Position& _position)
{
	Position value(*this);
	value.x_ *= _position.x_;
	value.y_ *= _position.y_;
	value.z_ *= _position.z_;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator/(const Position& _position)
{
	Position value(*this);
	value.x_ /= _position.x_;
	value.y_ /= _position.y_;
	value.z_ /= _position.z_;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator+(float _value)
{
	Position value(*this);
	value.x_ += _value;
	value.y_ += _value;
	value.z_ += _value;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator-(float _value)
{
	Position value(*this);
	value.x_ -= _value;
	value.y_ -= _value;
	value.z_ -= _value;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator*(float _value)
{
	Position value(*this);
	value.x_ *= _value;
	value.y_ *= _value;
	value.z_ *= _value;
	return value;
}

//////////////////////////////////////////////////////////////////////////////////////////
Position Position::operator/(float _value)
{
	Position value(*this);
	value.x_ /= _value;
	value.y_ /= _value;
	value.z_ /= _value;
	return value;
}
