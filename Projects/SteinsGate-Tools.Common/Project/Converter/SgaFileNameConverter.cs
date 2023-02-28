/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:46:26 AM
 *
 * 이름 긴 패키지들 짧게 변경해서 보여주기위한 컨바터
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using SGToolsCommon.Sga;

namespace SGToolsCommon.Converter
{
    public class SgaFileNameConverter : MarkupExtension, IValueConverter
    {
        public static Dictionary<int, Func<string, string, string>> Converter = new ()
        {
            { SgaPackageType.Interface, InterfaceConverter }
        };

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            string fileName = ((string)value).Replace("_", "");
            string fileExt = Path.GetExtension(fileName);
            int packageType = (int)parameter;

            if (!Converter.ContainsKey(packageType))
                throw new Exception($"{SgaPackageType.Name[packageType]}에 해당하는 컨버터가 없습니다.");

            return Converter[packageType](fileName, fileExt);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new Exception("SgaFileNameConverter ConvertBack 함수 미구현");
        }

        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            return this;
        }

        private static string InterfaceConverter(string fileName, string ext)
        {
            string replaced = fileName;

            replaced = fileName.Replace("sprite", "");
            if (replaced.Length <= ext.Length)
                return fileName;

            string replaced2 = replaced.Replace("interface", "");
            if (replaced2.Length <= ext.Length)
                return replaced;

            return replaced2;
        }
    }
}
