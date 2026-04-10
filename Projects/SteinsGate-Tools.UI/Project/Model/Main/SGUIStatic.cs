/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 2:50:54 PM
 *
 */

using System.ComponentModel;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model.Main
{
    [CategoryOrder(Constant.StaticCategoryName, Constant.OtherCategoryOrder)]
    public class SGUIStatic : SGUIElement, ISizeRestorable
    {
        public const int OrderSize = 1;

        public static int Seq;

        private IntSize visualSize_;

        //////////////////////////////////////////////////////////////////////////////////
        public SGUIStatic()
        {
            visualSize_ = Constant.DefaultVisualSize;
        }

        [ReadOnly(false)]
        [Category(Constant.StaticCategoryName), DisplayName("크기"), PropertyOrder(OrderSize)]
        public override IntSize VisualSize
        {
            get => visualSize_;
            set
            {
                visualSize_ = value;
                OnPropertyChanged();
                OnPropertyChanged(nameof(VisualRect));
            }
        }

        [Browsable(false)] public override double VisibleOpacity => visible_ ? 0.5 : 0;

        public override SGUIElementType UIElementType => SGUIElementType.Static;
        [Browsable(false)] public override bool Manipulatable => true;

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIStatic sprite = new SGUIStatic();
            sprite.CopyFrom(this);
            sprite.visualSize_ = visualSize_;
            return sprite;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override JObject ToJObject()
        {
            JObject root = base.ToJObject();
            root[JsonVisualSizeKey] = visualSize_.ToFullString();
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseJObject(JObject _root)
        {
            base.ParseJObject(_root);

            string sizeString = (string)_root[JsonVisualSizeKey];
            visualSize_ = SizeEx.ParseFullString(sizeString);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"스태틱_{Seq++}";

        //////////////////////////////////////////////////////////////////////////////////
        public void RestoreSize()
            => VisualSize = Constant.DefaultVisualSize;
    }
}
