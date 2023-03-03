/*
 * 작성자: 챗 GPT
 * 생성일: 3/1/2023 2:13:19 PM
 *
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
using SGToolsCommon.Extension;
using SGToolsUI.CustomControl;
using SGToolsUI.Model;

namespace SGToolsUI.ModelSelector
{
    /*
     * WPF에서 ItemTemplateSelector는 컬렉션 컨트롤(예: ListBox, TreeView, ItemsControl 등)의
     * 개별 항목에 대한 DataTemplate을 선택하기 위한 기능입니다.
     * 컬렉션 컨트롤에서 데이터를 바인딩할 때, 데이터 항목의 형식에 따라 다른 템플릿을 사용하려면 ItemTemplateSelector를 사용할 수 있습니다.
     * ItemTemplateSelector는 컬렉션의 각 항목에 대해 템플릿을 선택하는 로직을 구현할 수 있으며,
     * 선택된 템플릿은 각 항목의 UI를 정의하는 데 사용됩니다.
     * ItemTemplateSelector를 구현하려면, DataTemplateSelector 클래스를 상속하여 선택 로직을 구현해야 합니다.
     * 이 클래스는 SelectTemplate 메서드를 구현해야 하며,
     * 이 메서드에서 선택할 DataTemplate을 반환합니다.SelectTemplate 메서드는
     * 나타내는 두 개의 매개 변수(선택하려는 항목과 그 항목의 부모 요소)를 받습니다.
     */

    public class UIElementTreeViewSelector : DataTemplateSelector
    {
        public static readonly UIElementTreeViewSelector Selector = new ();

        public override DataTemplate SelectTemplate(object item, DependencyObject container)
        {
            UIElementTreeView treeView = container.FindParent<UIElementTreeView>();

            switch (item)
            {
            case SGUIGroup        __group__       :  return (DataTemplate)Application.Current.FindResource("GroupTreeViewTemplate");
            case SGUIButton       __button__      :  return (DataTemplate)Application.Current.FindResource("ButtonTreeViewTemplate");
            }

            throw new Exception("SGUIElementSelector에서 반환가능한 DataTemplate을 찾지 못했습니다.");
        }
    }
}
