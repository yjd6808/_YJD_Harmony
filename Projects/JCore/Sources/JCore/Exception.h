/*
	�ۼ��� : ������
	���� ����
*/

#pragma once

#include <exception>
#include <JCore/String.h>

namespace JCore {

struct Exception : public std::exception
{
	Exception(const char* msg) : std::exception(msg) {}
};

struct NullPointerException : Exception
{
	NullPointerException(const char* msg) : Exception(msg) {}
	NullPointerException(const String& msg) : Exception(msg.Source()) {}
};

struct RuntimeException : Exception
{
	RuntimeException(const char* msg) : Exception(msg) {}
	RuntimeException(const String& msg) : Exception(msg.Source()) {}
};

struct InvalidArgumentException : Exception
{
	InvalidArgumentException(const char* msg) : Exception(msg) {}
	InvalidArgumentException(const String& msg) : Exception(msg.Source()) {}
};

struct OutOfRangeException : public Exception
{
	OutOfRangeException(const char* msg) : Exception(msg) {}
	OutOfRangeException(const String& msg) : Exception(msg.Source()) {}
};

struct OverFlowException : public Exception
{
	OverFlowException(const char* msg) : Exception(msg) {}
	OverFlowException(const String& msg) : Exception(msg.Source()) {}
};

struct UnderFlowException : public Exception
{
	UnderFlowException(const char* msg) : Exception(msg) {}
	UnderFlowException(const String& msg) : Exception(msg.Source()) {}
};

struct InvalidOperationException : public Exception
{
	InvalidOperationException(const char* msg) : Exception(msg) {}
	InvalidOperationException(const String& msg) : Exception(msg.Source()) {}
};


} // namespace JCore