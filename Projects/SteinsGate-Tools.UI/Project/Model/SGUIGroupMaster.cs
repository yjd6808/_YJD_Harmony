/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 2:38:01 PM
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
                OnPropertyChanged();
            }
        }

        private SGUIElement _selectedElement;
    }
}
