/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:01:15 PM
 * =====================
 *
 */


#include "Properties.h"

void Properties::AddProperty(TKey propertyKey, PropertyType_t type) {
	PropertyBase** ppProperty = m_hProperties.Find(propertyKey);
	if (ppProperty != nullptr) {
		_LogWarn_("%s 프로퍼티에 %s타입으로 이미 사용중입니다.", GetPropertyName(propertyKey), PropertyType::Name[(*ppProperty)->GetType()]);
		return;
	}

	PropertyBase* pNewProperty = PropertyStatics::Factorys[type]->CreateInstance();
	m_hProperties.Insert(propertyKey, pNewProperty);
}

PropertyBase* Properties::GetProperty(TKey property) {
	PropertyBase** ppProperty = m_hProperties.Find(property);
	if (ppProperty != nullptr) {
		return nullptr;
	}
	return *ppProperty;
}

PropertyBase& Properties::operator[](TKey propertyKey) {
	PropertyBase** ppProperty = m_hProperties.Find(propertyKey);
	if (ppProperty == nullptr) {
		return Property<int>::Default;
	}
	return **ppProperty;
}

void Properties::InitializeNameMap() {

}

const char* Properties::GetPropertyName(TKey key) {
	const char** ppszName = PropertyNameMap.Find(key);
	if (ppszName == nullptr) {
		return "프로퍼티명 없음";
	}
	return *ppszName;
}
