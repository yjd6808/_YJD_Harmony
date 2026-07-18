/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 7:46:26 AM
 *
 * 이름 긴 패키지들 짧게 변경해서 보여주기위한 컨바터
 */

using System;
using System.Globalization;
using System.IO;
using System.Windows.Data;
using System.Windows.Markup;

using SGToolsCommon.Sga;

namespace SGToolsCommon.Customize.Converter
{
    public class SgaFileNameConverter : MarkupExtension, IValueConverter
    {
        public static Func<string, string, string>[] Converter = new[]
        {
            new Func<string, string, string>(InterfaceConverter)
        };

        //////////////////////////////////////////////////////////////////////////////////
        public object Convert(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            string fileName = ((string)_value).Replace("_", "");
            string fileExt = Path.GetExtension(fileName);
            int packageType = (int)_parameter;

            //if (packageType < 0 || packageType >= Converter.Length)
            //    throw new Exception($"{SgaPackageType.Name[packageType]}에 해당하는 컨버터가 없습니다.");

            return Converter[packageType](fileName, fileExt);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public object ConvertBack(object _value, Type _targetType, object _parameter, CultureInfo _culture)
        {
            throw new Exception("SgaFileNameConverter ConvertBack 함수 미구현");
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ProvideValue(IServiceProvider _serviceProvider)
        {
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private static string InterfaceConverter(string _fileName, string _ext)
        {
            string replaced = _fileName;

            replaced = _fileName.Replace("sprite", "");
            if (replaced.Length <= _ext.Length)
                return _fileName;

            string replaced2 = replaced.Replace("interface", "");
            if (replaced2.Length <= _ext.Length)
                return replaced;

            return replaced2;
        }
    }
}
