/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
 * 이 그룹의 자식은 트리뷰의 아이템소스
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
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

namespace SGToolsUI.Model
{
    public class SGUIGroupMaster : SGUIGroup
    {
        public SGUIElement SelectedElement
        {
            get => _selectedElement;
            set
            {
                _selectedElement = value;
                Debug.WriteLine(_selectedElement.VisualName + "선택됨");
                OnPropertyChanged();
                OnPropertyChanged("HasSelectedElement");
            }
        }

        public bool HasSelectedElement => _selectedElement != null;

        // 선택된 엘리먼트가 없을 경우 마스터
        // 선택된 엘리먼트가 그룹일 경우 그녀석
        public SGUIGroup SelectedGroupAny
        {
            get
            {
                if (!HasSelectedElement)
                    return this;

                if (SelectedElement.IsGroup)
                    return SelectedElement as SGUIGroup;

                return null;
            }
        }

        public void DeselectAll() => ForEachRecursive(element => element.Selected = false);

        private SGUIElement _selectedElement = null;
    }
}
