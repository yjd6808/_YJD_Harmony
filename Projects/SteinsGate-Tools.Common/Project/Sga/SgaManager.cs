/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 6:19:40 AM
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

namespace SGToolsCommon.Sga
{
    public class SgaManager
    {
        private SgaManager() {}
        public static SgaManager Instance = new ();



        public List<SgaPackage> Packages { get; } = new();


    }
}
