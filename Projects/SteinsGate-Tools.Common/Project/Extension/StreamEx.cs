/*
 * 복붙
 * 생성일: 2/27/2023 3:39:31 AM
 */

using System;
using System.IO;
using System.Text;

namespace SGToolsCommon.Extension
{
    public static class StreamEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static int Read(this Stream _stream, byte[] _buf)
            => _stream.Read(_buf, 0, _buf.Length);

        //////////////////////////////////////////////////////////////////////////////////
        public static int Read(this Stream _stream, int _length, out byte[] _buf)
        {
            _buf = new byte[_length];
            return _stream.Read(_buf, 0, _length);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static byte[] Read(this Stream _stream, int _length)
        {
            var buf = new byte[_length];
            _stream.Read(buf, 0, _length);
            return buf;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void Write(this Stream _stream, byte[] _buf)
            => _stream.Write(_buf, 0, _buf.Length);

        //////////////////////////////////////////////////////////////////////////////////
        public static void Seek(this Stream _stream, long _offset)
            => _stream.Seek(_offset, SeekOrigin.Current);

        //////////////////////////////////////////////////////////////////////////////////
        public static int ReadInt(this Stream _stream)
        {
            _stream.Read(4, out var buf);
            return BitConverter.ToInt32(buf, 0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static uint ReadUInt(this Stream _stream)
        {
            _stream.Read(4, out var buf);
            return BitConverter.ToUInt32(buf, 0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteInt(this Stream _stream, int _data)
            => _stream.Write(BitConverter.GetBytes(_data));

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteUInt(this Stream _stream, uint _data)
            => _stream.Write(BitConverter.GetBytes(_data));

        //////////////////////////////////////////////////////////////////////////////////
        public static short ReadShort(this Stream _stream)
        {
            var buf = new byte[2];
            _stream.Read(buf, 0, buf.Length);
            return BitConverter.ToInt16(buf, 0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static ushort ReadUShort(this Stream _stream)
        {
            var buf = new byte[2];
            _stream.Read(buf, 0, buf.Length);
            return BitConverter.ToUInt16(buf, 0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteShort(this Stream _stream, short _s)
            => _stream.Write(BitConverter.GetBytes(_s));

        //////////////////////////////////////////////////////////////////////////////////
        public static long ReadLong(this Stream _stream)
        {
            var buf = new byte[8];
            _stream.Read(buf, 0, buf.Length);
            return BitConverter.ToInt64(buf, 0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteLong(this Stream _stream, long _l)
            => _stream.Write(BitConverter.GetBytes(_l));

        //////////////////////////////////////////////////////////////////////////////////
        public static string ReadString(this Stream _stream)
            => ReadString(_stream, Encoding.Default);

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteString(this Stream _stream, string _str)
            => WriteString(_stream, _str, Encoding.Default, true);

        //////////////////////////////////////////////////////////////////////////////////
        public static string ReadString(this Stream _stream, Encoding _encoding)
        {
            var ms = new MemoryStream();
            int j;
            while ((j = _stream.ReadByte()) != 0 && j != -1)
                ms.WriteByte((byte)j);
            ms.Close();
            return _encoding.GetString(ms.ToArray());
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteString(this Stream _stream, string _str, Encoding _encoding)
            => WriteString(_stream, _str, _encoding, true);

        //////////////////////////////////////////////////////////////////////////////////
        public static void WriteString(this Stream _stream, string _str, Encoding _encoding, bool _split)
        {
            _stream.Write(_encoding.GetBytes(_str));
            if (_split)
                _stream.WriteByte(0);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static byte[] ReadToEnd(this Stream _stream)
        {
            var buf = new byte[_stream.Length - _stream.Position];
            _stream.Read(buf, 0, buf.Length);
            return buf;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ReadToEnd(this Stream _stream, out byte[] _buf)
        {
            _buf = new byte[_stream.Length - _stream.Position];
            _stream.Read(_buf, 0, _buf.Length);
        }
    }
}
