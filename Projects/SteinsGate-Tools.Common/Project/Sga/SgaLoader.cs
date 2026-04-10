/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:01 AM
 *
 */

using System.IO;
using System.Text;

using SGToolsCommon.Extension;

namespace SGToolsCommon.Sga
{
    public static class SgaLoader
    {
        public const string SgaFlag = "For SteinsGate Sga";
        public const string ImgFlag = "For SteinsGate Img";
        public const string SpriteFlag = "For SteinsGate Sprite";
        public const int SgaImgPathLen = 40;
        public static readonly Encoding Encoding = Encoding.UTF8;

        //////////////////////////////////////////////////////////////////////////////////
        public static SgaPackage Load(string _path, bool _indexOnly, bool _headerOnly, int _index)
        {
            FileStream readStream = File.OpenRead(_path);

            if (readStream.ReadString() != SgaFlag)
                return null;

            int elementCount = readStream.ReadInt();
            SgaPackage package = new SgaPackage(readStream, _path, elementCount, _index);

            if (elementCount == 0)
                return package;

            package.LoadIndex();

            if (_headerOnly)
                return package;

            package.Load(_indexOnly);
            return package;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string ReadElementPath(Stream _readStream)
        {
            byte[] data = new byte[SgaImgPathLen];
            int i = 0;
            while (i < SgaImgPathLen)
            {
                data[i] = (byte)(_readStream.ReadByte());

                if (data[i] == 0)
                    break;

                i++;
            }
            _readStream.Seek(SgaImgPathLen - 1 - i);
            return Encoding.GetString(data, 0, i);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static SgaElement ReadElement(SgaPackage _package, Stream _stream, SgaElementHeader _header, int _nextOffset, bool _indexOnly)
        {
            _stream.Seek(_header.Offset, SeekOrigin.Begin);

            SgaElement element;
            string elementFlag = _stream.ReadString();

            if (elementFlag == ImgFlag)
            {
                int indexLength = (int)_stream.ReadLong();
                int version = _stream.ReadInt();
                int spriteCount = _stream.ReadInt();
                int indexOffset = (int)_stream.Position;

                element = new SgaImage(spriteCount, _package, _header, version, indexOffset, indexLength);
                element.Load(_indexOnly);
                return element;
            }

            if (elementFlag == SpriteFlag)
            {
                int indexLength = _stream.ReadInt(); _stream.Seek(2, SeekOrigin.Current);
                int version = _stream.ReadInt();
                int spriteCount = _stream.ReadInt();
                int indexOffset = (int)_stream.Position;

                element = new SgaImage(spriteCount, _package, _header, version, indexOffset, indexLength);
                element.Load(_indexOnly);
                return element;
            }

            if (_header.Name.EndsWith(".wav") ||
                _header.Name.EndsWith(".ogg"))
            {
                _stream.Seek(_header.Offset);

                int version = 0;
                int indexLength = _header.NextOffset - (int)_stream.Position;
                int indexOffset = (int)_stream.Position;

                element = new SgaSound(_package, _header, version, indexOffset, indexLength);
                element.Load(_indexOnly);
                return element;
            }

            return null;
        }
    }
}
