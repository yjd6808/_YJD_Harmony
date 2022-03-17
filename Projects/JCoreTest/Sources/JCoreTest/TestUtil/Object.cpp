#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

Animal::Animal() {
	PrintFormat("¾Ö´Ï¸Ö »ý¼º\n");
}

Animal::~Animal() {
	PrintFormat("¾Ö´Ï¸Ö ¼Ò¸ê\n");
}

Dog::~Dog() {
	PrintFormat("°³ ¼Ò¸ê\n");
}

Bird::~Bird() {
	PrintFormat("³¯°³ ¼Ò¸ê\n");
}

Model::Model() { PrintFormat("¸ðµ¨ µðÆúÆ® »ý¼º\n"); }
Model::Model(int _) : a(_) { PrintFormat("¸ðµ¨ %d »ý¼º\n", a); }
Model::~Model() { PrintFormat("¸ðµ¨ %d ¼Ò¸ê\n", a); }

SuperModel::~SuperModel() { PrintFormat("½´ÆÛ¸ðµ¨ %d ¼Ò¸ê\n", a); }