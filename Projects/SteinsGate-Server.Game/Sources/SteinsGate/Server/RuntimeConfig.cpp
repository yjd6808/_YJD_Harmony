/*
 * 작성자: 윤정도
 * 생성일: 8/16/2023 7:12:53 AM
 * =====================
 *
 */

#include "Game.h"
#include "GameCoreHeader.h"
#include "RuntimeConfig.h"

USING_NS_JC;
USING_NS_JS;

RuntimeConfig::RuntimeConfig()
{}

RuntimeConfig::~RuntimeConfig() {
	
}

void RuntimeConfig::OnLoading(Json::Value& root) {
	ReadCore(root);
	ReadCommon(root);
	Read(root);
}

void RuntimeConfig::OnSaving(Json::Value& root) {
	WriteCore(root);
	WriteCommon(root);
	Write(root);
}

void RuntimeConfig::Read(Json::Value& root) {

}

void RuntimeConfig::Write(Json::Value& root) {

}
