#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model() { }
	Model(int da) { a = da; cout << StringUtil::Format("모델 %d호 생성\n", da); }
	virtual ~Model() { cout << StringUtil::Format("모델 %d호 소멸\n", a); }

	int a = 3;
	int b = 3;
};

struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("슈퍼 모델 소멸\n"); }
};


template <typename T>
struct ModelTemplate
{
	ModelTemplate(int da) { a = da; cout << ("모델 1호 생성\n"); }
	virtual ~ModelTemplate() { cout << ("모델 1호 소멸\n"); }

	T a = 3;
	T b = 3;
};

template <typename T>
struct SuperModelTempalte : ModelTemplate<T>
{
	SuperModelTempalte() : ModelTemplate<T>(1) {}
	~SuperModelTempalte() override { cout << ("슈퍼 모델 소멸\n"); }
};


class Test
{
public:
	Test() : owner(this, true) {}
	virtual ~Test() { owner.~VoidOwner(); }

	
	union { VoidOwner owner; };
protected:
	virtual void A() {
		cout << "a\n";
	}
protected:
	int a;
};


class Derived : public Test
{
public:
	Derived() : Test() {}
	virtual ~Derived() {  }
public:
	void A() override {
		Test::A();
		cout << "derived\n";
	}
};

class Legendary : public Derived
{
	void A() override {
	}
};


struct Zizon
{
	Zizon(){}
	~Zizon() {}
	union { Model model; };
};


struct A
{/*
	A() { cout << "A 디폴트 생성자\n"; }
	A(int a) : a(a) { cout << "A 일반 생성자\n"; }
	A(const A&) { cout << "A 복사 생성자\n"; }
	A(A&&) { cout << "A 이동 생성자\n"; }
	~A() { cout << "A 소멸자\n"; }

	A& operator=(const A&) = default;
		A& operator=(A&&) = default;
*/
	int a;
};

struct B
{
	A le;
	A le2;
};
#include <JCore/Container/HashMap.h>

HashMap<int, int> map;

struct FT
{
	bool operator() (const Model& m) {
		return m.a == 0;
	}
};

struct FT2
{
	void operator() (int& m) {
		cout << m << "\n";
	}
};

template <typename Fn>
bool Tefasefesast(const Model& m, Fn fn) {
	return fn(m);
}

void fasfesaf(int& m, std::function<void(int&)> fn) {
	fn(m);
}


int main() {
	MemoryLeakDetector detector;

	HashMap<int, Model> modelMap;

	// Insert 테스트
	// Key, Value 개별삽입
	(modelMap.Insert(1, Model(3)));
	(modelMap.Insert(1, Model(3)));	// 중복된 키값 삽입 실패
	(modelMap.Insert(2, Model(4)));
	(modelMap.Insert(3, Model(5)));
	(modelMap.Insert(4, Model(6)));

	// KeyValuePair로 삽입
	(modelMap.Insert(MakePair(5, Model(7))));
	(modelMap.Insert(MakePair(6, Model(8))));

	(modelMap.Size() == 6);

	// Remove 테스트
	(modelMap.Remove(2));	// 삭제 성공
	(modelMap.Remove(2));	// 삭제 실패해야함
	(modelMap.Size() == 5);

	// operator[] 테스트
	//(modelMap[2]); // 키값이 2에 해당하는 데이터는 삭제했으므로 예외를 던짐
	(modelMap[3].a == 5);

	// Clear 테스트
	modelMap.Clear();
	(modelMap.Size() == 0);
	(modelMap.Insert(2, Model(4)));	// 삭제 후 데이터 정상 삽입 되는지
	(modelMap.Insert(3, Model(5)));
	(modelMap.Insert(4, Model(6)));
	(modelMap.Size() == 3);

	auto z = modelMap.Keys().Begin();
	while (z->HasNext()) {
		cout << z->Next() << "\n";
	}

	auto z2 = modelMap.Values().Begin();
	while (z2->HasNext()) {
		cout << z2->Next().a << "\n";
	}
	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		



