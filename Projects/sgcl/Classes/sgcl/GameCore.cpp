/*
 * 작성자: 윤정도
 * 생성일: 2/18/2023 4:24:01 AM
 * =====================
 *
 */

#include "GameCore.h"

#include "sg/Util/DescLoaderMgr.h"
#include "sg/Core/VirtualMachine.h"
#include "sg/Core/AppConfig.h"

#include "sgcl/API/sgapiClient.h"
#include "sgcl/Net/NetCore.h"
#include "sgcl/Game/Contents/Contents.h"

#include "sgcl/Game/Texture/ImagePackManager.h"
#include "sgcl/Game/Contents/FontManager.h"

#include "sg/Util/DescMgr/DescMgr_Action.h"
#include "sg/Util/DescMgr/DescMgr_AI.h"
#include "sg/Util/DescMgr/DescMgr_AttackData.h"
#include "sg/Util/DescMgr/DescMgr_Channel.h"
#include "sg/Util/DescMgr/DescMgr_CharAnimation.h"
#include "sg/Util/DescMgr/DescMgr_CharCommon.h"
#include "sg/Util/DescMgr/DescMgr_ClientText.h"
#include "sg/Util/DescMgr/DescMgr_FrameEvent.h"
#include "sg/Util/DescMgr/DescMgr_Item.h"
#include "sg/Util/DescMgr/DescMgr_ItemOpt.h"
#include "sg/Util/DescMgr/DescMgr_Map.h"
#include "sg/Util/DescMgr/DescMgr_MapPhysics.h"
#include "sg/Util/DescMgr/DescMgr_ServerInfo.h"

#include "sgcl/Util/DescMgr/DescMgr_Char.h"
#include "sgcl/Util/DescMgr/DescMgr_Effect.h"
#include "sgcl/Util/DescMgr/DescMgr_Tile.h"
#include "sgcl/Util/DescMgr/DescMgr_MapObject.h"
#include "sgcl/Util/DescMgr/DescMgr_Monster.h"
#include "sgcl/Util/DescMgr/DescMgr_Projectile.h"

// ===========================================================
//     슈타인즈 게이트 모든 세계션이 만나는 곳
//     사실.. 소괄호 작성하기 귀찮아서 이렇게 모아놓음
// ===========================================================

NS_SG_BEGIN
::Contents Contents;
NS_END

//////////////////////////////////////////////////////////////////////////////////////////
void InitializeClientCore()
{
	g_cNet.Initialize();
	g_cImagePackMgr.LoadAllPackages();
	g_cFontMgr.Init();

	g_cDescMgr.AddLoader(dbg_new EffectInfoLoader());
	g_cDescMgr.AddLoader(dbg_new MapInfoLoader());
	g_cDescMgr.AddLoader(dbg_new MapPhysicsInfoLoader());
	g_cDescMgr.AddLoader(dbg_new MapObjectInfoLoader());
	g_cDescMgr.AddLoader(dbg_new MonsterInfoLoader());
	g_cDescMgr.AddLoader(dbg_new FrameEventLoader(ActorType::Monster));
	g_cDescMgr.AddLoader(dbg_new ProjectileInfoLoader(ActorType::Monster));
	g_cDescMgr.AddLoader(dbg_new AttackDataInfoLoader(ActorType::Monster));
	g_cDescMgr.AddLoader(dbg_new TileInfoLoader());
	g_cDescMgr.AddLoader(dbg_new ActionInfoLoader());
	g_cDescMgr.AddLoader(dbg_new AIInfoLoader());
	g_cDescMgr.AddLoader(dbg_new AttackDataInfoLoader(ActorType::Character));
	g_cDescMgr.AddLoader(dbg_new ProjectileInfoLoader(ActorType::Character));
	g_cDescMgr.AddLoader(dbg_new ChannelInfoLoader());
	g_cDescMgr.AddLoader(dbg_new CharAnimationInfoLoader());
	g_cDescMgr.AddLoader(dbg_new FrameEventLoader(ActorType::Character));
	g_cDescMgr.AddLoader(dbg_new CharInfoLoader());
	g_cDescMgr.AddLoader(dbg_new ClientTextInfoLoader());
	g_cDescMgr.AddLoader(dbg_new ItemInfoLoader());
	g_cDescMgr.AddLoader(dbg_new ItemOptInfoLoader());
	g_cDescMgr.AddLoader(dbg_new CharCommonInfoLoader());
	g_cDescMgr.LoadAll();
	
	sg::CharCommon = g_cDescMgr.GetCharCommonInfo(1);
}

//////////////////////////////////////////////////////////////////////////////////////////
void FinalizeClientCore()
{
	g_cNet.Free();
	g_cDescMgr.Free();
	g_cFontMgr.Free();
	g_cImagePackMgr.Free();
	
	sg::Contents.Finalize();
	SpriteFrameTexture::FreeDefault();
}
