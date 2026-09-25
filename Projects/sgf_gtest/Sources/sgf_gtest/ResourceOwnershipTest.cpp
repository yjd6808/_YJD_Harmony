#include "sgf/Scene/GameObject.h"
#include "sgf/Graphics/Material.h"
#include "sgf/Graphics/ResourceMgr.h"

#if TEST_ResourceOwnershipTest == ON

TEST(ResourceOwnershipTest, DefaultHasNoResourceKeys)
{
	sgf::GameObject object;
	EXPECT_EQ(object.GetMeshKey(), sgf::INVALID_RESOURCE_KEY);
	EXPECT_EQ(object.GetMaterialKey(), sgf::INVALID_RESOURCE_KEY);
	EXPECT_EQ(object.GetMesh(), nullptr);
	EXPECT_EQ(object.GetMaterial(), nullptr);
}

TEST(ResourceOwnershipTest, InvalidKeyStaysNullWithoutCrash)
{
	sgf::GameObject object;
	object.SetMeshKey(sgf::INVALID_RESOURCE_KEY);
	object.SetMaterialKey(sgf::INVALID_RESOURCE_KEY);
	EXPECT_EQ(object.GetMesh(), nullptr);
	EXPECT_EQ(object.GetMaterial(), nullptr);
}

TEST(ResourceOwnershipTest, StaleKeyResolvesToNull)
{
	sgf::GameObject object;
	const _u64 staleMeshKey = sgf::MakeResourceKey(sgf::ResourceType::rtMesh, 1, 0);
	const _u64 staleMaterialKey = sgf::MakeResourceKey(sgf::ResourceType::rtMaterial, 1, 0);
	object.SetMeshKey(staleMeshKey);
	object.SetMaterialKey(staleMaterialKey);
	EXPECT_EQ(object.GetMesh(), nullptr);
	EXPECT_EQ(object.GetMaterial(), nullptr);
}

TEST(ResourceOwnershipTest, MaterialDescEqualityAndHash)
{
	sgf::MaterialDesc left;
	sgf::MaterialDesc right;
	EXPECT_TRUE(left == right);
	EXPECT_FALSE(left != right);
	EXPECT_EQ(left.Hash(), right.Hash());

	right.baseColor_ = jc::color::RED;
	EXPECT_TRUE(left != right);
	EXPECT_FALSE(left == right);
}

#endif
