/*
 * 작성자: 윤정도
 * 생성일: 3/13/2023 7:07:03 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
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
using Newtonsoft.Json.Linq;
using SGToolsCommon.Sga;

namespace SGToolsCommon.Converter
{
    /*
     * 랩패널의 일정한 크기의 박스내에서 이미지를 "잘" 표현하기 위한 클래스이다.
     * 이미지 소스의 너비, 높이 둘중 하나가 박스보다 크면 Uniform으로 설정하여 이미지 비율을 그대로 유지하며 축소하여 보여주도록 한다.
     * 만약 너비, 높이 모두 박스크기보다 작다면 스트레치.None으로 설정하여 원래 이미지 크기로 보이도록 한다.
     */

    public class SourceStretchConverterWithSize : IMultiValueConverter
    {
        public static readonly SourceStretchConverterWithSize Instance = new ();

      
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (values.Length != 2)
                throw new Exception("무조건 2개의 소스가 전달되어야합니다.");
            if (values[0] is not SgaSprite sprite)
                throw new Exception("소스가 스프라이트 타입이 아닙니다.");
            if (values[1] is not double availabieSize)
                throw new Exception("파라미터가 double 타입이 아닙니다.");

            if (Math.Max(sprite.Width, sprite.Height) > availabieSize)
                return Stretch.Uniform;

            return Stretch.None;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
