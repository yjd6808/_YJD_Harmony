/*
 * 복붙
 * 생성일: 2/27/2023 6:54:00 AM
 *
 */

using System;
using System.Runtime.InteropServices;

namespace SGToolsCommon.ThirdParty
{
    public static class Zlib
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static byte[] Compress(byte[] _data)
        {
            int size = (int)(_data.LongLength * 1.001 + 12);
            byte[] target = new byte[size];
            Compress(target, ref size, _data, _data.Length);
            byte[] temp = new byte[size];
            Buffer.BlockCopy(target, 0, temp, 0, size);
            return temp;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static byte[] Decompress(byte[] _data, int _size)
        {
            byte[] target = new byte[_size];
            Decompress(target, ref _size, _data, _data.Length);
            return target;
        }

        //////////////////////////////////////////////////////////////////////////////////
        [DllImport("zlib1.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "compress")]
        private static extern int Compress([In][Out] byte[] _dest, ref int _destLen, byte[] _source, int _sourceLen);

        //////////////////////////////////////////////////////////////////////////////////
        [DllImport("zlib1.dll", CallingConvention = CallingConvention.Cdecl, EntryPoint = "uncompress")]
        private static extern int Decompress([In][Out] byte[] _dest, ref int _destLen, byte[] _source, int _sourceLen);
    }
}
