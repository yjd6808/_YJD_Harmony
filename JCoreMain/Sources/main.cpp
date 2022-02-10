#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model(int da) { a = da; cout <<("¸ðµ¨ 1È£ »ý¼º\n"); }
	virtual ~Model() { cout << ("¸ðµ¨ 1È£ ¼Ò¸ê\n"); }

	int a = 3;
	int b = 3;
};

struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("½´ÆÛ ¸ðµ¨ ¼Ò¸ê\n"); }
};


template <typename T>
struct ModelTemplate
{
	ModelTemplate(int da) { a = da; cout << ("¸ðµ¨ 1È£ »ý¼º\n"); }
	virtual ~ModelTemplate() { cout << ("¸ðµ¨ 1È£ ¼Ò¸ê\n"); }

	T a = 3;
	T b = 3;
};

template <typename T>
struct SuperModelTempalte : ModelTemplate<T>
{
	SuperModelTempalte() : ModelTemplate<T>(1) {}
	~SuperModelTempalte() override { cout << ("½´ÆÛ ¸ðµ¨ ¼Ò¸ê\n"); }
};

int main() {

	std::vector<int> v { 1, 2, 3};

	auto it = v.begin();
	auto z1 = ++it;
	auto z2 = it++;

	/*
	{
		UniquePointer<Model> f = MakeUnique<SuperModel>();
		(*f).a = 30;
		Model* z = f.Get<Model*>();
		SuperModel* e = f.Get<SuperModel*>();
	}
	{
		SharedPointer<Model> f = MakeShared<SuperModel>();
		(*f).a = 30;
		Model* z = f.Get<Model*>();
		SuperModel* e = f.Get<SuperModel*>();
	}

	{
		UniquePointer<ModelTemplate<int>> f = MakeUnique<SuperModelTempalte<int>>();
		(*f).b = 30;
		ModelTemplate<int>* z = f.Get<ModelTemplate<int>*>();
		SuperModelTempalte<int>* e = f.Get<SuperModelTempalte<int>*>();
	}
	{
		SharedPointer<Model> f = MakeShared<SuperModel>();
		(*f).a = 30;
		Model* z = f.Get<Model*>();
		SuperModel* e = f.Get<SuperModel*>();
	}

	*/
	

	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

