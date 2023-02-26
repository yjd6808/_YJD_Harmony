/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:01 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Security;
using System.Reflection.PortableExecutable;
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



        public static SgaPackage Load(string path, bool indexOnly, bool headerOnly)
        {
            FileStream readStream = File.OpenRead(path);

            if (readStream.ReadString() != SgaFlag)
                return null;

            int elementCount = readStream.ReadInt();
            SgaPackage package = new SgaPackage(readStream, path, elementCount);

            if (elementCount == 0)
                return package;


            package.LoadIndex();
            
            if (headerOnly)
                return package;

            package.Load(indexOnly);
            return package;
        }



        public static string ReadElementPath(Stream readStream)
        {
            var data = new byte[SgaImgPathLen];
            var i = 0;
            while (i < SgaImgPathLen)
            {
                data[i] = (byte)(readStream.ReadByte());

                if (data[i] == 0)
                    break;

                i++;
            }
            readStream.Seek(SgaImgPathLen - 1 - i);
            return Encoding.GetString(data, 0, i);
        }

        public static SgaElement ReadElement(SgaPackage package, Stream stream, SgaElementHeader header, int nextOffset, bool indexOnly)
        {
            stream.Seek(header.Offset);

            SgaElement element;
            string elementFlag = stream.ReadString();

            if (elementFlag == ImgFlag)
            {
                int indexLength = stream.ReadInt();
                int version = stream.ReadInt();
                int spriteCount = stream.ReadInt();
                int indexOffset = (int)stream.Position;

                element = new SgaImage(spriteCount, package, header, version, indexOffset, indexLength);

                if (indexOnly)
                    return element;

                element.Load(false);
                return element;
            }

            if (elementFlag == SpriteFlag)
            {
                int indexLength = stream.ReadInt(); stream.Seek(2, SeekOrigin.Current);
                int version = stream.ReadInt();
                int spriteCount = stream.ReadInt();
                int indexOffset = (int)stream.Position;

                element = new SgaImage(spriteCount, package, header, version, indexOffset, indexLength);

                if (indexOnly)
                    return element;

                element.Load(false);
                return element;
            }


            if (header.Name.EndsWith(".wav") ||
                header.Name.EndsWith(".ogg"))
            {
                stream.Seek(header.Offset);

                int version = 0;
                int indexLength = header.NextOffset - (int)stream.Position;
                int indexOffset = (int)stream.Position;

                element = new SgaSound(package, header, version, indexOffset, indexLength);

                if (indexOnly) 
                    return element;

                element.Load(false);
                return element;
            }

            return null;
        }
    }
}
