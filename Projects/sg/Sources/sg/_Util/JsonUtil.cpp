/*
 * 작성자: 윤정도
 * 생성일: 1/20/2023 3:56:56 PM
 * =====================
 *
 */

#include "Core.h"
#include "JsonUtil.h"

#include <sg/_Util/TextUtil.h>

USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
bool JsonUtil::Load(const char* _pFileName, OUT Json::Value& _root)
{
	if (jc::Path::Extension(_pFileName) != ".json")
		return false;

	std::ifstream reader(_pFileName, std::ifstream::in | std::ifstream::binary);
	jc_assert_msg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", _pFileName);
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
bool JsonUtil::Load(const jc::String& _fileName, OUT Json::Value& _root)
{
	return Load(_fileName.Source(), _root);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::LoadThrow(const char* _pFileName, Json::Value& _root)
{
	std::ifstream reader(_pFileName, std::ifstream::in | std::ifstream::binary);
	jc_assert_msg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", _pFileName);
	reader >> _root;
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::LoadThrow(const jc::String& _fileName, Json::Value& _root)
{
	LoadThrow(_fileName.Source(), _root);
}


//////////////////////////////////////////////////////////////////////////////////////////
jc::String JsonUtil::GetString(Json::Value& _value)
{
	jc_assert_msg(!_value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	_value.getString(&pBegin, &pEnd);
	const int length = pEnd - pBegin;
	return { pBegin, length + 1 };
}

//////////////////////////////////////////////////////////////////////////////////////////
const char* JsonUtil::GetStringRaw(Json::Value& _value, OUT_OPT int* _pLength /* = nullptr */)
{
	jc_assert_msg(!_value.isNull(), "인자로 전달한 Json 오브젝트에 문자열 데이터가 없습니다.");
	const char* pBegin;
	const char* pEnd;
	_value.getString(&pBegin, &pEnd);
	if (_pLength != nullptr)
		*_pLength = pEnd - pBegin;
	return pBegin;
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String JsonUtil::GetStringOrDefault(Json::Value& _value, const jc::String& _defaultValue)
{
	if (_value.empty())
		return _defaultValue;

	return GetString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
jc::String JsonUtil::GetStringOrNull(Json::Value& _value)
{
	if (_value.empty())
		return { 0 };

	return GetString(_value);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseThicknessInfo(Json::Value& _thicknessRoot, OUT ThicknessBox& _box)
{
	int num[4];
	TextUtil::ParseIntNumbers(GetString(_thicknessRoot), num, 4);

	_box.relativeX_ = (float)num[0];
	_box.relativeY_ = (float)num[1];
	_box.width_ = (float)num[2];
	_box.height_ = (float)num[3];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber2(Json::Value& _root, int& _num1, int& _num2)
{
	int num[2];
	TextUtil::ParseIntNumbers(GetString(_root), num, 2);

	_num1 = num[0];
	_num2 = num[1];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber3(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3)
{
	int num[3];
	TextUtil::ParseIntNumbers(GetString(_root), num, 3);

	_num1 = num[0];
	_num2 = num[1];
	_num3 = num[2];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumber4(Json::Value& _root, OUT int& _num1, OUT int& _num2, OUT int& _num3,
                               OUT int& _num4)
{
	int num[4];
	TextUtil::ParseIntNumbers(GetString(_root), num, 4);

	_num1 = num[0];
	_num2 = num[1];
	_num3 = num[2];
	_num4 = num[3];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseIntNumberN(Json::Value& _root, OUT int* _pNumArr, int _count)
{
	TextUtil::ParseIntNumbers(GetString(_root), _pNumArr, _count);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseFloatNumber2(Json::Value& _root, OUT float& _num1, OUT float& _num2)
{
	float num[2];
	TextUtil::ParseFloatNumbers(GetString(_root), num, 2);
	_num1 = num[0];
	_num2 = num[1];
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseFloatNumberN(Json::Value& _root, OUT float* _pNumArr, int _count)
{
	TextUtil::ParseFloatNumbers(GetString(_root), _pNumArr, _count);
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

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseAnimationInfo(Json::Value& _animationRoot, AnimationInfo& _info)
{
	_info.name_ = GetStringOrNull(_animationRoot["name"]);
	_info.code_ = GetIntDefault(_animationRoot["code"], 1);
	_info.loop_ = GetBooleanDefault(_animationRoot["loop"]);

	if (!_info.name_.IsNull() && _info.name_ == "sliding")
	{
		int a = 40;
	}

	Json::Value& frameListRoot = _animationRoot["frames"];
	for (size_t i = 0; i < frameListRoot.size(); ++i)
	{
		Json::Value& frameRoot = frameListRoot[(Json::ArrayIndex)i];

		int frameIndex; // 필수
		int delay; // 필수
		int frameEventId = InvalidValue_v;

		int frameLength;
		const char* pFrame = GetStringRaw(frameRoot, &frameLength);
		TextUtil::ParseFrameInfo(pFrame, frameLength, frameIndex, delay, frameEventId);
		_info.frames_.EmplaceBack(frameIndex, (float)delay / 1000.0f, frameEventId);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseActorRect(Json::Value& _root, ActorRect& _actorRect)
{
	int actorRectData[8];
	ParseIntNumberN(_root, actorRectData, sizeof(actorRectData) / sizeof(int));

	_actorRect.thicknessRect_.origin.x = actorRectData[0];
	_actorRect.thicknessRect_.origin.y = actorRectData[1];
	_actorRect.thicknessRect_.size.width = actorRectData[2];
	_actorRect.thicknessRect_.size.height = actorRectData[3];

	_actorRect.bodyRect_.origin.x = actorRectData[4];
	_actorRect.bodyRect_.origin.y = actorRectData[5];
	_actorRect.bodyRect_.size.width = actorRectData[6];
	_actorRect.bodyRect_.size.height = actorRectData[7];
}

/**
 * TODO: ActorSpriteData 기능 개선 필요 (추후 좀더 확장성있게 개발할려면 필수적으로 수행되어야함.)
 * 현재 ActorSpriteData는 캐릭터, 몬스터의 애니메이션 재생 구현에 초점을 두고 기능을 구현해서.
 * 모든 파츠마다 동일한 애니메이션을 실행한다고 가정하여 구현하였기 때문에
 * 액터의 각 파츠가 다른 애니메이션으로 구성된 경우에 대해서 처리하지 못한다.
 *
 * runAnimation을 ActorSprite에서 처리하도록 하고 있는데.. 이걸 빼야하나.. ㅠㅠ
 * 우선 다 만들고 고민하는걸로..
 */

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseColor4B(Json::Value& _root, OUT cc::Color4B& _color)
{
	int r;
	int g;
	int b;
	int a;

	ParseIntNumber4(_root, r, g, b, a);
	jc_assert_msg(r >= 0 && r <= MaxByte_v, "R 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", r, MaxByte_v);
	jc_assert_msg(g >= 0 && g <= MaxByte_v, "G 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", g, MaxByte_v);
	jc_assert_msg(b >= 0 && b <= MaxByte_v, "B 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", b, MaxByte_v);
	jc_assert_msg(a >= 0 && a <= MaxByte_v, "A 색상값(%d)이 0 ~ %d사이 값이 아닙니다.", a, MaxByte_v);

	_color.r = (unsigned char)r;
	_color.g = (unsigned char)g;
	_color.b = (unsigned char)b;
	_color.a = (unsigned char)a;
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseSize(Json::Value& _root, cc::size& _size)
{
	ParseFloatNumber2(_root, _size.width, _size.height);
}

//////////////////////////////////////////////////////////////////////////////////////////
void JsonUtil::ParseVec2(Json::Value& _root, cc::vec2& _vec)
{
	ParseFloatNumber2(_root, _vec.x, _vec.y);
}
