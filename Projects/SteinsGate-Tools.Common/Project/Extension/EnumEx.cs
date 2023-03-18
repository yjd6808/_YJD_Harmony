/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 8:50:26 PM
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
    public static class EnumEx
    {
        public static T Parse<T>(string t)
        {
            return (T)Enum.Parse(typeof(T), t);
        }
    }
}
