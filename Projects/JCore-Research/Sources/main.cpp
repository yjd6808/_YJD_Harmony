#include "header.h"

struct NetGroup
{
	virtual ~NetGroup() = default;
};

struct AuthNetGroup : NetGroup
{
	AuthNetGroup(int _1, int _2) {
		sss.Insert(_1, _2);
	}
	~AuthNetGroup() override {
		
	}



	HashMap<int, int> sss;
};

struct BaseCollection
{
	virtual ~BaseCollection() {
		
	}

	HashMap<int, SharedPtr<NetGroup>> Map;
};

class DerivedCollection final : public BaseCollection, public SingletonPointer<DerivedCollection>
{
	friend class TSingleton;
};

int main() {
	auto derived = DerivedCollection::Get();
	derived->Map.Insert(1, MakeShared<AuthNetGroup>(1, 2));
	derived->Map.Insert(2, MakeShared<AuthNetGroup>(1, 3));
	derived->Free();
	
	return 0;
}




