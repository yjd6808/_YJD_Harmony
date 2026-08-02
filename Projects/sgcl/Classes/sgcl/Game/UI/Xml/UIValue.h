/*
 * 작성자: 윤정도
 * =====================
 * sgui - XML 바인딩 데이터 모델
 * DataList/Data 요소를 파싱해 Window 레벨 데이터를 구성한다.
 * - 스칼라 타입: Bool / Int / Float / String
 * - Int 리터럴: 0b(2진수), 0x(16진수), 10진수
 * - 컨테이너 타입: Array(Key가 정수인 자식 Data 묶음) / Map(이름 Key 자식 Data 묶음)
 * - 멀티라인 텍스트: 자식 <Value> 요소의 텍스트
 */

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace tinyxml2
{
class XMLElement;
}

namespace sgui
{

enum class UIValueType : uint8_t
{
	Null,
	Bool,
	Int,
	Float,
	String,
	Array,
	Map
};

class UIValue
{
public:
	UIValue() = default;

	static UIValue MakeNull();
	static UIValue MakeBool(bool _value);
	static UIValue MakeInt(int64_t _value);
	static UIValue MakeFloat(double _value);
	static UIValue MakeString(const std::string& _value);
	static UIValue MakeArray();
	static UIValue MakeMap();

	UIValueType GetType() const { return type_; }

	bool IsNull() const { return type_ == UIValueType::Null; }
	bool IsBool() const { return type_ == UIValueType::Bool; }
	bool IsInt() const { return type_ == UIValueType::Int; }
	bool IsFloat() const { return type_ == UIValueType::Float; }
	bool IsNumber() const { return type_ == UIValueType::Int || type_ == UIValueType::Float; }
	bool IsString() const { return type_ == UIValueType::String; }
	bool IsArray() const { return type_ == UIValueType::Array; }
	bool IsMap() const { return type_ == UIValueType::Map; }

	// ==================== 캐스팅 변환 ====================
	bool ToBool() const;
	int64_t ToInt() const;
	double ToFloat() const;
	std::string ToString() const;

	// ==================== Array 접근 ====================
	const UIValue* GetArrayItem(int _index) const;
	int GetArraySize() const { return (int)arrayValue_.size(); }

	// ==================== Map 접근 (순서 보존) ====================
	const UIValue* GetMapValue(const char* _key) const;
	int GetMapSize() const { return (int)mapValue_.size(); }
	const std::string& GetMapKeyAt(int _index) const { return mapValue_[_index].first; }

	// 점 체인/인덱스 조회의 최종 단계 (Array는 정수, Map은 문자열 키)
	const UIValue* GetItem(const UIValue& _index) const;

	// ==================== 컨테이너 수정 (파싱용) ====================
	void AddArrayItem(const UIValue& _value);
	void SetArrayItem(int _index, const UIValue& _value);
	void EnsureArraySize(int _size);
	void SetMapValue(const std::string& _key, const UIValue& _value);

private:
	UIValueType type_ = UIValueType::Null;
	bool boolValue_ = false;
	int64_t intValue_ = 0;
	double floatValue_ = 0.0;
	std::string stringValue_;
	std::vector<UIValue> arrayValue_;
	std::vector<std::pair<std::string, UIValue>> mapValue_;
};

// Window 데이터 (DataList). 순서 보존 리스트로 구성된다.
class UIDataList
{
public:
	const UIValue* Find(const char* _key) const;
	UIValue* FindMutable(const char* _key);
	void Set(const std::string& _key, const UIValue& _value);
	int GetSize() const { return (int)values_.size(); }
	const std::string& GetKeyAt(int _index) const { return values_[_index].first; }
	const UIValue* GetValueAt(int _index) const { return &values_[_index].second; }

	std::vector<std::pair<std::string, UIValue>> values_;
};

// <DataList> 요소를 파싱한다. (Key/Type/Value 속성, 0b/0x 리터럴, 멀티라인 <Value>, 중첩 Data)
UIDataList ParseDataList(tinyxml2::XMLElement* _dataListElement);

} // namespace sgui
