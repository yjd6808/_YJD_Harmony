/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:24:37 PM
 * =====================
 *
 */


#include "Core.h"
#include "DataManagerAbstract.h"
#include "CommonCoreHeader.h"

#include <filesystem>

bool ConfigFileLoaderAbstract::DirectoryTree::init(const SGString& rootDirectoryName) {
	try {
		constructTreeRecursive(&Root, rootDirectoryName.Source(), 0);
		return true;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 초기화 하는중 오류가 발생하였습니다. (%s)", Loader->getConfigFileName(), ex.what());
		return false;
	}
}

bool ConfigFileLoaderAbstract::DirectoryTree::load() {
	if (Root == nullptr) {
		return false;
	}

	try {
		loadRecursive(this, Root);
		return true;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", Loader->getConfigFileName(), ex.what());
		return false;
	}
	
}

void ConfigFileLoaderAbstract::DirectoryTree::clear() {
	if (Root)
	clearRecursive(Root);
}

void ConfigFileLoaderAbstract::DirectoryTree::setCallback(const char* directoryName, DirectoryTreeNodeCallback_t&& callback) {
	if (Root == nullptr) {
		_LogWarn_("디렉토리 트리의 Root 노드가 MULL입니다.");
		return;
	}

	DirectoryTreeNode* pNode = findNodeRecursive(Root, directoryName);
	if (pNode == nullptr) {
		_LogWarn_("디렉토리 트리에서 %s 경로의 디렉토리를 찾지 못했습니다.", directoryName);
		return;
	}
	pNode->OnJsonLoaded = JCore::Move(callback);
}

void ConfigFileLoaderAbstract::DirectoryTree::setCallback(const char* directoryName, const DirectoryTreeNodeCallback_t& callback) {
	if (Root == nullptr) {
		_LogWarn_("디렉토리 트리의 Root 노드가 MULL입니다.");
		return;
	}

	DirectoryTreeNode* pNode = findNodeRecursive(Root, directoryName);
	if (pNode == nullptr) {
		_LogWarn_("디렉토리 트리에서 %s 경로의 디렉토리를 찾지 못했습니다.");
		return;
	}

	pNode->OnJsonLoaded = callback;
}

void ConfigFileLoaderAbstract::DirectoryTree::setCallbackCommon(DirectoryTreeNodeCallback_t&& callback) {
	OnJsonLoadedCommon = JCore::Move(callback);
}

void ConfigFileLoaderAbstract::DirectoryTree::setCallbackCommon(const DirectoryTreeNodeCallback_t& callback) {
	OnJsonLoadedCommon = callback;
}


ConfigFileLoaderAbstract::DirectoryTreeNode* ConfigFileLoaderAbstract::DirectoryTree::findNodeRecursive(DirectoryTreeNode* node, const char* directoryName) {
	DebugAssert(node);

	if (node->DirectoryName == directoryName) 
		return node;

	for (int i = 0; i < node->Children.Size(); ++i) {

		DirectoryTreeNode* pFindNode = findNodeRecursive(node->Children[i], directoryName);
		if (pFindNode != nullptr) {
			return pFindNode;
		}
	}

	return nullptr;
}

void ConfigFileLoaderAbstract::DirectoryTree::constructTreeRecursive(JCORE_OUT DirectoryTreeNode** node, const char* path, int depth) {

	const int iDirCount = JCore::Directory::DirectoryCount(path);
	const SGString szDirName = JCore::Path::FileNameLevel(path, depth);
	DirectoryTreeNode* pNewNode = dbg_new DirectoryTreeNode{ szDirName , iDirCount };

	for (const std::filesystem::directory_entry& dirEntry : std::filesystem::directory_iterator(path)) {
		if (is_directory(dirEntry)) {
			DirectoryTreeNode* pChild = nullptr;
			constructTreeRecursive(&pChild, dirEntry.path().string().c_str(), depth + 1);
			pNewNode->Children.PushBack(pChild);
		}
	}

	*node = pNewNode;
}

void ConfigFileLoaderAbstract::DirectoryTree::loadRecursive(DirectoryTree* tree, DirectoryTreeNode* node) {
	const SGString szDirectoryFullPath = JCore::Path::Combine(CoreCommonInfo_v->ConfigPath, node->DirectoryName);

	for (const std::filesystem::directory_entry& dirEntry : std::filesystem::directory_iterator(szDirectoryFullPath.Source())) {
		if (dirEntry.is_regular_file()) {

			Json::Value root;
			SGString szFileName = dirEntry.path().string().c_str();

			if (!loadJson(szFileName.Source(), root)) {
				_LogDebug_("%s 파일 로딩 실패 (무시함)", JCore::Path::FileName(szFileName.Source()));
				continue;
			}

			if (node->OnJsonLoaded)
				node->OnJsonLoaded(root, szFileName, node);

			if (tree->OnJsonLoadedCommon)
				tree->OnJsonLoadedCommon(root, szFileName, node);
		}
	}

	for (int i = 0; i < node->Children.Size(); ++i) {
		loadRecursive(tree, node->Children[i]);
	}
}

void ConfigFileLoaderAbstract::DirectoryTree::clearRecursive(DirectoryTreeNode* node) {
	for (int i = 0; i < node->Children.Size(); ++i) {
		clearRecursive(node->Children.At(i));
	}

	delete node;
}

ConfigFileLoaderAbstract::ConfigFileLoaderAbstract(DataManagerAbstract* manager)
	: m_pManager(manager)
{}

ConfigFileLoaderAbstract::~ConfigFileLoaderAbstract() {
	m_hConfigDataAbstract.ForEachValue([](ConfigDataAbstract* data) {
		delete data;
	});
}

bool ConfigFileLoaderAbstract::loadJson(JCORE_OUT Json::Value& root) {
	DebugAssertMsg(CoreCommonInfo_v != nullptr, "프로그램 실행 후 즉시 CoreCommon_v를 초기화해서 설정/데이터 파일 경로를 확보해주세요. 데이터 매니저 생성시 곧바로 실행파일 경로의 common.json이 로딩되므로 원하는 설정 코드로 세팅하면 됩니다.");
	SGString path = JCore::Path::Combine(CoreCommonInfo_v->ConfigPath, getConfigFileName());

	if (JCore::Path::Extension(path) != ".json") {
		_LogWarn_("%s 설정파일은 json 형식의 파일이 아닙니다.", getConfigFileName());
		return false;
	}

	std::ifstream reader(path.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", getConfigFileName());
	try {
		reader >> root;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", getConfigFileName(), ex.what());
		return false;
	}
	return true;
}

bool ConfigFileLoaderAbstract::loadDirectory(JCORE_OUT DirectoryTree& tree) {
	DebugAssertMsg(CoreCommonInfo_v != nullptr, "프로그램 실행 후 즉시 CoreCommon_v를 초기화해서 설정/데이터 파일 경로를 확보해주세요. 데이터 매니저 생성시 곧바로 실행파일 경로의 common.json이 로딩되므로 원하는 설정 코드로 세팅하면 됩니다.");
	const SGString path = JCore::Path::Combine(CoreCommonInfo_v->ConfigPath, getConfigFileName());

	if (!JCore::Directory::Exist(path)) {
		_LogWarn_("%s 디렉토리를 찾지 못했습니다.", path.Source());
		return false;
	}

	return tree.init(path);
}

bool ConfigFileLoaderAbstract::loadJson(const char* fileName, JCORE_OUT Json::Value& root) {

	if (JCore::Path::Extension(fileName) != ".json") {
		return false;
	}

	std::ifstream reader(fileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", fileName);
	try {
		reader >> root;
	} catch (std::exception& ex) {
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", fileName, ex.what());
		return false;
	}
	return true;
}

bool ConfigFileLoaderAbstract::loadJson(const SGString& fileName, JCORE_OUT Json::Value& root) {
	return loadJson(fileName.Source(), root);
}

void ConfigFileLoaderAbstract::loadJsonThrow(const char* fileName, Json::Value& root) {
	std::ifstream reader(fileName, std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", fileName);
	reader >> root;
}

void ConfigFileLoaderAbstract::loadJsonThrow(const SGString& fileName, Json::Value& root) {
	loadJsonThrow(fileName.Source(), root);
}

void ConfigFileLoaderAbstract::addData(ConfigDataAbstract* data) {
	if (m_hConfigDataAbstract.Insert(data->Code, data) == false) {
		DebugAssertMsg(false, "%s 파일에서 이미 %d번 데이터를 읽은 후 입력하였습니다.", getConfigFileName(), data->Code);
	}
}

ConfigDataAbstract* ConfigFileLoaderAbstract::getData(int code) {
	DebugAssertMsg(m_hConfigDataAbstract.Exist(code), "%s 파일에서 읽은 데이터중 %d의 코드에 해당하는 데이터는 존재하지 않습니다.", getConfigFileName(), code);
	return m_hConfigDataAbstract[code];
}

