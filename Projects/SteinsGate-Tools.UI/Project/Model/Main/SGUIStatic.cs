/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 2:50:54 PM
 *
 */

using System.ComponentModel;
using System.Xml.Linq;
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
        public override string GetElementTagName() => "Static";

        //////////////////////////////////////////////////////////////////////////////////
        public override XElement ToXElement()
        {
            XElement root = base.ToXElement();
            root.SetAttributeValue("width", visualSize_.Width);
            root.SetAttributeValue("height", visualSize_.Height);
            return root;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void ParseXElement(XElement _root)
        {
            base.ParseXElement(_root);

            visualSize_.Width = (int)_root.Attribute("width")!;
            visualSize_.Height = (int)_root.Attribute("height")!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override object Clone()
        {
            SGUIStatic sprite = new SGUIStatic();
            sprite.CopyFrom(this);
            sprite.visualSize_ = visualSize_;
            return sprite;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void CreateInit() => VisualName = $"스태틱_{Seq++}";

        //////////////////////////////////////////////////////////////////////////////////
        public void RestoreSize()
            => VisualSize = Constant.DefaultVisualSize;
    }
}
