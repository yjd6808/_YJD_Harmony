/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Xml/UIValue.h"

#include "sg/Struct/SteinsGate_UI.h"

#include <tinyxml2.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

USING_NS_CC;
USING_NS_JC;

namespace sgui
{

//////////////////////////////////////////////////////////////////////////////////////////
// UIValue
//////////////////////////////////////////////////////////////////////////////////////////

UIValue UIValue::MakeNull() { return UIValue(); }

UIValue UIValue::MakeBool(bool _value)
{
	UIValue value;
	value.type_ = UIValueType::Bool;
	value.boolValue_ = _value;
	return value;
}

UIValue UIValue::MakeInt(int64_t _value)
{
	UIValue value;
	value.type_ = UIValueType::Int;
	value.intValue_ = _value;
	return value;
}

UIValue UIValue::MakeFloat(double _value)
{
	UIValue value;
	value.type_ = UIValueType::Float;
	value.floatValue_ = _value;
	return value;
}

UIValue UIValue::MakeString(const std::string& _value)
{
	UIValue value;
	value.type_ = UIValueType::String;
	value.stringValue_ = _value;
	return value;
}

UIValue UIValue::MakeArray()
{
	UIValue value;
	value.type_ = UIValueType::Array;
	return value;
}

UIValue UIValue::MakeMap()
{
	UIValue value;
	value.type_ = UIValueType::Map;
	return value;
}

bool UIValue::ToBool() const
{
	switch (type_)
	{
	case UIValueType::Bool: return boolValue_;
	case UIValueType::Int: return intValue_ != 0;
	case UIValueType::Float: return floatValue_ != 0.0;
	case UIValueType::String:
	{
		const char* p = stringValue_.c_str();
		return strcmp(p, "true") == 0 || strcmp(p, "1") == 0 || strcmp(p, "True") == 0;
	}
	case UIValueType::Array: return !arrayValue_.empty();
	case UIValueType::Map: return !mapValue_.empty();
	default: return false;
	}
}

int64_t UIValue::ToInt() const
{
	switch (type_)
	{
	case UIValueType::Int: return intValue_;
	case UIValueType::Float: return (int64_t)floatValue_;
	case UIValueType::Bool: return boolValue_ ? 1 : 0;
	case UIValueType::String: return (int64_t)std::strtoll(stringValue_.c_str(), nullptr, 0);
	default: return 0;
	}
}

double UIValue::ToFloat() const
{
	switch (type_)
	{
	case UIValueType::Float: return floatValue_;
	case UIValueType::Int: return (double)intValue_;
	case UIValueType::Bool: return boolValue_ ? 1.0 : 0.0;
	case UIValueType::String: return std::strtod(stringValue_.c_str(), nullptr);
	default: return 0.0;
	}
}

std::string UIValue::ToString() const
{
	switch (type_)
	{
	case UIValueType::Bool: return boolValue_ ? "true" : "false";
	case UIValueType::Int: return jc::StringUtil::Format("%lld", (long long)intValue_).Source();
	case UIValueType::Float:
	{
		// 정수 값이면 소수점을 생략해 표기한다.
		const double integral = std::floor(floatValue_);
		if (floatValue_ == integral)
		{
			return jc::StringUtil::Format("%lld", (long long)floatValue_).Source();
		}
		return jc::StringUtil::Format("%g", floatValue_).Source();
	}
	case UIValueType::String: return stringValue_;
	case UIValueType::Array:
	{
		std::string out = "[";
		for (size_t i = 0; i < arrayValue_.size(); ++i)
		{
			if (i > 0)
			{
				out += ", ";
			}
			out += arrayValue_[i].ToString();
		}
		out += "]";
		return out;
	}
	case UIValueType::Map:
	{
		std::string out = "{";
		for (size_t i = 0; i < mapValue_.size(); ++i)
		{
			if (i > 0)
			{
				out += ", ";
			}
			out += mapValue_[i].first + ": " + mapValue_[i].second.ToString();
		}
		out += "}";
		return out;
	}
	default: return "";
	}
}

const UIValue* UIValue::GetArrayItem(int _index) const
{
	if (type_ != UIValueType::Array || _index < 0 || _index >= (int)arrayValue_.size())
	{
		return nullptr;
	}

	return &arrayValue_[_index];
}

const UIValue* UIValue::GetMapValue(const char* _key) const
{
	if (type_ != UIValueType::Map || _key == nullptr)
	{
		return nullptr;
	}

	for (const auto& pair : mapValue_)
	{
		if (pair.first == _key)
		{
			return &pair.second;
		}
	}

	return nullptr;
}

const UIValue* UIValue::GetItem(const UIValue& _index) const
{
	if (type_ == UIValueType::Array)
	{
		return GetArrayItem((int)_index.ToInt());
	}

	if (type_ == UIValueType::Map)
	{
		return GetMapValue(_index.ToString().c_str());
	}

	return nullptr;
}

void UIValue::AddArrayItem(const UIValue& _value)
{
	arrayValue_.push_back(_value);
}

void UIValue::SetArrayItem(int _index, const UIValue& _value)
{
	if (_index < 0)
	{
		return;
	}

	EnsureArraySize(_index + 1);
	arrayValue_[_index] = _value;
}

void UIValue::EnsureArraySize(int _size)
{
	if (_size <= (int)arrayValue_.size())
	{
		return;
	}

	arrayValue_.resize(_size);
}

void UIValue::SetMapValue(const std::string& _key, const UIValue& _value)
{
	for (auto& pair : mapValue_)
	{
		if (pair.first == _key)
		{
			pair.second = _value;
			return;
		}
	}

	mapValue_.push_back({ _key, _value });
}

//////////////////////////////////////////////////////////////////////////////////////////
// UIDataList
//////////////////////////////////////////////////////////////////////////////////////////

const UIValue* UIDataList::Find(const char* _key) const
{
	if (_key == nullptr)
	{
		return nullptr;
	}

	for (const auto& pair : values_)
	{
		if (pair.first == _key)
		{
			return &pair.second;
		}
	}

	return nullptr;
}

UIValue* UIDataList::FindMutable(const char* _key)
{
	if (_key == nullptr)
	{
		return nullptr;
	}

	for (auto& pair : values_)
	{
		if (pair.first == _key)
		{
			return &pair.second;
		}
	}

	return nullptr;
}

void UIDataList::Set(const std::string& _key, const UIValue& _value)
{
	for (auto& pair : values_)
	{
		if (pair.first == _key)
		{
			pair.second = _value;
			return;
		}
	}

	values_.push_back({ _key, _value });
}

//////////////////////////////////////////////////////////////////////////////////////////
// DataList / Data 파싱
//////////////////////////////////////////////////////////////////////////////////////////

namespace
{

//////////////////////////////////////////////////////////////////////////////////////////
static bool IsIntegerString(const char* _text)
{
	if (_text == nullptr || _text[0] == '\0')
	{
		return false;
	}

	const char* p = _text;
	if (*p == '+' || *p == '-')
	{
		++p;
	}

	if (*p == '\0')
	{
		return false;
	}

	for (; *p; ++p)
	{
		if (*p < '0' || *p > '9')
		{
			return false;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
static int64_t ParseIntLiteral(const std::string& _text)
{
	const std::string trimmed = _text;
	const char* p = trimmed.c_str();

	if (trimmed.size() >= 2 && trimmed[0] == '0' && (trimmed[1] == 'b' || trimmed[1] == 'B'))
	{
		int64_t result = 0;
		for (size_t i = 2; i < trimmed.size(); ++i)
		{
			const char c = trimmed[i];
			if (c != '0' && c != '1')
			{
				break;
			}
			result = (result << 1) | (c - '0');
		}
		return result;
	}

	if (trimmed.size() >= 2 && trimmed[0] == '0' && (trimmed[1] == 'x' || trimmed[1] == 'X'))
	{
		return (int64_t)std::strtoll(p + 2, nullptr, 16);
	}

	return (int64_t)std::strtoll(p, nullptr, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////
static bool ParseBool(const std::string& _text)
{
	return _text == "true" || _text == "True" || _text == "1";
}

//////////////////////////////////////////////////////////////////////////////////////////
static UIValue ParseScalar(const std::string& _text, const char* _type)
{
	std::string type = _type ? _type : "String";

	if (type == "Bool" || type == "Boolean")
	{
		return UIValue::MakeBool(ParseBool(_text));
	}

	if (type == "String")
	{
		return UIValue::MakeString(_text);
	}

	// 정수 계열: _u8 ~ _u64, _s8 ~ _s64, Int, Integer, UInt ...
	if (type.rfind("_u", 0) == 0 || type.rfind("_s", 0) == 0
		|| type == "Int" || type == "Integer" || type == "UInt" || type == "SInt")
	{
		return UIValue::MakeInt(ParseIntLiteral(_text));
	}

	// 실수 계열: _f32, _f64, Float, Double ...
	if (type.rfind("_f", 0) == 0 || type == "Float" || type == "Double")
	{
		return UIValue::MakeFloat(std::strtod(_text.c_str(), nullptr));
	}

	// 인식 불가 타입이면 문자열로 보관한다.
	return UIValue::MakeString(_text);
}

//////////////////////////////////////////////////////////////////////////////////////////
static bool IsContainerType(const char* _type)
{
	return _type != nullptr && (strcmp(_type, "Map") == 0 || strcmp(_type, "Array") == 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
static UIValue ParseData(tinyxml2::XMLElement* _dataElement)
{
	// 1) 자식 <DataList>가 있으면 Map/Array 컨테이너
	tinyxml2::XMLElement* pDataList = _dataElement->FirstChildElement("DataList");

	if (pDataList != nullptr)
	{
		const char* typeAttr = _dataElement->Attribute("Type");
		const bool asArray = typeAttr != nullptr && strcmp(typeAttr, "Array") == 0;

		UIValue container = asArray ? UIValue::MakeArray() : UIValue::MakeMap();
		int implicitIndex = 0;

		for (tinyxml2::XMLElement* pChild = pDataList->FirstChildElement("Data");
			pChild != nullptr; pChild = pChild->NextSiblingElement("Data"))
		{
			const char* key = pChild->Attribute("Key");
			UIValue item = ParseData(pChild);

			if (asArray)
			{
				int index = implicitIndex;
				if (key != nullptr && IsIntegerString(key))
				{
					index = atoi(key);
				}
				container.SetArrayItem(index, item);
				++implicitIndex;
			}
			else
			{
				container.SetMapValue(key ? key : "", item);
			}
		}

		return container;
	}

	// 2) 자식 <Value> 요소 텍스트 (멀티라인)
	if (tinyxml2::XMLElement* pValueElement = _dataElement->FirstChildElement("Value"))
	{
		const char* text = pValueElement->GetText();
		return UIValue::MakeString(text ? text : "");
	}

	// 3) Value 속성 + Type 속성
	const char* valueAttr = _dataElement->Attribute("Value");
	const char* typeAttr = _dataElement->Attribute("Type");
	return ParseScalar(valueAttr ? valueAttr : "", typeAttr);
}

} // namespace

//////////////////////////////////////////////////////////////////////////////////////////
UIDataList ParseDataList(tinyxml2::XMLElement* _dataListElement)
{
	UIDataList dataList;

	if (_dataListElement == nullptr)
	{
		return dataList;
	}

	for (tinyxml2::XMLElement* pData = _dataListElement->FirstChildElement("Data");
		pData != nullptr; pData = pData->NextSiblingElement("Data"))
	{
		const char* key = pData->Attribute("Key");
		dataList.Set(key ? key : "", ParseData(pData));
	}

	return dataList;
}

} // namespace sgui
