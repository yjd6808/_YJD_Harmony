/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:10:38 PM
 * =====================
 *
 */

#pragma once

#include <SteinsGate/Common/ConfigDataAbstract.h>

class DataManagerAbstract;

class ConfigFileLoaderAbstract
{
protected:
	struct DirectoryTreeNode;
	using DirectoryTreeNodeCallback_t = SGActionFn<Json::Value&, const SGString&, DirectoryTreeNode*>;

	struct DirectoryTreeNode
	{
		DirectoryTreeNode(const SGString& _directoryName, int _childDirectoryCount)
		: directoryName_(_directoryName)
		, children_(_childDirectoryCount)
		{
		}

		SGString directoryName_;
		SGVector<DirectoryTreeNode*> children_;
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

		bool Init(const SGString& _rootDirectoryName);
		bool Load();
		void Clear();

		void SetCallback(const char* _directoryName, DirectoryTreeNodeCallback_t&& _callback);
		void SetCallback(const char* _directoryName, const DirectoryTreeNodeCallback_t& _callback);
		void SetCallbackCommon(DirectoryTreeNodeCallback_t&& _callback);
		auto SetCallbackCommon(const DirectoryTreeNodeCallback_t& _callback) -> void;

		static DirectoryTreeNode* FindNodeRecursive(DirectoryTreeNode* _pNode, const char* _pDirectoryName);
		static void ConstructTreeRecursive(JCORE_OUT DirectoryTreeNode** _ppNode, const char* _path, int _depth);
		static void LoadRecursive(DirectoryTree* _pTree, DirectoryTreeNode* _pNode);
		static void ClearRecursive(DirectoryTreeNode* _pNode);

		DirectoryTreeNode* root_;
		ConfigFileLoaderAbstract* loader_;
		DirectoryTreeNodeCallback_t onJsonLoadedCommon_;
	};

public:
	ConfigFileLoaderAbstract(DataManagerAbstract* _pManager);
	virtual ~ConfigFileLoaderAbstract();

	virtual bool LoadJson(JCORE_OUT Json::Value& _root);
	virtual bool LoadDirectory(JCORE_OUT DirectoryTree& _directoryTree);
	virtual bool Load() = 0;

	virtual ConfigFileType_t GetConfigFileType() = 0;
	const char* GetConfigFileName() { return ConfigFileType::FileName[GetConfigFileType()]; }

	void AddData(ConfigDataAbstract* _pData);
	ConfigDataAbstract* GetData(int _code);

protected:
	SGString* configPath_{};
	DataManagerAbstract* pManager_;
	SGHashMap<int, ConfigDataAbstract*> configDataMap_;
};
