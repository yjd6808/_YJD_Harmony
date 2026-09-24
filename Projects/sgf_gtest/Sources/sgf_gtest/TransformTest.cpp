#include "sgf/Scene/Transform.h"

#if TEST_TransformTest == ON

TEST(TransformTest, DefaultWorldIsIdentity)
{
	sgf::Transform t(nullptr);
	const jc::mat4& w = t.GetWorldMatrix();
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
			EXPECT_TRUE(w.m[i][j] == ((i == j) ? 1.0f : 0.0f));
	}
}

TEST(TransformTest, PositionMovesWorldTranslation)
{
	sgf::Transform t(nullptr);
	t.SetLocalPosition(jc::vec3(1.0f, 2.0f, 3.0f));
	const jc::mat4& w = t.GetWorldMatrix();
	EXPECT_TRUE(w.m[3][0] == 1.0f);
	EXPECT_TRUE(w.m[3][1] == 2.0f);
	EXPECT_TRUE(w.m[3][2] == 3.0f);
	EXPECT_TRUE(t.GetWorldPosition() == jc::vec3(1.0f, 2.0f, 3.0f));
}

TEST(TransformTest, ScaleAppliesToPoint)
{
	sgf::Transform t(nullptr);
	t.SetLocalScale(jc::vec3(2.0f, 2.0f, 2.0f));
	const jc::vec3 p = t.GetWorldMatrix().TransformPoint(jc::vec3(1.0f, 0.0f, 0.0f));
	EXPECT_TRUE(p == jc::vec3(2.0f, 0.0f, 0.0f));
}

#endif
