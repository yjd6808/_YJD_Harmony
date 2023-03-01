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
    public abstract class SGUIElement : Bindable, ICloneable
    {
        public SGUIElement(SGUIElementType elementType)
        {
            _elementType = elementType;
        }


        // =========================================================================
        //                         엘리먼트 관련 정보
        // =========================================================================
        [Category("Element")]
        [DisplayName("ElementType")]
        [Description("UI 타입을 의미")]
        [ItemsSource(typeof(SGUIElementType))]
        public SGUIElementType ElementType
        {
            get => _elementType;
            set
            {
                _elementType = value;
                OnPropertyChanged();
            }
        }

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
                _selected = value;
                OnPropertyChanged();
            }
        }


        // =========================================================================
        //                         비주얼 관련 정보
        // =========================================================================
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
                _visualName = value;
                OnPropertyChanged();
            }
        }


        private string _visualName = string.Empty;
        private Rect _visualRect = new (0, 0, 50, 50);
        private bool _selected;

        private SGUIElementType _elementType;
        private string _name;
        private int _code;


        public void CopyFrom(SGUIElement element)
        {
            _visualName = element._visualName;
            _visualRect = element._visualRect;

            _elementType = element._elementType;
            _name = element._name;
            _code = element._code;
        }

        public abstract object Clone();
    }
}
