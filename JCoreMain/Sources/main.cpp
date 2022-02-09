#define _CRT_SECURE_NO_WARNINGS

#include "header.h"


struct Model
{
	Model(int da) { a = da; cout <<("�� 1ȣ ����\n"); }
	virtual ~Model() { cout << ("�� 1ȣ �Ҹ�\n"); }

	int a = 3;
	int b = 3;
};


struct SuperModel : Model
{
	SuperModel() : Model(1) {}
	~SuperModel() override { cout << ("���� �� �Ҹ�\n"); }
};


int main() {
	// C++ ���̺귯���� ����Ʈ�����ʹ� ���̳��� ĳ������ �����Ѵ�.
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
		SharedPointer<Model> _1			= modelArray1;		// �迭 ���´� �Ű澲�� �ʰ� ���� ����� �� �ִ�.
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


		

		
		

