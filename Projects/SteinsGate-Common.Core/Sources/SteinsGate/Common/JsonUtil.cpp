/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Core.h"
#include "JsonUtil.h"

#include <SteinsGate/Common/TextParser.h>


//////////////////////////////////////////////////////////////////////////////////////////
bool JsonUtil::Load(const char* _pFileName, OUT Json::Value& _root)
{
	if (JCore::Path::Extension(_pFileName) != ".json")
		return false;

	std::ifstream reader(_pFileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", _pFileName);
	try
	{
		reader >> _root;
	}
	catch (std::exception& ex)
	{
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", _pFileName, ex.what());
		return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool JsonUtil::Load(const SGString& _fileName, OUT Json::Value& _root)
{
	return Load(_fileName.Source(), _root);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::LoadThrow(const char* _pFileName, Json::Value& _root)
{
	std::ifstream reader(_pFileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", _pFileName);
	reader >> _root;
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::LoadThrow(const SGString& _fileName, Json::Value& _root)
{
	LoadThrow(_fileName.Source(), _root);
}


//////////////////////////////////////////////////////////////////////////////////////////
SGString JsonUtil::GetString(Json::Value& _value)
{
	DebugAssertMsg(!_value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	_value.getString(&pBegin, &pEnd);
	const int length = pEnd - pBegin;
	return { pBegin, length + 1 };
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* JsonUtil::GetStringRaw(Json::Value& _value, OUT_OPT int* _pLength /* = nullptr */)
{
	DebugAssertMsg(!_value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	_value.getString(&pBegin, &pEnd);
	if (_pLength != nullptr)
		*_pLength = pEnd - pBegin;
	return pBegin;
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString JsonUtil::GetStringOrDefault(Json::Value& _value, const SGString& _defaultValue)
{
	if (_value.empty())
		return _defaultValue;

	return GetString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
SGString JsonUtil::GetStringOrNull(Json::Value& _value)
{
	if (_value.empty())
		return { 0 };

	return GetString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseThicknessInfo(Json::Value& _thicknessRoot, OUT ThicknessBox& _box)
{
	int num[4];
	TextParser::ParseIntNumbers(GetString(_thicknessRoot), num, 4);

	_box.relativeX_ = (float)num[0];
	_box.relativeY_ = (float)num[1];
	_box.width_ = (float)num[2];
	_box.height_ = (float)num[3];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber2(Json::Value& _root, int& _num1, int& _num2)
{
	int num[2];
	TextParser::ParseIntNumbers(GetString(_root), num, 2);

	_num1 = num[0];
	_num2 = num[1];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber3(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3)
{
	int num[3];
	TextParser::ParseIntNumbers(GetString(_root), num, 3);

	_num1 = num[0];
	_num2 = num[1];
	_num3 = num[2];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber4(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3,
                               OUT int& _num4)
{
	int num[4];
	TextParser::ParseIntNumbers(GetString(_root), num, 4);

	_num1 = num[0];
	_num2 = num[1];
	_num3 = num[2];
	_num4 = num[3];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumberN(Json::Value& _root, OUT int* _pNumArr, int _count)
{
	TextParser::ParseIntNumbers(GetString(_root), _pNumArr, _count);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseFloatNumber2(Json::Value& _root, OUT float& _num1, OUT float& _num2)
{
	float num[2];
	TextParser::ParseFloatNumbers(GetString(_root), num, 2);
	_num1 = num[0];
	_num2 = num[1];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseFloatNumberN(Json::Value& _root, OUT float* _pNumArr, int _count)
{
	TextParser::ParseFloatNumbers(GetString(_root), _pNumArr, _count);
}


//////////////////////////////////////////////////////////////////////////////////////////
float JsonUtil::GetFloatDefault(Json::Value& _value, float _defaultValue /* = 0 */)
{
	if (_value.empty())
		return _defaultValue;

	return _value.asFloat();
}

//////////////////////////////////////////////////////////////////////////////////////////
int JsonUtil::GetIntDefault(Json::Value& _value, int _defaultValue /* = 0 */)
{
	if (_value.empty())
		return _defaultValue;

	return _value.asInt();
}

//////////////////////////////////////////////////////////////////////////////////////////
bool JsonUtil::GetBooleanDefault(Json::Value& _value, bool _defaultValue /* = false */)
{
	if (_value.empty())
		return _defaultValue;

	return _value.asBool();
}
