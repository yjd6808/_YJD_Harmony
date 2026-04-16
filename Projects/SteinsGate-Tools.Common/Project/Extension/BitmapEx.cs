/*
 * 복붙
 * 생성일: 2/27/2023 6:41:47 AM
 */

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using Vanara.PInvoke;

using PixelFormat = System.Drawing.Imaging.PixelFormat;
using DPoint = System.Drawing.Point;
using DSize = System.Drawing.Size;

namespace SGToolsCommon.Extension
{
    public static class BitmapEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static Bitmap FromArray(byte[] _data, int _width, int _height)
        {
            Bitmap bmp = new Bitmap(_width, _height, PixelFormat.Format32bppArgb);
            BitmapData bmpData = bmp.LockBits(new Rectangle(DPoint.Empty, new DSize(_width, _height)), ImageLockMode.WriteOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(_data, 0, bmpData.Scan0, _data.Length);
            bmp.UnlockBits(bmpData);
            return bmp;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static ImageSource ToSource(this Bitmap _bitmap)
            => BitmapSource.Create(_bitmap.Width, _bitmap.Height, 0, 0, PixelFormats.Bgra32, null, _bitmap.ToArray(), 4 * _bitmap.Width);

        //////////////////////////////////////////////////////////////////////////////////
        public static byte[] ToArray(this Bitmap _bmp)
        {
            ToArray(_bmp, out byte[] data);
            return data;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ToArray(this Bitmap _bmp, out byte[] _data)
        {
            _data = new byte[_bmp.Width * _bmp.Height * 4];
            BitmapData bmpData = _bmp.LockBits(new Rectangle(DPoint.Empty, _bmp.Size), ImageLockMode.ReadOnly,
                PixelFormat.Format32bppArgb);
            Marshal.Copy(bmpData.Scan0, _data, 0, _data.Length);
            _bmp.UnlockBits(bmpData);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 코코스에서는 Rgb이므로 바이트 순서 유념해야한다.
        public static void Bgra32LinearDodgePixel(Span<byte> _pixelData)
        {
            byte max = Math.Max(_pixelData[0], Math.Max(_pixelData[1], _pixelData[2]));
            byte sub = (byte)(0xff - max);
            _pixelData[3] = Math.Min(_pixelData[3], max);
            _pixelData[2] += sub;
            _pixelData[1] += sub;
            _pixelData[0] += sub;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 비동기버전은 지원안해서 만들어씀
        public static Task SaveAsync(this Bitmap _bitmap, string _path, ImageFormat? _format = null)
        {
            return Task.Run(() =>
            {
                using (FileStream fileStream = new FileStream(_path, FileMode.Create, FileAccess.Write))
                    _bitmap.Save(fileStream, _format == null ? ImageFormat.Png : _format);
            });
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void SaveToClipboard(this Bitmap _bitmap)
        {
            IntPtr handle = _bitmap.GetHbitmap();
            try
            {
                BitmapSource source = Imaging.CreateBitmapSourceFromHBitmap(handle, IntPtr.Zero, Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
                Clipboard.SetImage(source);
            }
            finally
            {
                Gdi32.DeleteObject(handle);
            }
        }
    }
}
