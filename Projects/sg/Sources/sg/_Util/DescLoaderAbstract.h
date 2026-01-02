/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 11:10:38 PM
 * =====================
 *
 */

#pragma once

#include <sg/_Util/DescBase.h>

JC_SENUM_BEGIN(ConfigFileType)
		Effect,
		Begin = Effect,
		Map,
		MapPhysics,
		MapObject,
		Monster,
		Monster_Animation_Frame_Event,
		Monster_Attack_Data,
		Monster_Projectile,
		Monster_Stat,
		Server,
		Tile,
		UI,
		Action,
		AI,
		AttackBox,
		Channel,
		Char_Animation,
		Char_Animation_Frame_Event,
		Char_Attack_Data,
		Char_Base,
		Char_Common,
		Char_Projectile,
		ClientText,
		Item,
		ItemOpt,
		Enchant,
		Database,
		End,
		Max = End
JC_SENUM_MIDDLE(ConfigFileType)

	static constexpr const char* FileName[Max]{
		"effect.json", // 파일
		"map", // 디렉토리
		"map_physics.json", // 파일
		"map_object.json", // 파일
		"monster.json", // 파일
		"monster_animation_frame_event.json", // 파일
		"monster_attack_data.json", // 파일
		"monster_projectile.json", // 파일
		"monster_stat.json", // 파일
		"server.json", // 파일
		"tile.json", // 파일
		"ui.json", // 파일
		"action.json", // 파일
		"ai.json", // 파일
		"attack_box.json", // 파일
		"channel.json", // 파일
		"char_animation.json", // 파일
		"char_animation_frame_event.json", // 파일
		"char_attack_data.json", // 파일
		"char_base.json", // 파일
		"char_common.json", // 파일
		"char_projectile.json", // 파일
		"client_text.json", // 파일
		"item", // 디렉토리
		"item_opt.json", // 파일
		"enchant.json", // 파일
		"database.json" // 파일

	};
JC_SENUM_MIDDLE_END(ConfigFileType)

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
	DescLoaderAbstract();
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
	jc::HashMap<int, SDescBase*> configDataMap_;
};
