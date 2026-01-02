/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:55:09 PM
 * =====================
 *
 */


#pragma once

struct JsonUtil
{
public:
	static bool Load(const char* _pFileName, OUT Json::Value& _root);
	static bool Load(const jc::String& _fileName, OUT Json::Value& _root);
	static void LoadThrow(const char* _pFileName, OUT Json::Value& _root);
	static void LoadThrow(const jc::String& _fileName, OUT Json::Value& _root);

	static jc::String GetString(Json::Value& _value);
	static const char* GetStringRaw(Json::Value& _value, OUT_OPT int* _pLength = nullptr);
	static jc::String GetStringOrNull(Json::Value& _value);
	static jc::String GetStringOrDefault(Json::Value& _value, const jc::String& _defaultValue = 0);
	// 내 문자열은 0으로 초기화하면 동적 할당 안함
	static void ParseThicknessInfo(Json::Value& _thicknessRoot, OUT ThicknessBox& _box);

	static void ParseIntNumber2(Json::Value& _root, OUT int& _num1, OUT int& _num2);
	static void ParseIntNumber3(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3);
	static void ParseIntNumber4(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3,
	                            OUT int& _num4);
	static void ParseIntNumberN(Json::Value& _root, OUT int* _pNumArr, int _count);

	static void ParseFloatNumber2(Json::Value& _root, OUT float& _num1, OUT float& _num2);
	static void ParseFloatNumberN(Json::Value& _root, OUT float* _pNumArr, int _count);

	static int GetIntDefault(Json::Value& _value, int _defaultValue = 0);
	static bool GetBooleanDefault(Json::Value& _value, bool _defaultValue = false);
	static float GetFloatDefault(Json::Value& _value, float _defaultValue = 0);
};
