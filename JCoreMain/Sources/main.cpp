#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <utility>
#include <type_traits>
#include <time.h>
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

#include <JCore/Core.h>
#include <JCore/StringUtil.h>
#include <JCore/String.h>
#include <JCore/TypeTraits.h>
#include <JCore/Type.h>
#include <JCore/Random.h>
#include <JCore/StaticString.h>
#include <JCore/Time.h>
#include <JCore/Container/Arrays.h>

using namespace JCore;
using namespace std;


struct ModelA
{
	ModelA() {
		cout << "model a constructors" << "\n";
	}
	~ModelA() {
		cout << "model a destructor" << "\n";
	}

	int c = 5;
};

struct ModelB
{
	ModelB(int a, int b) {
		c = new int{ a };
		d = b;
		cout << "model constructors" << "\n";
	}
	~ModelB() {
		delete c;
		cout << "model destructor" << "\n";
	}

	void GetA() {

	}

	int* c;
	int d;
};



template <typename T>
class SmartPointer
{
protected:
	template <typename... Args>
	SmartPointer(Args&&... args) {
		m_Pointer = new T(Forward<Args>(args)...);
	}

	~SmartPointer() {
		delete m_Pointer;
	}

	T& operator*() {
		return *m_Pointer;
	}

	T* operator->() {
		return m_Pointer;
	}

	T* m_Pointer;
};


template <typename T>
class UniquePointer : public SmartPointer<T>
{
public:
	template <typename... Args>
	UniquePointer(Args&&... args) : SmartPointer<T>(Forward<Args>(args)...) {}
	virtual ~UniquePointer() {}

	template <typename AnyType>
	void operator=(AnyType&& other) = delete;
};

struct ControlBlock
{
	Int32 ReferenceCount;
	Int32 WeakCount;

	ControlBlock(Int32 refCount, Int32 weakCount) {
		ReferenceCount = refCount;
		WeakCount = weakCount;
	}
};

/*template <typename... Args>
SharedPointer(Args&&... args) : SmartPointer<T>(Forward<Args>(args)...) {}
virtual ~SharedPointer() {
	ReleaseControlBlock();
}*/

/*
class SharedPointer
{
public:
	SharedPointer(int d) {
		m_Pointer = new int{ d };
		m_ControlBlock = new ControlBlock();
	}
	~SharedPointer() {
		delete m_Pointer;
		SubtractReferenceCount();
	}

	int& operator*() {
		return *m_Pointer;
	}

	int* operator->() {
		return m_Pointer;
	}

	int& operator=(int& other) {
		*m_Pointer = other;
		return *m_Pointer;
	}

	SharedPointer& operator=(SharedPointer& other) {
		ChangeTo(other);
		return *this;
	}

private:
	void ChangeTo(SharedPointer& other) {
		SubtractReferenceCount(*this);
		m_Pointer = other.m_Pointer;
		m_ControlBlock = other.m_ControlBlock;
		AddReferenceCount(*this);
	}
	

	static void AddReferenceCount(SharedPointer& shared) {
		shared.m_ControlBlock->ReferenceCount++;
		shared.m_ControlBlock->WeakCount++;
	}

	static void SubtractReferenceCount(SharedPointer& shared) {

		if (shared.m_ControlBlock->ReferenceCount > 0)
			shared.m_ControlBlock->ReferenceCount--;

		if (shared.m_ControlBlock->WeakCount > 0)
			shared.m_ControlBlock->WeakCount--;

		if (shared.m_ControlBlock->ReferenceCount == 0 && shared.m_ControlBlock->WeakCount == 0) {
			DeleteSafe(shared.m_ControlBlock);
			DeleteSafe(shared.m_Pointer);
		}
	}


protected:
	int* m_Pointer;
	ControlBlock* m_ControlBlock;
};

*/

template <typename>
struct WeakPointer;

template <typename T>
class SharedPointer
{
private:
	using TSharedPointer = typename SharedPointer<T>;
	using TWeakPointer = typename WeakPointer<T>;
public:
	template <typename... Args>
	SharedPointer(Args&&... args) {
		
		m_Pointer = new T{Forward<Args>(args)...};
		m_ControlBlock = new ControlBlock(1, 1);
	}

	~SharedPointer() {
		SubtractReferenceCount(*this);
	}

	T& operator*() const {
		return *m_Pointer;
	}

	T* operator->() const {
		return m_Pointer;
	}

	T& operator=(T& other) {
		*m_Pointer = other;
		return *m_Pointer;
	}

	TSharedPointer& operator=(TSharedPointer& other) {
		ChangeTo(other);
		return *this;
	}
private:
	void ChangeTo(TSharedPointer& other) {
		SubtractReferenceCount(*this);
		m_Pointer = other.m_Pointer;
		m_ControlBlock = other.m_ControlBlock;
		AddReferenceCount(*this);
	}


	static void AddReferenceCount(TSharedPointer& shared) {
		shared.m_ControlBlock->WeakCount++;
	}

	static void SubtractReferenceCount(TSharedPointer& shared) {

		if (shared.m_ControlBlock->ReferenceCount > 0)
			shared.m_ControlBlock->ReferenceCount--;

		if (shared.m_ControlBlock->WeakCount > 0)
			shared.m_ControlBlock->WeakCount--;

		if (shared.m_ControlBlock->ReferenceCount == 0 && shared.m_ControlBlock->WeakCount == 0) {
			DeleteSafe(shared.m_ControlBlock);
			DeleteSafe(shared.m_Pointer);
		}
	}
protected:
	T* m_Pointer;
	ControlBlock* m_ControlBlock;
};



template <typename T>
class WeakPointer
{
private:
	using TSharedPointer	= typename SharedPointer<T>;
	using TWeakPointer		= typename WeakPointer<T>;
public:
	WeakPointer(const TSharedPointer& other) {
		m_ControlBlock = other.m_ControlBlock;
		m_Pointer = other.m_Pointer;
		AddWeakCount(*this);
	}

	~WeakPointer() {
		SubtractWeakCount(*this);
	}

	T& operator*() const {
		if (!Exist()) {
			throw std::
		}
		return *m_Pointer;
	}

	T* operator->() const {
		return m_Pointer;
	}

	T& operator=(T& other) {
		*m_Pointer = other;
		return *m_Pointer;
	}

	bool Exist() {
		return m_ControlBlock->ReferenceCount > 0;
	}

	TWeakPointer& operator=(TWeakPointer& other) {
		ChangeTo(other);
		return *this;
	}
private:
	void ChangeTo(TWeakPointer& other) {
		SubtractWeakCount(*this);
		m_Pointer = other.m_Pointer;
		m_ControlBlock = other.m_ControlBlock;
		AddWeakCount(*this);
	}


	static void AddWeakCount(TWeakPointer& shared) {
		shared.m_ControlBlock->WeakCount++;
	}

	static void SubtractWeakCount(TWeakPointer& shared) {
		if (shared.m_ControlBlock->WeakCount > 0)
			shared.m_ControlBlock->WeakCount--;

		if (shared.m_ControlBlock->WeakCount == 0) {
			DeleteSafe(shared.m_ControlBlock);
		}
	}
protected:
	T* m_Pointer;
	ControlBlock* m_ControlBlock;
};




int main() {
	SharedPointer<ModelB> a(1, 2);
	SharedPointer<ModelB> b(1, 3);

	a = b;

	return 0;
}

