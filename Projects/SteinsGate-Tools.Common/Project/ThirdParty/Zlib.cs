/*
 * 복붙
 * 생성일: 2/27/2023 6:54:00 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
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

namespace SGToolsCommon.ThirdParty
{
    public static class Zlib
    {
        public static byte[] Compress(byte[] data)
        {
            var size = (int)(data.LongLength * 1.001 + 12);
            var target = new byte[size];
            Compress(target, ref size, data, data.Length);
            var temp = new byte[size];
            Buffer.BlockCopy(target, 0, temp, 0, size);
            return temp;
        }

        public static byte[] Decompress(byte[] data, int size)
        {
            var target = new byte[size];
            Decompress(target, ref size, data, data.Length);
            return target;
        }


        [DllImport("zlib1.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "compress")]
        private static extern int Compress([In][Out] byte[] dest, ref int destLen, byte[] source, int sourceLen);

        [DllImport("zlib1.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "uncompress")]
        private static extern int Decompress([In][Out] byte[] dest, ref int destLen, byte[] source, int sourceLen);
    }
}
