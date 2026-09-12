#include "gtest/gtest.h"
#include "Core.h"
#include "jc/IO/PathResolver.h"
#include "jc/IO/File.h"
using namespace jc;
TEST(PathResolverTest, IsAbsolutePath) { EXPECT_TRUE(PathResolver::IsAbsolutePath(_T("C:/tmp/a.bin"))); EXPECT_TRUE(PathResolver::IsAbsolutePath(_T("/abs"))); EXPECT_FALSE(PathResolver::IsAbsolutePath(_T("theme/dark.json"))); }
TEST(PathResolverTest, MountAndResolveWritable) { PathResolver fs; fs.Mount(_T("re"), _T("test_io/re")); fs.Mount(_T("res"), _T("test_io/res")); auto r1 = fs.ResolveWritable(_T("re:/a")); EXPECT_TRUE(r1.IsOk()); EXPECT_EQ(r1.alias_, String(_T("re"))); auto r2 = fs.ResolveWritable(_T("res://a")); EXPECT_TRUE(r2.IsOk()); EXPECT_EQ(r2.alias_, String(_T("res"))); }
TEST(PathResolverTest, ResolveWithFallback) { PathResolver fs; fs.Mount(_T("asset"), _T("test_io/chain/origin")); fs.AddSearchDir(_T("asset"), _T("test_io/chain/hotfix"), -10); auto w = fs.ResolveWritable(_T("asset:/x.bin")); EXPECT_TRUE(w.IsOk()); EXPECT_TRUE(w.fullPath_.Contain(_T("origin")) || w.fullPath_.Contain(_T("hotfix"))); }
