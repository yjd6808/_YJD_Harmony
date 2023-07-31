/*
 * 작성자: 윤정도
 * 생성일: 1/24/2023 9:27:21 AM
 * =====================
 *
 */

#include "Tutturu.h"
#include "ClientTextInfoLoader.h"

#include <SteinsGate/Client/JsonUtilEx.h>

#define MAX_ID_LEN		63
#define MAX_TEXT_LEN	511

USING_NS_JS;
USING_NS_JC;

ClientTextInfoLoader::ClientTextInfoLoader(DataManagerAbstract* manager)
	: ConfigFileLoaderAbstract(manager)
	, m_TextMap(1024)
{}

bool ClientTextInfoLoader::load() {

	Value root;
	if (!loadJson(root))
		return false;

	try {
		Value clientInfoListRoot = root["text"];

		SGString szId{ MAX_ID_LEN + 1 };
		SGString szText{ MAX_TEXT_LEN + 1 };

		for (int i = 0; i < clientInfoListRoot.size(); ++i) {
			Value& clientRoot = clientInfoListRoot[i];

			szId.SetLength(0);
			szText.SetLength(0);

			if (clientRoot.type() == arrayValue && readClientTextInfo(clientRoot, szId, szText)) {
				m_TextMap.Insert(szId, szText);
			}
		}
	}
	catch (std::exception& ex) {
		_LogError_("%s 파싱중 오류가 발생하였습니다. %s", getConfigFileName(), ex.what());
		return false;
	}

	return true;
}

char* ClientTextInfoLoader::getTextRaw(const char* szId) {
	const SGString* pFind = m_TextMap.Find(szId);
	if (pFind == nullptr) {
		return nullptr;
	}
	return pFind->Source();
}

SGString& ClientTextInfoLoader::getText(const char* szId) {
	SGString* pFind = m_TextMap.Find(szId);
	if (pFind == nullptr) {
		return m_Dummy;
	}
	return *pFind;
}

SGString& ClientTextInfoLoader::getText(const SGString& szId) {
	SGString* pFind = m_TextMap.Find(szId);
	if (pFind == nullptr) {
		return m_Dummy;
	}
	return *pFind;
}

bool ClientTextInfoLoader::readClientTextInfo(Json::Value& clientTextRoot, JCORE_OUT SGString& szId, JCORE_OUT SGString& szText) {
	const int iTextArraySize = (int)clientTextRoot.size();
	if (iTextArraySize < 2) {
		DebugAssert(false);
		return false;
	}

	Value& idRoot = clientTextRoot[0];
	Value& textRoot = clientTextRoot[1];

	if (idRoot.type() != stringValue || textRoot.type() != stringValue) {
		return false;
	}

	int iIdLen;
	int iTextLen;

	const char* szIdRaw = JsonUtil::getStringRaw(idRoot, &iIdLen);
	const char* szTextRaw = JsonUtil::getStringRaw(textRoot, &iTextLen);

	if (iIdLen <= 0 || iIdLen > MAX_ID_LEN ||
		iTextLen <= 0 || iTextLen > MAX_TEXT_LEN) {
		return false;
	}

	szId += szIdRaw;
	szText += szTextRaw;

	return true;
}
