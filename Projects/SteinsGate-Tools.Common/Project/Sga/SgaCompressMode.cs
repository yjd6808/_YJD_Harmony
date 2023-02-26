/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:17:45 AM
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
    public enum SgaCompressMode
    {
        Unkown = 0x00,
        None = 0x05,
        Zlib = 0x06,
        DDSZlib = 0x07
    }
}
