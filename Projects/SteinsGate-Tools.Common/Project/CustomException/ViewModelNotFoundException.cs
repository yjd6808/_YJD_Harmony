/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:51:34 AM
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

namespace SGToolsCommon.CustomException
{
    public class ViewModelNotFoundException : System.Exception
    {
        public ViewModelNotFoundException() : base("뷰 모델을 찾지 못했습니다.") { }
    }
}
