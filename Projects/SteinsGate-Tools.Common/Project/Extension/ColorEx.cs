/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:54:52 PM
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

namespace SGToolsCommon.Extension
{
    public static class ColorEx
    {
        public static Color AddAllWithoutAlpha(this Color me, byte all)
        {
            me.R = me.R + all > byte.MaxValue ? byte.MaxValue : (byte)(me.R + all);
            me.G = me.G + all > byte.MaxValue ? byte.MaxValue : (byte)(me.G + all);
            me.B = me.B + all > byte.MaxValue ? byte.MaxValue : (byte)(me.B + all);
            return me;
        }

        public static Color Add(this Color me, byte r, byte g, byte b, byte a)
        {
            me.R = me.R + r > byte.MaxValue ? byte.MaxValue : (byte)(me.R + r);
            me.G = me.G + g > byte.MaxValue ? byte.MaxValue : (byte)(me.G + g);
            me.B = me.B + b > byte.MaxValue ? byte.MaxValue : (byte)(me.B + b);
            me.A = me.A + a > byte.MaxValue ? byte.MaxValue : (byte)(me.A + a);
            return me;
        }

        public static Color Add(this Color me, byte r, byte g, byte b)
        {
            me.R = me.R + r > byte.MaxValue ? byte.MaxValue : (byte)(me.R + r);
            me.G = me.G + g > byte.MaxValue ? byte.MaxValue : (byte)(me.G + g);
            me.B = me.B + b > byte.MaxValue ? byte.MaxValue : (byte)(me.B + b);
            return me;
        }

        public static Color AddA(this Color me, byte a)
        {
            me.A = me.A + a > byte.MaxValue ? byte.MaxValue : (byte)(me.A + a);
            return me;
        }

        public static Color AddR(this Color me, byte r)
        {
            me.R = me.R + r > byte.MaxValue ? byte.MaxValue : (byte)(me.R + r);
            return me;
        }

        public static Color AddG(this Color me, byte g)
        {
            me.G = me.G + g > byte.MaxValue ? byte.MaxValue : (byte)(me.G + g);
            return me;
        }

        public static Color AddB(this Color me, byte b)
        {
            me.B = me.B + b > byte.MaxValue ? byte.MaxValue : (byte)(me.B + b);
            return me;
        }
    }
}
