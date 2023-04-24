/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 4:01:04 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Pattern/Singleton.h>
#include <SteinsGate/Common/DataManagerAbstract.h>


class DataManager : public DataManagerAbstract, public JCore::SingletonPointer<DataManager>
{
private:
	friend class TSingleton;
	DataManager();
	~DataManager() override = default;
	void initializeLoader() override;
};