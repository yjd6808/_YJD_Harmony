/*
 * 작성자: 윤정도
 * 생성일: 3/10/2023 8:19:45 PM
 *
 */

using System;
using System.IO;

namespace SGToolsCommon.Extension
{
    public static class FileEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/1395205/better-way-to-check-if-a-path-is-a-file-or-a-directory
        public static bool IsFile(string _path)
        {
            FileAttributes attr = File.GetAttributes(_path);

            if ((attr & FileAttributes.Directory) != FileAttributes.Directory)
                return true;

            return false;
        }

        public static byte[] ReadBytes(string _path, int _size)
        {
            byte[] bytes = new byte[_size];
            using FileStream fs = new FileStream(_path, FileMode.Open, FileAccess.Read);
            int readBytes = fs.Read(bytes, 0, _size);
            if (readBytes < _size)
                throw new Exception($"파일 크기가 {_size}보다 작습니다.");
            return bytes;
        }

        public static int ReadAllBytes(string _path, byte[] _buffer)
        {
            using FileStream fs = new FileStream(_path, FileMode.Open, FileAccess.Read);
            int chunkSize = (int)fs.Length > 2048 ? 2048 : (int)fs.Length;

            int bytesRead;
            int offset = 0;
            // 파일 끝까지 반복해서 읽음
            while ((bytesRead = fs.Read(_buffer, offset, chunkSize)) > 0)
            {
                offset += bytesRead;
            }
            offset += bytesRead;
            return offset;
        }

        public static int WriteAllBytes(string _path, byte[] _buffer)
            => WriteAllBytes(_path, _buffer, 0, _buffer.Length);

        public static int WriteAllBytes(string _path, byte[] _buffer, int _offset)
            => WriteAllBytes(_path, _buffer, _offset, _buffer.Length - _offset);

        public static int WriteAllBytes(string _path, byte[] _buffer, int _offset, int _count)
        {
            const int chunkSize = 2048;
            using FileStream fs = new FileStream(_path, FileMode.OpenOrCreate, FileAccess.Write);
            int bytesWritten = 0;

            while (bytesWritten < _count)
            {
                int bytesToWrite = Math.Min(chunkSize, _count - bytesWritten);
                fs.Write(_buffer, _offset + bytesWritten, bytesToWrite);
                bytesWritten += bytesToWrite;
            }
            return bytesWritten;
        }
    }
}

