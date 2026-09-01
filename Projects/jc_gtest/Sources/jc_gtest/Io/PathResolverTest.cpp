#include "gtest/gtest.h"
#include "Core.h"
#include "jc/Io/PathResolver.h"
#include "jc/FileSystem/File.h"
using namespace jc;
TEST(PathResolverTest, IsAbsolutePath) { EXPECT_TRUE(PathResolver::IsAbsolutePath("C:/tmp/a.bin")); EXPECT_TRUE(PathResolver::IsAbsolutePath("/abs")); EXPECT_FALSE(PathResolver::IsAbsolutePath("theme/dark.json")); }
TEST(PathResolverTest, MountAndResolveWritable) { PathResolver fs; fs.Mount("re", "test_io/re"); fs.Mount("res", "test_io/res"); auto r1 = fs.ResolveWritable("re:/a"); EXPECT_TRUE(r1.IsOk()); EXPECT_EQ(r1.alias_, String("re")); auto r2 = fs.ResolveWritable("res://a"); EXPECT_TRUE(r2.IsOk()); EXPECT_EQ(r2.alias_, String("res")); }
TEST(PathResolverTest, ResolveWithFallback) { PathResolver fs; fs.Mount("asset", "test_io/chain/origin"); fs.AddSearchDir("asset", "test_io/chain/hotfix", -10); auto w = fs.ResolveWritable("asset:/x.bin"); EXPECT_TRUE(w.IsOk()); EXPECT_TRUE(w.fullPath_.Contain("origin") || w.fullPath_.Contain("hotfix")); }
