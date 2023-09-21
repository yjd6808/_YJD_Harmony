/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:01:15 PM
 * =====================
 *
 */


#include <JCore/Core.h>
#include <JCore/Container/Properties.h>

NS_JC_BEGIN

void Properties::AddProperty(TKey propertyKey, PropertyType_t type) {
	PropertyBase** ppProperty = m_hProperties.Find(propertyKey);
	if (ppProperty != nullptr) {
		_LogWarn_("%s 프로퍼티에 %s타입으로 이미 사용중입니다.", GetPropertyName(propertyKey), PropertyType::Name[(*ppProperty)->GetType()]);
		return;
	}

	IPropertyFactory* pFactory = PropertyStatics::Factorys[type];
	if (pFactory == nullptr) {
		_LogWarn_("%s 타입의 프로퍼티 팩토리가 없습니다.", PropertyType::Name[type]);
		return;
	}

	PropertyBase* pNewProperty = pFactory->CreateInstance();
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

const char* Properties::GetPropertyName(TKey key) {
	const char** ppszName = PropertyNameMap.Find(key);
	if (ppszName == nullptr) {
		return "프로퍼티명 없음";
	}
	return *ppszName;
}

NS_JC_END