﻿/*
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

        public SGUIGroup(int depth)
        {
            _depth = depth;
        }

        // ============================================================
        //            프로파티
        // ============================================================


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
                if (_horizontalAlignment == value)
                    return;

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
                if (_verticalAlignment == value)
                    return;

                _verticalAlignment = value;
                OnPropertyChanged();
            }
        }

        [Category("Visual")]
        [DisplayName("Visible")]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정합니다. (그룹 요소입니다. 하위 항목들도 모두 적용됩니다.)")]
        public override bool IsVisible
        {
            get => _visible;
            set
            {
                if (_visible == value)
                    return;

                _visible = value;
                ForEachRecursive(element => element.IsVisible = value);
                OnPropertyChanged();
            }
        }

        [Category("Visual")]
        [DisplayName("Depth")]
        [Description("이 엘리먼트의 계층구조상 위치")]
        public override int Depth => _depth;


       

        [Browsable(false)] public override bool IsGroup => true;
        public override SGUIElementType UIElementType => SGUIElementType.Group;

        private ObservableCollection<SGUIElement> _children = new();
        private HorizontalAlignment _horizontalAlignment = HorizontalAlignment.Left;
        private VerticalAlignment _verticalAlignment = VerticalAlignment.Bottom;
        private int _depth; // 계층 구조상 깊이. 추가한 이유: 깊이 계산시 연산 낭비가 심함. 특히 모든 원소 깊이를 계산하는 경우

        public static int Seq = 0;


        // ============================================================
        //            기능
        // ============================================================
        public override void CreateInit()
        {
            VisualName = $"그룹_{Seq++}";
        }

        public override object Clone()
        {
            SGUIGroup group = new SGUIGroup(_depth);
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

                if (element.UIElementType == SGUIElementType.Group)
                {
                    ((SGUIGroup)element).ForEachRecursive(action);
                    continue;
                }
            }
        }

        public void ForEach(Action<SGUIElement> action)
        {
            action(this);

            _children.ForEach(action);
        }





        public void UpdateDepth()
        {
            Children.Where(childElement => childElement.IsGroup)
                .Cast<SGUIGroup>()
                .ForEach(group =>
                {
                    group._depth = Parent._depth + 1;
                    group.UpdateDepth();
                });
        }



        // 디버깅용
        public void UpdateParent()
        {
            Children.ForEach(x =>
            {
                x.Parent = this;

                if (x.IsGroup)
                    x.Cast<SGUIGroup>().UpdateParent();
            });
        }

        
    }
}
