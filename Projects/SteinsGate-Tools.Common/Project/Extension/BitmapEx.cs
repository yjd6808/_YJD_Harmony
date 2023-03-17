/*
 * 복붙
 * 생성일: 2/27/2023 6:41:47 AM
 */

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using PixelFormat = System.Drawing.Imaging.PixelFormat;
using System.Windows.Media.Media3D;
using System.Windows;
using Vanara.PInvoke;

using Point = System.Windows.Point;
using Size = System.Windows.Size;
using DPoint = System.Drawing.Point;
using DSize = System.Drawing.Size;

namespace SGToolsCommon.Extension
{
    public static class BitmapEx
    {
        public static Bitmap FromArray(byte[] data, int width, int height)
        {
            var bmp = new Bitmap(width, height, PixelFormat.Format32bppArgb);
            var bmpData = bmp.LockBits(new Rectangle(DPoint.Empty, new DSize(width, height)), ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(data, 0, bmpData.Scan0, data.Length);
            bmp.UnlockBits(bmpData);
            return bmp;
        }

        public static ImageSource ToSource(this Bitmap bitmap)
            => BitmapSource.Create(bitmap.Width, bitmap.Height, 0, 0, PixelFormats.Bgra32, null, bitmap.ToArray(), 4 * bitmap.Width);
        
        public static byte[] ToArray(this Bitmap bmp)
        {
            ToArray(bmp, out var data);
            return data;
        }

        public static void ToArray(this Bitmap bmp, out byte[] data)
        {
            data = new byte[bmp.Width * bmp.Height * 4];
            var bmpData = bmp.LockBits(new Rectangle(DPoint.Empty, bmp.Size), ImageLockMode.ReadOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(bmpData.Scan0, data, 0, data.Length);
            bmp.UnlockBits(bmpData);
        }


        // 코코스에서는 Rgb이므로 바이트 순서 유념해야한다.
        public static void Bgra32LinearDodgePixel(Span<byte> pixelData)
        {
            var max = Math.Max(pixelData[0], Math.Max(pixelData[1], pixelData[2]));
            var sub = (byte)(0xff - max);
            pixelData[3] = Math.Min(pixelData[3], max);
            pixelData[2] += sub;
            pixelData[1] += sub;
            pixelData[0] += sub;
        }

        // 비동기버전은 지원안해서 만들어씀
        public static Task SaveAsync(this Bitmap bitmap, string path, ImageFormat format = null)
        {
            return Task.Run(() =>
            {
                using (FileStream fileStream = new FileStream(path, FileMode.Create, FileAccess.Write))
                    bitmap.Save(fileStream, format == null ? ImageFormat.Png : format);
            });
        }

        public static void SaveToClipboard(this Bitmap bitmap)
        {
            IntPtr handle = bitmap.GetHbitmap();
            try
            {
                var source = Imaging.CreateBitmapSourceFromHBitmap(handle, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
                Clipboard.SetImage(source);
            }
            finally
            {
                Gdi32.DeleteObject(handle);
            }
        }
    }
}
