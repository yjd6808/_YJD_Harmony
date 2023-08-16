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
		DirectoryTreeNode(const SGString& directoryName, int childDirectoryCount)
			: DirectoryName(directoryName)
			, Children(childDirectoryCount)
		{}

		SGString DirectoryName;
		SGVector<DirectoryTreeNode*> Children;
		DirectoryTreeNodeCallback_t OnJsonLoaded;
	};

	struct DirectoryTree
	{
		DirectoryTree()
			: Root(nullptr)
			, Loader(nullptr)
		{}

		~DirectoryTree() { clear(); }

		bool init(const SGString& rootDirectoryName);
		bool load();
		void clear();

		void setCallback(const char* directoryName, DirectoryTreeNodeCallback_t&& callback);
		void setCallback(const char* directoryName, const DirectoryTreeNodeCallback_t& callback);
		void setCallbackCommon(DirectoryTreeNodeCallback_t&& callback);
		void setCallbackCommon(const DirectoryTreeNodeCallback_t& callback);

		static DirectoryTreeNode* findNodeRecursive(DirectoryTreeNode* node, const char* directoryName);
		static void constructTreeRecursive(JCORE_OUT DirectoryTreeNode** node, const char* path, int depth);
		static void loadRecursive(DirectoryTree* tree, DirectoryTreeNode* node);
		static void clearRecursive(DirectoryTreeNode* node);

		DirectoryTreeNode* Root;
		ConfigFileLoaderAbstract* Loader;
		DirectoryTreeNodeCallback_t OnJsonLoadedCommon;
	};


public:
	ConfigFileLoaderAbstract(DataManagerAbstract* manager);
	virtual ~ConfigFileLoaderAbstract();

	virtual bool loadJson(JCORE_OUT Json::Value& root);
	virtual bool loadDirectory(JCORE_OUT DirectoryTree& tree);
	virtual bool load() = 0;

	virtual ConfigFileType_t getConfigFileType() = 0;
	const char* getConfigFileName() { return ConfigFileType::FileName[getConfigFileType()]; }

	void addData(ConfigDataAbstract* data);
	ConfigDataAbstract* getData(int code);

	
protected:
	SGString* m_pConfigPath{};
	DataManagerAbstract* m_pManager;
	SGHashMap<int, ConfigDataAbstract*> m_hConfigDataAbstract;
};


