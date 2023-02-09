/*
 * 작성자: 윤정도
 * 생성일: 12/15/2022 3:27:09 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Type.h>

NS_JC_BEGIN

enum ByteUnit
{
	eByte,
	eMegaByte,
	eGigaByte,
	eTeraByte,
	eByteUnitMax = eTeraByte
};



template <ByteUnit From, ByteUnit To, typename T>
constexpr T ConvertByteUnit(T from) {
	static_assert(From >= 0 && From <= eByteUnitMax,	"... invalid byte unit");
	static_assert(To >= 0	&& To <= eByteUnitMax,		"... invalid byte unit");

	if constexpr (From == eByte) {
		switch (To) {
		case eByte:		{ return from; }
		case eMegaByte:	{ return from / 1024; }
		case eGigaByte: { return from / 1024 / 1024; }
		case eTeraByte: { return from / 1024 / 1024 / 1024; }
		}
	} else if constexpr (From == eMegaByte) {
		switch (To) {
		case eByte: { return from * 1024; }
		case eMegaByte: { return from; }
		case eGigaByte: { return from / 1024; }
		case eTeraByte: { return from / 1024 * 1024; }
		}
	} else if constexpr (From == eGigaByte) {
		switch (To) {
		case eByte: { return from * 1024 * 1024; }
		case eMegaByte: { return from * 1024; }
		case eGigaByte: { return from; }
		case eTeraByte: { return from / 1024; }
		}
	}

	switch (To) {
	case eByte: { return from * 1024 * 1024 * 1024; }
	case eMegaByte: { return from * 1024 * 1024; }
	case eGigaByte: { return from * 1024; }
	case eTeraByte: { return from; }
	}

	return 0;
}


NS_JC_END