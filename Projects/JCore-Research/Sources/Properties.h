/*
 * 작성자: 윤정도
 * 생성일: 8/28/2023 10:01:06 PM
 * =====================
 *
 */


#pragma once

#include "Property.h"

class Properties {
	using TKey = int;
	using TPropertyMap = HashMap<TKey, PropertyBase*>;
	using TNameMap = HashMap<TKey, const char*>;
public:
	Properties(int capacity = 16) : m_hProperties(capacity) {}
	~Properties() { m_hProperties.ForEachValueDelete(); }

	void AddProperty(TKey propertyKey, PropertyType_t type);
	PropertyBase* GetProperty(TKey property);
	PropertyBase& operator[](TKey propertyKey);
	int Count() { return m_hProperties.Size(); }

	static void InitializeNameMap();
	static const char* GetPropertyName(TKey key);
private:
	TPropertyMap m_hProperties;

	inline static TNameMap PropertyNameMap;
};

