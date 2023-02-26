/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:14:57 AM
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
    public enum SgaColorFormat
    {
        Unknown = 0x00,  // 0
        Argb1555 = 0x0e, // 14 ( 9591 )
        Argb4444 = 0x0f, // 15 (  278 )
        Argb8888 = 0x10, // 16 ( 2871 )
        DDSDxt1 = 0x12,  // 18
        DDSDxt3 = 0x13,  // 19
        DDSDxt5 = 0x14,  // 20
        Link = 0x11,     // 17
    }
}
