#include "header.h"

int main() {
	Console::SetSize(800, 600);
	Random::EngineInitialize();

	{
		TreeMap<int, int*> s;
		s.Insert(1, new int);
		s.Insert(2, new int);
		s.Insert(3, new int);
		s.ForEachValueDelete();
	}

	{
		HashMap<int, int*> s;
		s.Insert(1, new int);
		s.Insert(2, new int);
		s.Insert(3, new int);
		s.ForEachValueDelete();
	}

	{
		LinkedList<int*> s;
		s.PushBack(new int);
		s.PushBack(new int);
		s.PushBack(new int);
		s.ForEachDelete();
	}

	{
		Vector<int*> s;
		s.PushBack(new int);
		s.PushBack(new int);
		s.PushBack(new int);
		s.ForEachDelete();
	}

	new int;


	return 0;
}

