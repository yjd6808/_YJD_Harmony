/*
 * 작성자: 윤정도
 * 생성일: 1/10/2023 11:51:44 PM
 * =====================
 *
 */


#include "SGImagePackManager.h"

#include <SteinsGate/Common/Core/Npk/NpkLoader.h>
#include <SteinsGate/Common/Core/Npk/NpkElementInitializer.h>

#include <JCore/FileSystem/Directory.h>
#include <JCore/FileSystem/Path.h>
#include <JCore/Threading/Thread.h>

#include <SteinsGate/Research/FrameTexture.h>


using namespace cocos2d;
USING_NS_JC;

SGImagePackManager::~SGImagePackManager() {
	for (int i = 0; i < MaxNpkFileCount_v; ++i) {
		DeleteSafe(m_LoadedPackages[i]);
	}
}

void SGImagePackManager::loadAllPackages() {

	NpkElementInitializer::Initialize();

	JCore::Thread loaderThread[MaxNpkParallelLoadingThreadCount_v];
	SGVector<SGString> paths = Directory::Files(DataDirectory_v, false);

	for (int i = 0; i < paths.Size(); ++i) {
		m_PathToIdMap.Insert(Path::FileName(paths[i]), i);
	}

	m_iLoadedPackageCount = paths.Size();
	
	// 8개씩 병렬 로딩 진행
	for (int i = 0; i < MaxNpkParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Start([i, this, &paths](void*) {
			for (int j = i; j < m_iLoadedPackageCount; j += MaxNpkParallelLoadingThreadCount_v) {
				SGString szFileName =  Path::FileName(paths[j]);
				NpkPackagePtr package = NpkLoader::LoadIndexOnly(
					Path::Combine(DataDirectory_v, szFileName)
				);
				m_LoadedPackages[j] = new SGImagePack(package);
				SafeConsole::WriteLine("%d %s 로딩완료", j, szFileName.Source());
			}
		});
	}

	

	for (int i = 0; i < MaxNpkParallelLoadingThreadCount_v; ++i) {
		loaderThread[i].Join();
	}
	SGConsole::WriteLine("모든 NPK 헤더만 로딩 완료");
}

SGImagePack* SGImagePackManager::getPack(const SGString& packPath) {
	DebugAssertMessage(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다.");
	return m_LoadedPackages[m_PathToIdMap[packPath]];
}
SGImagePack* SGImagePackManager::getPack(const int idx) {
	DebugAssertMessage(idx >= 0 && idx < m_iLoadedPackageCount, "올바르지 않은 패키지 인덱스 입니다.");
	return m_LoadedPackages[idx];
}

int SGImagePackManager::getPackIndex(const SGString& packPath) {
	DebugAssertMessage(m_PathToIdMap.Exist(packPath), "해당 패키지가 존재하지 않습니다. (2)");
	return m_PathToIdMap[packPath];
}

