/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:21:20 PM
 *
 */

using System.IO;

namespace SGToolsCommon.Extension
{
    public class DirectoryEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/1395205/better-way-to-check-if-a-path-is-a-file-or-a-directory
        public static bool IsDirectory(string _path)
        {
            FileAttributes attr = File.GetAttributes(_path);

            if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                return true;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void CreateDirectoryIfNotExist(string _path)
        {
            if (!Directory.Exists(_path))
                Directory.CreateDirectory(_path);
        }
    }
}
