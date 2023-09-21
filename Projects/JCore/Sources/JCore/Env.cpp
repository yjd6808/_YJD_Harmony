/*
 * 작성자: 윤정도
 * 생성일: 1/7/2023 10:53:21 PM
 * =====================
 *
 */

#include <JCore/Core.h>
#include <JCore/Env.h>
#include <JCore/Time.h>
#include <JCore/Primitives/String.h>
#include <JCore/Wrapper/WinApi.h>


NS_JC_BEGIN

String Env::CurrentDirectory() {
	char szFileName[MAX_PATH];
	int iLen = WinApi::GetModuleFilePath(NULL, szFileName, MAX_PATH);

	while (szFileName[--iLen] != '\\') {}
	if (iLen > 0) szFileName[iLen] = NULL;
	return szFileName;
}

TimeSpan Env::AppTime() {
	return DateTime::Now().Diff(AppStartUpTime_v);
}

TimeSpan Env::SystemTime() {
	return { Int64(::GetTickCount64()) * 1000 };
}

NS_JC_END