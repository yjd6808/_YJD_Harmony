/*
	작성자 : 윤정도
	예외 모음
*/

#pragma once

#include <exception>

namespace JCore {

struct NullPointerException : public std::exception
{
	NullPointerException(const char* msg) : std::exception(msg) {}
};

struct RuntimeException : public std::exception
{
	RuntimeException(const char* msg) : std::exception(msg) {}
};

struct InvalidArgumentException : public std::exception
{
	InvalidArgumentException(const char* msg) : std::exception(msg) {}
};

struct OutOfRangeException : public std::exception
{
	OutOfRangeException(const char* msg) : std::exception(msg) {}
};

struct OverFlowException : public std::exception
{
	OverFlowException(const char* msg) : std::exception(msg) {}
};

struct UnderFlowException : public std::exception
{
	UnderFlowException(const char* msg) : std::exception(msg) {}
};

struct InvalidOperationException : public std::exception
{
	InvalidOperationException(const char* msg) : std::exception(msg) {}
};


} // namespace JCore