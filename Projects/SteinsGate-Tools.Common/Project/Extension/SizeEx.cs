/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 7:31:52 AM
 *
 */

using System;
using System.Collections.Generic;
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
    public static class SizeEx
    {
        public static string ToFullString(this Size size)
            => $"{(int)size.Width} {(int)size.Height}";
    }
}
