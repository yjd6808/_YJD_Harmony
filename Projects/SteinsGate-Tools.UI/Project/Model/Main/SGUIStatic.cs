/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 2:50:54 PM
 *
 */

using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Sga;
using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using SGToolsCommon.Primitive;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{

    [CategoryOrder(Constant.StaticCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIStatic : SGUIElement, ISizeRestorable
    {
        public const int OrderSize = 1;

        public SGUIStatic()
        {
            _visualSize = Constant.DefaultVisualSize;
        }


        [ReadOnly(false)]
        [Category(Constant.StaticCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => _visualSize;
            set
            {
                _visualSize = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Browsable(false)] public override double VisibleOpacity => _visible ? 0.5 : 0;
        


        public override SGUIElementType UIElementType => SGUIElementType.Static;
        [Browsable(false)] public override bool Manipulatable => true;

        public override object Clone()
        {
            SGUIStatic sprite = new SGUIStatic();
            sprite.CopyFrom(this);
            sprite._visualSize = _visualSize;
            return sprite;
        }

        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            root[JsonVisualSizeKey] = _visualSize.ToFullString();
            return root;
        }

        public override void ParseJObject(JObject root)
        {
            base.ParseJObject(root);

            string sizeString = (string)root[JsonVisualSizeKey];
            _visualSize = SizeEx.ParseFullString(sizeString);
        }

        public override void CreateInit() => VisualName = $"스태틱_{Seq++}";
        public static int Seq;
        private IntSize _visualSize;

        public void RestoreSize()
            => VisualSize = Constant.DefaultVisualSize;
    }
}
