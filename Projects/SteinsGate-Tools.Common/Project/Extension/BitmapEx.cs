/*
 * 복붙
 * 생성일: 2/27/2023 6:41:47 AM
 */

using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using PixelFormat = System.Drawing.Imaging.PixelFormat;
using System.Windows.Media.Media3D;

namespace SGToolsCommon.Extension
{
    public static class BitmapEx
    {
        public static Bitmap FromArray(byte[] data, int width, int height)
        {
            var bmp = new Bitmap(width, height, PixelFormat.Format32bppArgb);
            var bmpData = bmp.LockBits(new Rectangle(Point.Empty, new Size(width, height)), ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(data, 0, bmpData.Scan0, data.Length);
            bmp.UnlockBits(bmpData);
            return bmp;
        }

        public static ImageSource ToSource(this Bitmap bitmap)
            => BitmapSource.Create(bitmap.Width, bitmap.Height, 0, 0, System.Windows.Media.PixelFormats.Bgra32, null, bitmap.ToArray(), 4 * bitmap.Width);
        
        public static byte[] ToArray(this Bitmap bmp)
        {
            ToArray(bmp, out var data);
            return data;
        }

        public static void ToArray(this Bitmap bmp, out byte[] data)
        {
            data = new byte[bmp.Width * bmp.Height * 4];
            var bmpData = bmp.LockBits(new Rectangle(Point.Empty, bmp.Size), ImageLockMode.ReadOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(bmpData.Scan0, data, 0, data.Length);
            bmp.UnlockBits(bmpData);
        }
    }
}
