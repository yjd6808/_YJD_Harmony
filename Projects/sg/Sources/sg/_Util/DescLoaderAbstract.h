/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:10:38 PM
 * =====================
 *
 */

#pragma once

#include <sg/_Util/DescBase.h>

class DescLoaderMgr;
class DescLoaderAbstract
{
protected:
	struct DirectoryTreeNode;
	using DirectoryTreeNodeCallback_t = jc::Action<Json::Value&, const jc::String&, DirectoryTreeNode*>;

	struct DirectoryTreeNode
	{
		DirectoryTreeNode(const jc::String& _directoryName, int _childDirectoryCount)
		: directoryName_(_directoryName)
		, children_(_childDirectoryCount)
		{
		}

		jc::String directoryName_;
		jc::Vector<DirectoryTreeNode*> children_;
		DirectoryTreeNodeCallback_t onJsonLoaded_;
	};

	struct DirectoryTree
	{
		DirectoryTree()
		: root_(nullptr)
		, loader_(nullptr)
		{
		}

		~DirectoryTree() { Clear(); }

		bool Init(const jc::String& _rootDirectoryName);
		bool Load();
		void Clear();

		void SetCallback(const char* _directoryName, DirectoryTreeNodeCallback_t&& _callback);
		void SetCallback(const char* _directoryName, const DirectoryTreeNodeCallback_t& _callback);
		void SetCallbackCommon(DirectoryTreeNodeCallback_t&& _callback);
		auto SetCallbackCommon(const DirectoryTreeNodeCallback_t& _callback) -> void;

		static DirectoryTreeNode* FindNodeRecursive(DirectoryTreeNode* _pNode, const char* _pDirectoryName);
		static void ConstructTreeRecursive(OUT DirectoryTreeNode** _ppNode, const char* _path, int _depth);
		static void LoadRecursive(DirectoryTree* _pTree, DirectoryTreeNode* _pNode);
		static void ClearRecursive(DirectoryTreeNode* _pNode);

		DirectoryTreeNode* root_;
		DescLoaderAbstract* loader_;
		DirectoryTreeNodeCallback_t onJsonLoadedCommon_;
	};

public:
	DescLoaderAbstract(DescLoaderMgr* _pManager);
	virtual ~DescLoaderAbstract();

	virtual void LoadDepedencies();
	virtual bool LoadJson(OUT Json::Value& _root);
	virtual bool LoadDirectory(OUT DirectoryTree& _directoryTree);
	virtual bool Load() = 0;
	virtual void Unload();

	virtual std::initializer_list<ConfigFileType_t> GetDependencies() { return {}; }
	int GetDependenciesCount() { return static_cast<int>(GetDependencies().size()); }

	bool IsLoaded() const { return isLoaded_; }
	void Loaded();
	void Unloaded();

	virtual ConfigFileType_t GetConfigFileType() = 0;
	const char* GetConfigFileName() { return ConfigFileType::FileName[GetConfigFileType()]; }

	void AddData(SDescBase* _pData);
	SDescBase* GetData(int _code);

private:
	bool isLoaded_ = false;

protected:
	jc::String* configPath_{};
	DescLoaderMgr* pManager_;
	jc::HashMap<int, SDescBase*> configDataMap_;
};
