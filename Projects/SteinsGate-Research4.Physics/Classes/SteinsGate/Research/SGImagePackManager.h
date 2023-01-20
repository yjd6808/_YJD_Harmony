/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:46:52 PM
 * =====================
 *
 */


#pragma once


#include <SteinsGate/Research/SGImagePack.h>

class SGImagePackManager
{
public:
	static SGImagePackManager* getInstance() {
		static SGImagePackManager* loader;
		if (loader == nullptr) {
			loader = new SGImagePackManager;
			loader->loadAllPackages();
		}
		return loader;
	}

	~SGImagePackManager();

	void loadAllPackages();

	SGImagePack* getPack(const SGString& packPath);
	SGImagePack* getPack(const int idx);
	int getPackIndex(const SGString& packPath);
private:

	// 주기적으로 텍스쳐풀 정리해주는 기능 필요
	SGHashMap<SGString, int> m_PathToIdMap;
	SGImagePack* m_LoadedPackages[MaxNpkFileCount_v]{};
	int m_iLoadedPackageCount{};
};


