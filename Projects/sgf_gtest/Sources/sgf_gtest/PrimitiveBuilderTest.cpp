#include "sgf/Graphics/PrimitiveBuilder.h"

#if TEST_PrimitiveBuilderTest == ON

TEST(PrimitiveBuilderTest, BuildRectEmitsSingleQuad)
{
	sgf::FillResult out;
	sgf::PrimitiveBuilder::BuildRect(jc::rect(0.0f, 0.0f, 10.0f, 20.0f), jc::color::WHITE, out);
	EXPECT_TRUE(out.vertices_.Size() == 4);
	EXPECT_TRUE(out.indices_.Size() == 6);
	EXPECT_TRUE(out.pTexture_ == nullptr);
}

TEST(PrimitiveBuilderTest, BuildCircleSegmentCounts)
{
	sgf::FillResult out;
	sgf::PrimitiveBuilder::BuildCircle(jc::vec2(0.0f, 0.0f), 5.0f, jc::color::WHITE, 8, out);
	EXPECT_TRUE(out.vertices_.Size() == 10);
	EXPECT_TRUE(out.indices_.Size() == 24);
}

TEST(PrimitiveBuilderTest, BuildTriangleEmitsSingleTriangle)
{
	sgf::FillResult out;
	sgf::PrimitiveBuilder::BuildTriangle(jc::vec2(0.0f, 0.0f), jc::vec2(1.0f, 0.0f), jc::vec2(0.0f, 1.0f), jc::color::WHITE, out);
	EXPECT_TRUE(out.vertices_.Size() == 3);
	EXPECT_TRUE(out.indices_.Size() == 3);
}

#endif
