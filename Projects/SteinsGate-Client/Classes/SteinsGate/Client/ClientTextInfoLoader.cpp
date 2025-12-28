/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ClientTextInfoLoader.h"

#include <SteinsGate/Client/JsonUtilEx.h>

#define MAX_ID_LEN      63
#define MAX_TEXT_LEN    511

USING_NS_JS;
USING_NS_JC;

//////////////////////////////////////////////////////////////////////////////////////////
ClientTextInfoLoader::ClientTextInfoLoader(DataManagerAbstract* _pManager)
: ConfigFileLoaderAbstract(_pManager)
, textMap_(1024)
{
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientTextInfoLoader::Load()
{
	Value root;
	if (!LoadJson(root))
		return false;

	try
	{
		Value clientInfoListRoot = root["text"];

		SGString idString{ MAX_ID_LEN + 1 };
		SGString textString{ MAX_TEXT_LEN + 1 };

		for (int i = 0; i < clientInfoListRoot.size(); ++i)
		{
			Value& clientRoot = clientInfoListRoot[i];

			idString.SetLength(0);
			textString.SetLength(0);

			if (clientRoot.type() == arrayValue && ReadClientTextInfo(clientRoot, idString, textString))
			{
				textMap_.Insert(idString, textString);
			}
		}
	}
	catch (std::exception& ex)
	{
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", GetConfigFileName(), ex.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientTextInfoLoader::TryGetTextRaw(const char* _id, OUT char** _pText)
{
	const SGString* pFind = textMap_.Find(_id);
	if (pFind == nullptr)
	{
		*_pText = DummyText.Source();
		return false;
	}

	*_pText = pFind->Source();
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientTextInfoLoader::TryGetText(const char* _id, OUT SGString** _pText)
{
	SGString* pFind = textMap_.Find(_id);
	if (pFind == nullptr)
	{
		*_pText = &DummyText;
		return false;
	}

	*_pText = pFind;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientTextInfoLoader::TryGetText(const SGString& _id, OUT SGString** _pText)
{
	SGString* pFind = textMap_.Find(_id);
	if (pFind == nullptr)
	{
		*_pText = &DummyText;
		return false;
	}

	*_pText = pFind;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ClientTextInfoLoader::ReadClientTextInfo(Json::Value& _clientTextRoot, OUT SGString& _id,
                                              OUT SGString& _text)
{
	const int textArraySize = static_cast<int>(_clientTextRoot.size());
	if (textArraySize < 2)
	{
		DebugAssert(false);
		return false;
	}

	Value& idRoot = _clientTextRoot[0];
	Value& textRoot = _clientTextRoot[1];

	if (idRoot.type() != stringValue || textRoot.type() != stringValue)
	{
		return false;
	}

	int idLen;
	int textLen;

	const char* pIdRaw = JsonUtil::GetStringRaw(idRoot, &idLen);
	const char* pTextRaw = JsonUtil::GetStringRaw(textRoot, &textLen);

	if (idLen <= 0 || idLen > MAX_ID_LEN ||
		textLen <= 0 || textLen > MAX_TEXT_LEN)
	{
		return false;
	}

	_id += pIdRaw;
	_text += pTextRaw;

	return true;
}
