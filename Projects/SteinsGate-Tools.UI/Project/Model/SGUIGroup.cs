/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 9:16:47 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Automation.Peers;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using MoreLinq;

namespace SGToolsUI.Model
{
    public class SGUIGroup : SGUIElement
    {
        public SGUIGroup() : base(SGUIElementType.Group)
        {
        }

        [Browsable(false)]
        [Category("Group")]
        [DisplayName("Children")]
        [Description("UI 그룹의 자식들")]
        public ObservableCollection<SGUIElement> Children
        {
            get => _children;
            set
            {
                _children = value;
                OnPropertyChanged();
            }
        }

        [Category("Group")]
        [DisplayName("ChildCount")]
        [Description("UI 그룹의 자식수를 의미합니다.")]
        public int ChildCount => Children.Count;

        [Category("Group")]
        [DisplayName("ChildCountRecursive")]
        [Description("UI 그룹 자식수를 의미합니다. (하위 자식들 모두 포함)")]
        public int ChildCountRecursive
        {
            get
            {
                int Count = 0;
                ForEachRecursive((_) => Count++);
                return Count;
            }
        }

        [Category("Group")]
        [DisplayName("HorizontalAlignment")]
        [Description("UI 그룹내 자식의 가로 정렬 기준입니다.")]
        public HorizontalAlignment HorizontalAlignment
        {
            get => _horizontalAlignment;
            set
            {
                _horizontalAlignment = value;
                OnPropertyChanged();
            }
        }

        [Category("Group")]
        [DisplayName("VerticalAlignment")]
        [Description("UI 그룹내 자식의 세로 정렬 기준입니다.")]
        public VerticalAlignment VerticalAlignment
        {
            get => _verticalAlignment;
            set
            {
                _verticalAlignment = value;
                OnPropertyChanged();
            }
        }


        public override object Clone()
        {
            SGUIGroup group = new SGUIGroup();
            group.CopyFrom(this);
            group.HorizontalAlignment = HorizontalAlignment;
            group.VerticalAlignment = VerticalAlignment;

            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement cloned = _children[i].Clone() as SGUIElement;
                if (cloned == null)
                    throw new Exception("클론한 갹체가 null입니다.");
                group._children.Add(cloned);
            }

            return group;
        }

        public void ForEachRecursive(Action<SGUIElement> action)
        {
            for (int i = 0; i < _children.Count; ++i)
            {
                SGUIElement element = _children[i];
                action(element);
                if (element.ElementType == SGUIElementType.Group)
                {
                    ((SGUIGroup)element).ForEachRecursive(action);
                    continue;
                }
            }
        }

        public void ForEach(Action<SGUIElement> action) => _children.ForEach(action);

        private ObservableCollection<SGUIElement> _children = new();
        private HorizontalAlignment _horizontalAlignment = HorizontalAlignment.Left;
        private VerticalAlignment _verticalAlignment = VerticalAlignment.Bottom;

    }
}
