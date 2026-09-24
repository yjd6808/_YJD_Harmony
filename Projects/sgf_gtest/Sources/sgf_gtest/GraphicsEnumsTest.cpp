#include "sgf/Graphics/GraphicsEnums.h"

#if TEST_GraphicsEnumsTest == ON

TEST(GraphicsEnumsTest, ResourceUsageMapping)
{
	EXPECT_TRUE(sgf::ToD3D11(sgf::ResourceUsage::ruDefault) == D3D11_USAGE_DEFAULT);
	EXPECT_TRUE(sgf::ToD3D11(sgf::ResourceUsage::ruImmutable) == D3D11_USAGE_IMMUTABLE);
	EXPECT_TRUE(sgf::ToD3D11(sgf::ResourceUsage::ruDynamic) == D3D11_USAGE_DYNAMIC);
	EXPECT_TRUE(sgf::ToD3D11(sgf::ResourceUsage::ruStaging) == D3D11_USAGE_STAGING);
}

TEST(GraphicsEnumsTest, RasterizerMapping)
{
	EXPECT_TRUE(sgf::ToD3D11(sgf::CullMode::cmNone) == D3D11_CULL_NONE);
	EXPECT_TRUE(sgf::ToD3D11(sgf::CullMode::cmFront) == D3D11_CULL_FRONT);
	EXPECT_TRUE(sgf::ToD3D11(sgf::CullMode::cmBack) == D3D11_CULL_BACK);
	EXPECT_TRUE(sgf::ToD3D11(sgf::FillMode::fmSolid) == D3D11_FILL_SOLID);
	EXPECT_TRUE(sgf::ToD3D11(sgf::FillMode::fmWireframe) == D3D11_FILL_WIREFRAME);
}

TEST(GraphicsEnumsTest, SamplerMapping)
{
	EXPECT_TRUE(sgf::ToD3D11(sgf::FilterMode::fmPoint) == D3D11_FILTER_MIN_MAG_MIP_POINT);
	EXPECT_TRUE(sgf::ToD3D11(sgf::FilterMode::fmLinear) == D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	EXPECT_TRUE(sgf::ToD3D11(sgf::FilterMode::fmAnisotropic) == D3D11_FILTER_ANISOTROPIC);
	EXPECT_TRUE(sgf::ToD3D11(sgf::AddressMode::amWrap) == D3D11_TEXTURE_ADDRESS_WRAP);
	EXPECT_TRUE(sgf::ToD3D11(sgf::AddressMode::amMirror) == D3D11_TEXTURE_ADDRESS_MIRROR);
	EXPECT_TRUE(sgf::ToD3D11(sgf::AddressMode::amClamp) == D3D11_TEXTURE_ADDRESS_CLAMP);
	EXPECT_TRUE(sgf::ToD3D11(sgf::AddressMode::amBorder) == D3D11_TEXTURE_ADDRESS_BORDER);
}

TEST(GraphicsEnumsTest, TopologyAndFormatMapping)
{
	EXPECT_TRUE(sgf::ToD3D11(sgf::PrimitiveTopology::ptPointList) == D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PrimitiveTopology::ptLineList) == D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PrimitiveTopology::ptTriangleList) == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PrimitiveTopology::ptTriangleStrip) == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PixelFormat::pfRgba8) == DXGI_FORMAT_R8G8B8A8_UNORM);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PixelFormat::pfDepth24Stencil8) == DXGI_FORMAT_D24_UNORM_S8_UINT);
	EXPECT_TRUE(sgf::ToD3D11(sgf::PixelFormat::pfDepth32) == DXGI_FORMAT_D32_FLOAT);
}

#endif
