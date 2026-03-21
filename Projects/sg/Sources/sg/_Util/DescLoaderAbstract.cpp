/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:24:37 PM
 * =====================
 *
 */

#include "DescLoaderMgr.h"

#include "sg/_Util/JsonUtil.h"
#include "sg/_Core/AppConfig.h"

#include "filesystem"
#include "fstream"

//////////////////////////////////////////////////////////////////////////////////////////
bool DescLoaderAbstract::DirectoryTree::Init(const jc::String& _rootDirectoryName)
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
bool DescLoaderAbstract::DirectoryTree::Load()
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
void DescLoaderAbstract::DirectoryTree::Clear()
{
	if (root_)
		ClearRecursive(root_);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::DirectoryTree::SetCallback(const char* _directoryName,
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

	pNode->onJsonLoaded_ = jc::Move(_callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::DirectoryTree::SetCallback(const char* _directoryName,
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
void DescLoaderAbstract::DirectoryTree::SetCallbackCommon(DirectoryTreeNodeCallback_t&& _callback)
{
	onJsonLoadedCommon_ = jc::Move(_callback);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::DirectoryTree::SetCallbackCommon(const DirectoryTreeNodeCallback_t& _callback)
{
	onJsonLoadedCommon_ = _callback;
}

//////////////////////////////////////////////////////////////////////////////////////////
DescLoaderAbstract::DirectoryTreeNode* DescLoaderAbstract::DirectoryTree::FindNodeRecursive(
	DirectoryTreeNode* _pNode, const char* _pDirectoryName)
{
	jc_assert(_pNode);

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
void DescLoaderAbstract::DirectoryTree::ConstructTreeRecursive(
	OUT DirectoryTreeNode** _ppNode, const char* _path, int _depth)
{
	const int directoryCount = jc::Directory::DirectoryCount(_path);
	const jc::String directoryName = jc::Path::FileNameLevel(_path, _depth);
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
void DescLoaderAbstract::DirectoryTree::LoadRecursive(DirectoryTree* _pTree, DirectoryTreeNode* _pNode)
{
	const jc::String& srcDataPath = g_cAppConfig.srcDataPath_;
	const jc::String directoryFullPath = jc::Path::Combine(srcDataPath, _pNode->directoryName_);

	for (const std::filesystem::directory_entry& directoryEntry : std::filesystem::directory_iterator(directoryFullPath.Source()))
	{
		if (directoryEntry.is_regular_file())
		{
			Json::Value root;
			jc::String fileName = directoryEntry.path().string().c_str();

			if (!JsonUtil::Load(fileName.Source(), root))
			{
				jc::String fName = jc::Path::FileName(fileName.Source());
				_LogDebug_("%s 파일 로딩 실패 (무시함)", fName.SafeSource());
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
void DescLoaderAbstract::DirectoryTree::ClearRecursive(DirectoryTreeNode* _pNode)
{
	for (int i = 0; i < _pNode->children_.Size(); ++i)
		ClearRecursive(_pNode->children_.At(i));

	delete _pNode;
}

//////////////////////////////////////////////////////////////////////////////////////////
DescLoaderAbstract::DescLoaderAbstract()
{
}

//////////////////////////////////////////////////////////////////////////////////////////
DescLoaderAbstract::~DescLoaderAbstract()
{
	configDataMap_.ForEachValue([](SDescBase* _pData)
	{
		delete _pData;
	});
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::LoadDepedencies()
{
	for (ConfigFileType_t dependency : GetDependencies())
	{
		g_cDescMgr.Load(dependency);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
bool DescLoaderAbstract::LoadJson(OUT Json::Value& _root)
{
	jc::String configPath = GetConfigFileName();
	if (UseSrcDataPath())
	{
		configPath = jc::Path::Combine(g_cAppConfig.srcDataPath_, GetConfigFileName());
	}

	if (jc::Path::Extension(configPath) != ".json")
	{
		_LogWarn_("%s 설정파일은 json 형식의 파일이 아닙니다.", GetConfigFileName());
		return false;
	}

	std::ifstream reader(configPath.Source(), std::ifstream::in | std::ifstream::binary);
	jc_assert_msg(reader.is_open(), "%s 파일을 여는데 실패했습니다.", GetConfigFileName());

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
bool DescLoaderAbstract::LoadDirectory(OUT DirectoryTree& _directoryTree)
{
	const jc::String& srcDataPath = g_cAppConfig.srcDataPath_;
	const jc::String directoryPath = jc::Path::Combine(srcDataPath, GetConfigFileName());

	if (!jc::Directory::Exist(directoryPath))
	{
		_LogWarn_("%s 디렉토리를 찾지 못했습니다.", directoryPath.Source());
		return false;
	}

	return _directoryTree.Init(directoryPath);
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::Unload()
{
	configDataMap_.ForEachValueDelete();
	configDataMap_.Clear();
	Unloaded();
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::Loaded()
{
	isLoaded_ = true;
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::Unloaded()
{
	isLoaded_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
void DescLoaderAbstract::AddData(SDescBase* _pData)
{
	if (!configDataMap_.Insert(_pData->code_, _pData))
	{
		jc_assert_msg(false, "%s 파일에서 이미 %d번 데이터를 읽은 후 입력하였습니다.", GetConfigFileName(), _pData->code_);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SDescBase* DescLoaderAbstract::GetData(int _code)
{
	jc_assert_msg(configDataMap_.Exist(_code), "%s 파일에서 읽은 데이터중 %d의 코드에 해당하는 데이터는 존재하지 않습니다.", GetConfigFileName(), _code);
	return configDataMap_[_code];
}

//////////////////////////////////////////////////////////////////////////////////////////
SDescBase* DescLoaderAbstract::GetDataAny()
{
	SDescBase** pFound = configDataMap_.AnyValue();
	if (pFound == nullptr) return nullptr;
	return *pFound;
}
