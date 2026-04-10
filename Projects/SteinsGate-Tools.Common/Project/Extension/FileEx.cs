/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:19:45 PM
 *
 */

using System.IO;

namespace SGToolsCommon.Extension
{
    public class FileEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/1395205/better-way-to-check-if-a-path-is-a-file-or-a-directory
        public static bool IsFile(string _path)
        {
            FileAttributes attr = File.GetAttributes(_path);

            if ((attr & FileAttributes.Directory) != FileAttributes.Directory)
                return true;

            return false;
        }
    }
}
