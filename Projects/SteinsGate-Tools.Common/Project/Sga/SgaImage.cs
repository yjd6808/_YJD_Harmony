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
using System.Collections.ObjectModel;

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

        public List<SgaSpriteAbstract> SpriteList => _spriteList;

        // Xaml 바인딩용
        public SgaImage()
        {
            _spriteList = new();
            _indexLoaded = true;
            _dataLoaded = true;
        }

        public SgaImage(int spriteCount, SgaPackage parent, SgaElementHeader header, int version, int indexOffset, int indexLength) 
            : base(SgaElementType.Image, parent, header, version, indexOffset, indexLength)
        {
            // @https://stackoverflow.com/questions/3363940/fill-listint-with-default-values
            // C# 리스트 디폴트로 초기화 방법
            
            _waitForLoading = spriteCount;
            _indexLoaded = false;
            _dataLoaded = false;
        }

        public bool IsValidSpriteIndex(int index) => index >= 0 && index < _spriteList.Count;
        public SgaSpriteAbstract GetSprite(int index)
        {
            if (!IndexLoaded)
                LoadIndexOnly();

            if (!IsValidSpriteIndex(index))
                throw new Exception($"{index}번 인덱스 스프라이트를 {Header.Name}에서 찾지 못했습니다.");

            return _spriteList[index];
        }

        public override void Load(bool indexOnly)
        {
            if (_spriteList != null)
                Unload();

            _spriteList = new List<SgaSpriteAbstract>(new SgaSpriteAbstract[_waitForLoading]);

            VersionLoader[Version](this, indexOnly);
            _indexLoaded = true;

            if (indexOnly)
                return;

            _dataLoaded = true;
            NotifyUpdateList();
        }

        public override void Unload()
        {
            if (_spriteList == null)
                return;

            for (int i = 0; i < _spriteList.Count; ++i)
                _spriteList[i]?.Unload();

            _spriteList = null;
            _dataLoaded = false;
            NotifyUpdateList();
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

                
                image._spriteList[i] = new SgaSprite(rect, format, compressMode, dataOffset, dataLength, image, i);

                if (indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }


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

                image._spriteList[i] = new SgaSprite(temp.Item2, temp.Item1, temp.Item3, dataOffset, dataLength, image, i);

                if (indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }

                image._spriteList[i].Load();
            }
            
        }

        public override string ToString()
            => Header.NameWithoutExt;


        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(SpriteList));
    }
}
