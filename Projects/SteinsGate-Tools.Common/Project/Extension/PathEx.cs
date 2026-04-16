// jdyun 24/12/21(토)
using System;
using System.IO;

namespace SGToolsCommon.Extension
{
    public static class PathEx
    {
        public static bool IsEndWithDll(this string _path)
        {
            return string.Equals(Path.GetExtension(_path), ".dll", StringComparison.OrdinalIgnoreCase);
        }

        public static bool IsEndWithExe(this string _path)
        {
            return string.Equals(Path.GetExtension(_path), ".exe", StringComparison.OrdinalIgnoreCase);
        }

        public static bool IsEndWithLib(this string _path)
        {
            return string.Equals(Path.GetExtension(_path), ".lib", StringComparison.OrdinalIgnoreCase);
        }

        public static bool IsEndWithPdb(this string _path)
        {
            return string.Equals(Path.GetExtension(_path), ".pdb", StringComparison.OrdinalIgnoreCase);
        }
    }
}
