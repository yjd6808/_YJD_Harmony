#pragma once

struct Animal
{
	virtual int GetAge() const = 0;

	Animal();
	virtual ~Animal();
};

struct Dog : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Dog();

	int Age = 300;
};

struct Bird : Animal
{
	int GetAge() const override { return Age; };

	virtual ~Bird();

	int Dummy = 0;
	int Age = 400;
};

struct Model
{
	Model();
	Model(int _);
	virtual ~Model();

	int a = 3;
	int b = 3;
};

struct SuperModel : Model
{
	SuperModel(int a) : Model(a) {}
	SuperModel() : Model(0) {}
	~SuperModel();
};


template <typename T>
struct Template
{
	Template() { }
	Template(T _) { a = _; }
	virtual ~Template() {}

	T a = 3;
	T b = 3;
};

template <typename T>
struct ChildTemplate : Template<T>
{
	ChildTemplate() {}
	ChildTemplate(T o) : Template<T>(o) {}
	~ChildTemplate() override {}
};
