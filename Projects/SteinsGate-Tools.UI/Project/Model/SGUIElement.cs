/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 7:54:27 AM
 *
 */

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
using SGToolsCommon;
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    public abstract class SGUIElement : CanvasElement, ICloneable
    {
        // =========================================================================
        //                         엘리먼트 관련 정보
        // =========================================================================

       
        [Browsable(false)] public virtual bool IsGroup => false;

        [Category("Element")]
        [DisplayName("ElementType")]
        [Description("UI 타입을 의미")]
        public abstract SGUIElementType UIElementType { get; }

        

        [Category("Element")]
        [DisplayName("Name")]
        [Description("게임에서 사용할 이름 혹은 변수명의 축약형태")]
        public string Name
        {
            get => _name;
            set
            {
                _name = value;
                OnPropertyChanged();
            }
        }

        [ReadOnly(true)]
        [Browsable(true)]
        [Category("Element")]
        [DisplayName("Name")]
        [Description("게임에서 실제로 사용할 변수/Define명")]
        public string DefineName
        {
            get
            {
                StringBuilder sb = new StringBuilder(60);


                return sb.ToString();
            }
        }

        [Category("Element")]
        [DisplayName("Code")]
        [Description("엘리먼트 고유 코드를 의미")]
        public int Code
        {
            get => _code;
            set
            {
                _code = value;
                OnPropertyChanged();
            }
        }

        [Browsable(false)]
        [Category("Element")]
        [DisplayName("Selected")]
        [Description("엘리먼트가 트리뷰/캔버스 상에서 선택되었는지 ")]
        public bool Selected
        {
            get => _selected;
            set
            {
                if (_selected == value)
                    return;

                _selected = value;
                OnPropertyChanged();
            }
        }


        // =========================================================================
        //                         비주얼 관련 정보
        // =========================================================================
        [Browsable(false)]
        [Category("Visual")]
        [DisplayName("VisualRect")]
        [Description("UI엘리먼트의 캔버스 좌상단 위치와 크기를 의미")]
        public Rect VisualRect
        {
            get => _visualRect;
            set
            {
                _visualRect = value;
                OnPropertyChanged();
                OnPropertyChanged("VisualSize");
                OnPropertyChanged("VisualPosition");
            } 
        }

        [Category("Visual")]
        [DisplayName("VisualPosition")]
        [Description("UI엘리먼트의 캔버스 좌상단 위치를 의미")]
        public Point VisualPosition
        {
            get => _visualRect.Location;
            set
            {
                _visualRect.Location = value;
                OnPropertyChanged();
                OnPropertyChanged("VisualRect");
            }
        }

        [Category("Visual")]
        [DisplayName("VisualSize")]
        [Description("UI엘리먼트의 크기를 의미")]
        public Size VisualSize
        {
            get => _visualRect.Size;
            set
            {
                _visualRect.Size = value;
                OnPropertyChanged();
                OnPropertyChanged("VisualRect");
            }
        }

        [Category("Visual")]
        [DisplayName("VisualName")]
        [Description("UI엘리먼트가 트리뷰에서 나타내는 이름입니다.")]
        public string VisualName
        {
            get => _visualName;
            set
            {
                if (_visualName == value)
                    return;

                _visualName = value;
                OnPropertyChanged();
            }
        }

        [Category("Visual")]
        [DisplayName("Visible")]
        [Description("현재 엘리먼트를 캔버스상에서 표시될지를 결정")]
        public virtual bool IsVisible
        {
            get => _visible;
            set
            {
                if (_visible == value)
                    return;

                _visible = value;
                OnPropertyChanged();
            }
        }

        


        protected string _visualName = string.Empty;
        protected Rect _visualRect = new (0, 0, 50, 50);
        protected bool _selected = false;
        protected bool _visible = true;

        protected string _name;
        protected int _code;


        public void CopyFrom(SGUIElement element)
        {
            _visualName = element._visualName;
            _visualRect = element._visualRect;

            _name = element._name;
            _code = element._code;
        }

        public abstract void CreateInit();

        // ================================================
        // 팩토리 메서드
        // ================================================
        public static SGUIElement Create(SGUIElementType type)
        {
            SGUIElement element = null;

            switch (type)
            {
                case SGUIElementType.Button: 
                    element = new SGUIButton();
                    break;
                case SGUIElementType.Group: 
                    element = new SGUIGroup();
                    break;
            }

            if (element == null)
                throw new Exception("");

            element.CreateInit();
            return element;
        }

        

        public abstract object Clone();
        [Browsable(false)]
        public override CanvasElementType CanvasElementType => CanvasElementType.UIElement;
    }
}
