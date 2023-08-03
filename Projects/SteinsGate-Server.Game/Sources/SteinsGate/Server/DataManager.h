/*
 * 작성자: 윤정도
 * 생성일: 8/3/2023 9:10:29 AM [SteinsGate-Server.Lobby 프로젝트 복사 생성]
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