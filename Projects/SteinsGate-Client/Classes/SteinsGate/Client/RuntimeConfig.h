/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 8:13:09 AM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/RuntimeConfigBase.h>

struct RuntimeConfig
	: RuntimeConfigBase
	, SGSingletonPointer<RuntimeConfig>
{
private:
	friend class TSingleton;
	RuntimeConfig();
	~RuntimeConfig() override;
public:
	void OnLoading(Json::Value& root) override;
	void OnSaving(Json::Value& root) override;

	void Read(Json::Value& root);
	void Write(Json::Value& root);
};

