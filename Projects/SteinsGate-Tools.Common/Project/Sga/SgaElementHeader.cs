/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:41:09 AM
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
    public class SgaElementHeader
    {
        public int Offset { get; set; }
        public int NextOffset { get; set; }
        public int IndexInPackage { get; set; }
        public int Length { get; set; }
        public String Name { get; set; }
    }
}
