/*
	작성자 : 윤정도
	예외 모음
	----------------------
	TODO: 스택 트레이스 기능 추가
*/

#pragma once

#include <exception>
#include <stacktrace>
#include <jc/Primitives/String.h>

NS_JC_BEGIN

struct Exception : public std::exception
{
	Exception(const char* _msg) : std::exception(_msg) {}
};

struct NullPointerException : Exception
{
	NullPointerException(const char* _msg) : Exception(_msg) {}
	NullPointerException(const String& _msg) : Exception(_msg.Source()) {}
};

struct RuntimeException : Exception
{
	RuntimeException(const char* _msg) : Exception(_msg) {}
	RuntimeException(const String& _msg) : Exception(_msg.Source()) {}
};

struct InvalidArgumentException : Exception
{
	InvalidArgumentException(const char* _msg) : Exception(_msg) {}
	InvalidArgumentException(const String& _msg) : Exception(_msg.Source()) {}
};

struct OutOfRangeException : public Exception
{
	OutOfRangeException(const char* _msg) : Exception(_msg) {}
	OutOfRangeException(const String& _msg) : Exception(_msg.Source()) {}
};

struct OverFlowException : public Exception
{
	OverFlowException(const char* _msg) : Exception(_msg) {}
	OverFlowException(const String& _msg) : Exception(_msg.Source()) {}
};

struct UnderFlowException : public Exception
{
	UnderFlowException(const char* _msg) : Exception(_msg) {}
	UnderFlowException(const String& _msg) : Exception(_msg.Source()) {}
};

struct InvalidOperationException : public Exception
{
	InvalidOperationException(const char* _msg) : Exception(_msg) {}
	InvalidOperationException(const String& _msg) : Exception(_msg.Source()) {}
};

struct NotImplementedException : public Exception
{
  NotImplementedException(const char* _msg) : Exception(_msg) {}
  NotImplementedException(const String& _msg) : Exception(_msg.Source()) {}
};


NS_JC_END