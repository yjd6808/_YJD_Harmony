#include "header.h"

int main() {
	{
		HashSet<int> s;
		s.Insert(1);
		s.Insert(2);
	}

	{
		TreeSet<int> s;
		s.Insert(1);
		s.Insert(2);
	}

	return 0;
}




