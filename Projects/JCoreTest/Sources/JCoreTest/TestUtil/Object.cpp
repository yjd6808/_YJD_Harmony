#include <JCoreTest/CoreTest.h>
#include <JCoreTest/TestUtil/Object.h>

Animal::Animal() {
	PrintFormat("�ִϸ� ����\n");
}

Animal::~Animal() {
	PrintFormat("�ִϸ� �Ҹ�\n");
}

Dog::~Dog() {
	PrintFormat("�� �Ҹ�\n");
}

Bird::~Bird() {
	PrintFormat("���� �Ҹ�\n");
}

Model::Model() { PrintFormat("�� ����Ʈ ����\n"); }
Model::Model(int _) : a(_) { PrintFormat("�� %d ����\n", a); }
Model::~Model() { PrintFormat("�� %d �Ҹ�\n", a); }

SuperModel::~SuperModel() { PrintFormat("���۸� %d �Ҹ�\n", a); }