/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:19:45 PM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsCommon.Extension
{
    public class FileEx
    {
        // https://stackoverflow.com/questions/1395205/better-way-to-check-if-a-path-is-a-file-or-a-directory
        public static bool IsFile(string path)
        {
            FileAttributes attr = File.GetAttributes(path);

            if ((attr & FileAttributes.Directory) != FileAttributes.Directory)
                return true;

            return false;
        }
    }
}

