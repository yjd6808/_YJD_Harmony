#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model(int da) { a = da; cout <<("모델 1호 생성\n"); }
	virtual ~Model() { cout << ("모델 1호 소멸\n"); }

	int a = 3;
	int b = 3;
};


struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("슈퍼 모델 소멸\n"); }
};


int main() {
	// C++ 라이브러리의 스마트포인터는 다이나믹 캐스팅을 지원한다.
	//shared_ptr<Model> p = std::make_shared<SuperModel>();
	//shared_ptr<SuperModel> d = dynamic_pointer_cast<SuperModel>(p);


	int* c = new int(3);
	int* p = dynamic_cast<short*>(c);

	{
		SharedPointer<SuperModel> model = MakeShared<SuperModel>();
		SharedPointer<Model> model1 = MakeShared<SuperModel[]>(5);
		SharedPointer<Model> model2 = MakeShared<SuperModel[30]>();
		WeakPointer<SuperModel> model3 = model;
	}


	cout << "===\n";
	{
		SharedPointer<Model[]> modelArray1 = MakeShared<SuperModel[]>(3); 
		SharedPointer<Model> _1			= modelArray1;		// 배열 형태는 신경쓰지 않고 서로 사용할 수 있다.
		SharedPointer<Model[10]> _2		= _1;	
		SharedPointer<Model[]> _3		= _2;
		SharedPointer<SuperModel> _4	= _3;
		SharedPointer<SuperModel[]> _5  = _4;

	}
	cout << "===\n";
	{
		//SharedPointer<Model[3]> modelArray2 = MakeShared<SuperModel[3]>();
	}
	

	_CrtDumpMemoryLeaks();
	return 0;
}


		

		
		

