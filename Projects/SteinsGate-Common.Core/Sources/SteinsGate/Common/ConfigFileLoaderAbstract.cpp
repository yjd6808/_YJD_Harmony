/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:24:37 PM
 * =====================
 *
 */

#include "Core.h"
#include "DataManagerAbstract.h"
#include "CommonCoreHeader.h"

#include <SteinsGate/Common/JsonUtil.h>

#include <filesystem>
#include <fstream>

//////////////////////////////////////////////////////////////////////////////////////////
bool ConfigFileLoaderAbstract::DirectoryTree::Init(const SGString& _rootDirectoryName)
{
	try
	{
		ConstructTreeRecursive(&root_, _rootDirectoryName.Source(), 0);
		return true;
	}
	catch (std::exception& exception)
	{
		_LogError_("설정파일 %s을 초기화 하는중 오류가 발생하였습니다. (%s)", loader_->GetConfigFileName(), exception.what());
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConfigFileLoaderAbstract::DirectoryTree::Load()
{
	if (root_ == nullptr)
		return false;

	try
	{
		LoadRecursive(this, root_);
		return true;
	}
	catch (std::exception& exception)
	{
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", loader_->GetConfigFileName(), exception.what());
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::Clear()
{
	if (root_)
		ClearRecursive(root_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::SetCallback(const char* _directoryName,
                                                          DirectoryTreeNodeCallback_t&& _callback)
{
	if (root_ == nullptr)
	{
		_LogWarn_("디렉토리 트리의 Root 노드가 MULL입니다.");
		return;
	}

	DirectoryTreeNode* pNode = FindNodeRecursive(root_, _directoryName);
	if (pNode == nullptr)
	{
		_LogWarn_("디렉토리 트리에서 %s 경로의 디렉토리를 찾지 못했습니다.", _directoryName);
		return;
	}

	pNode->onJsonLoaded_ = JCore::Move(_callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::SetCallback(const char* _directoryName,
                                                          const DirectoryTreeNodeCallback_t& _callback)
{
	if (root_ == nullptr)
	{
		_LogWarn_("디렉토리 트리의 Root 노드가 MULL입니다.");
		return;
	}

	DirectoryTreeNode* pNode = FindNodeRecursive(root_, _directoryName);
	if (pNode == nullptr)
	{
		_LogWarn_("디렉토리 트리에서 %s 경로의 디렉토리를 찾지 못했습니다.", _directoryName);
		return;
	}

	pNode->onJsonLoaded_ = _callback;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::SetCallbackCommon(DirectoryTreeNodeCallback_t&& _callback)
{
	onJsonLoadedCommon_ = JCore::Move(_callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::SetCallbackCommon(const DirectoryTreeNodeCallback_t& _callback)
{
	onJsonLoadedCommon_ = _callback;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigFileLoaderAbstract::DirectoryTreeNode* ConfigFileLoaderAbstract::DirectoryTree::FindNodeRecursive(
	DirectoryTreeNode* _pNode, const char* _pDirectoryName)
{
	DebugAssert(_pNode);

	if (_pNode->directoryName_ == _pDirectoryName)
		return _pNode;

	for (int i = 0; i < _pNode->children_.Size(); ++i)
	{
		DirectoryTreeNode* pFoundNode = FindNodeRecursive(_pNode->children_[i], _pDirectoryName);
		if (pFoundNode != nullptr)
			return pFoundNode;
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::ConstructTreeRecursive(
	OUT DirectoryTreeNode** _ppNode, const char* _path, int _depth)
{
	const int directoryCount = JCore::Directory::DirectoryCount(_path);
	const SGString directoryName = JCore::Path::FileNameLevel(_path, _depth);
	DirectoryTreeNode* pNewNode = dbg_new DirectoryTreeNode{ directoryName, directoryCount };

	for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(_path))
	{
		if (is_directory(directoryEntry))
		{
			DirectoryTreeNode* pChild = nullptr;
			ConstructTreeRecursive(&pChild, directoryEntry.path().string().c_str(), _depth + 1);
			pNewNode->children_.PushBack(pChild);
		}
	}

	*_ppNode = pNewNode;
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::LoadRecursive(DirectoryTree* _pTree, DirectoryTreeNode* _pNode)
{
	const SGString directoryFullPath = JCore::Path::Combine(Core::CommonInfo->configPath_, _pNode->directoryName_);

	for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(
		     directoryFullPath.Source()))
	{
		if (directoryEntry.is_regular_file())
		{
			Json::Value root;
			SGString fileName = directoryEntry.path().string().c_str();

			if (!JsonUtil::Load(fileName.Source(), root))
			{
				_LogDebug_("%s 파일 로딩 실패 (무시함)", JCore::Path::FileName(fileName.Source()));
				continue;
			}

			if (_pNode->onJsonLoaded_)
				_pNode->onJsonLoaded_(root, fileName, _pNode);

			if (_pTree->onJsonLoadedCommon_)
				_pTree->onJsonLoadedCommon_(root, fileName, _pNode);
		}
	}

	for (int i = 0; i < _pNode->children_.Size(); ++i)
		LoadRecursive(_pTree, _pNode->children_[i]);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::DirectoryTree::ClearRecursive(DirectoryTreeNode* _pNode)
{
	for (int i = 0; i < _pNode->children_.Size(); ++i)
		ClearRecursive(_pNode->children_.At(i));

	delete _pNode;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigFileLoaderAbstract::ConfigFileLoaderAbstract(DataManagerAbstract* _pManager)
: configPath_(nullptr)
, pManager_(_pManager)
, configDataMap_()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigFileLoaderAbstract::~ConfigFileLoaderAbstract()
{
	configDataMap_.ForEachValue([](ConfigDataAbstract* pData)
	{
		delete pData;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConfigFileLoaderAbstract::LoadJson(OUT Json::Value& _root)
{
	DebugAssertMsg(Core::CommonInfo != nullptr, "프로그램 실행 후 즉시 CoreCommon_v를 초기화해서 설정/데이터 파일 경로를 확보해주세요. 데이터 매니저 생성시 곧바로 실행파일 경로의 common.json이 로딩되므로 원하는 설정 코드로 세팅하면 됩니다.");

	SGString configPath = JCore::Path::Combine(Core::CommonInfo->configPath_, GetConfigFileName());

	if (JCore::Path::Extension(configPath) != ".json")
	{
		_LogWarn_("%s 설정파일은 json 형식의 파일이 아닙니다.", GetConfigFileName());
		return false;
	}

	std::ifstream reader(configPath.Source(), std::ifstream::in | std::ifstream::binary);
	DebugAssertMsg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", GetConfigFileName());

	try
	{
		reader >> _root;
	}
	catch (std::exception& exception)
	{
		_LogError_("설정파일 %s을 로드하는중 오류가 발생하였습니다. (%s)", GetConfigFileName(), exception.what());
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConfigFileLoaderAbstract::LoadDirectory(OUT DirectoryTree& _directoryTree)
{
	DebugAssertMsg(Core::CommonInfo != nullptr,
	               "프로그램 실행 후 즉시 CoreCommon_v를 초기화해서 설정/데이터 파일 경로를 확보해주세요. 데이터 매니저 생성시 곧바로 실행파일 경로의 common.json이 로딩되므로 원하는 설정 코드로 세팅하면 됩니다.")
	;
	const SGString directoryPath = JCore::Path::Combine(Core::CommonInfo->configPath_, GetConfigFileName());

	if (!JCore::Directory::Exist(directoryPath))
	{
		_LogWarn_("%s 디렉토리를 찾지 못했습니다.", directoryPath.Source());
		return false;
	}

	return _directoryTree.Init(directoryPath);
}

//////////////////////////////////////////////////////////////////////////////////////////
void ConfigFileLoaderAbstract::AddData(ConfigDataAbstract* _pData)
{
	if (!configDataMap_.Insert(_pData->code_, _pData))
	{
		DebugAssertMsg(false, "%s 파일에서 이미 %d번 데이터를 읽은 후 입력하였습니다.", GetConfigFileName(), _pData->code_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
ConfigDataAbstract* ConfigFileLoaderAbstract::GetData(int _code)
{
	DebugAssertMsg(configDataMap_.Exist(_code), "%s 파일에서 읽은 데이터중 %d의 코드에 해당하는 데이터는 존재하지 않습니다.", GetConfigFileName(),
	               _code);
	return configDataMap_[_code];
}
