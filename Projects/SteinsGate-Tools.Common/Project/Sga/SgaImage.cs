/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:51:19 AM
 *
 */

using SGToolsCommon.Extension;
using System;
using System.Collections.Generic;
using System.IO;
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
using static System.Net.Mime.MediaTypeNames;
using System.Xml.Linq;

namespace SGToolsCommon.Sga
{
    public class SgaImage : SgaElement
    {
        private static Action<SgaImage, bool>[] VersionLoader = new Action<SgaImage, bool>[]
        {
            null,
            LoadVersion1,
            LoadVersion2,
        };
        

        private List<SgaSpriteAbstract> _spriteList;
        private int _waitForLoading;

        public SgaImage(int spriteCount, SgaPackage parent, SgaElementHeader header, int version, int indexOffset, int indexLength) 
            : base(SgaElementType.Image, parent, header, version, indexOffset, indexLength)
        {
            // @https://stackoverflow.com/questions/3363940/fill-listint-with-default-values
            // C# 리스트 디폴트로 초기화 방법
            _spriteList = new(new SgaSpriteAbstract[spriteCount]);
            _waitForLoading = spriteCount;
        }

        public override void Load(bool indexOnly)
        {
            VersionLoader[Version](this, indexOnly);
        }

        public override void Unload()
        {
            for (int i = 0; i < _spriteList.Count; ++i)
                _spriteList[i].Unload();
        }


        // ===============================================================================
        // 버전별 로딩 분리
        // ===============================================================================
        public static void LoadVersion1(SgaImage image, bool indexOnly)
        {
            Stream stream = image._parent.ReadStream;
            stream.Seek(image.IndexOffset, SeekOrigin.Begin);
            int spriteDataOffset = image.IndexOffset + image.IndexLength;

            for (int i = 0; i < image._waitForLoading; ++i)
            {
                SgaColorFormat format = (SgaColorFormat)stream.ReadInt();

                if (format == SgaColorFormat.Link)
                {
                    int targetFrameIndex = stream.ReadInt();
                    image._spriteList[i] = new SgaLinkSprite(image, targetFrameIndex, i);
                    continue;
                }

                SgaSpriteRect rect = new SgaSpriteRect();
                SgaCompressMode compressMode = (SgaCompressMode)stream.ReadInt();
                int width = stream.ReadInt();
                int height = stream.ReadInt();
                int dataLength = stream.ReadInt();
                rect.X = stream.ReadInt();
                rect.Y = stream.ReadInt();
                rect.FrameWidth = stream.ReadInt();
                rect.FrameHeight = stream.ReadInt();
                rect.Width = width;
                rect.Height = height;
                int dataOffset = (int)stream.Position;

                if (compressMode == SgaCompressMode.None)
                    dataLength = width * height * (format == SgaColorFormat.Argb8888 ? 4 : 2);

                if (indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }


                image._spriteList[i] = new SgaSprite(rect, dataOffset, dataLength, format, image, i);
                image._spriteList[i].Load();
            }
        }

        public static void LoadVersion2(SgaImage image, bool indexOnly)
        {
            Stream stream = image._parent.ReadStream;
            stream.Seek(image.IndexOffset, SeekOrigin.Begin);
            int spriteDataOffset = image.IndexOffset + image.IndexLength;


            List<Tuple<SgaColorFormat, SgaSpriteRect, SgaCompressMode, int>> tempList = new(image._waitForLoading);

            for (int i = 0; i < image._waitForLoading; ++i)
            {
                SgaColorFormat format = (SgaColorFormat)stream.ReadInt();

                if (format == SgaColorFormat.Link)
                {
                    int targetFrameIndex = stream.ReadInt();
                    image._spriteList[i] = new SgaLinkSprite(image, targetFrameIndex, i);
                    continue;
                }

                SgaSpriteRect rect = new SgaSpriteRect();
                SgaCompressMode compressMode = (SgaCompressMode)stream.ReadInt();
                int width = stream.ReadInt();
                int height = stream.ReadInt();
                int dataLength = stream.ReadInt();
                rect.X = stream.ReadInt();
                rect.Y = stream.ReadInt();
                rect.FrameWidth = stream.ReadInt();
                rect.FrameHeight = stream.ReadInt();
                rect.Width = width;
                rect.Height = height;

                if (compressMode == SgaCompressMode.None)
                    dataLength = width * height * (format == SgaColorFormat.Argb8888 ? 4 : 2);

                tempList.Add(new Tuple<SgaColorFormat, SgaSpriteRect, SgaCompressMode, int>(format, rect, compressMode, dataLength));
            }

            for (int i = 0; i < tempList.Count; ++i)
            {
                Tuple<SgaColorFormat, SgaSpriteRect, SgaCompressMode, int> temp = tempList[i];

                if (temp.Item1 == SgaColorFormat.Link)
                    continue;

                int dataOffset = (int)stream.Position;
                int dataLength = temp.Item4;

                image._spriteList[i] = new SgaSprite(temp.Item2, dataOffset, dataLength, temp.Item1, image, i);

                if (indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }

                image._spriteList[i].Load();
            }
        }
    }
}
