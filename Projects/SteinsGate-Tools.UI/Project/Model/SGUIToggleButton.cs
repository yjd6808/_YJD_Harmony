/*
 * 작성자: 윤정도
 * 생성일: 3/8/2023 6:31:58 PM
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
using Newtonsoft.Json.Linq;
using SGToolsUI.File;

namespace SGToolsUI.Model
{
    public class SGUIToggleButton : SGUIElement
    {
        public override SGUIElementType UIElementType => SGUIElementType.ToggleButton;
        public override void CreateInit()
        {
        }

        public override object Clone()
        {
            SGUIToggleButton clone = new SGUIToggleButton();
            return clone;
        }

        public override JObject ToJObject()
        {
            throw new NotImplementedException();
        }
    }
}
