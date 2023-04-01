/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 4:01:04 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/DataManagerAbstract.h>

class DataManager : public DataManagerAbstract
{
public:
	static DataManager* get() {
		static DataManager* loader;

		if (loader == nullptr) {
			loader = dbg_new DataManager;
			loader->initializeLoader();
		}
		return loader;
	}

	void initializeLoader() override;
};