/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:51:19 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;

using SGToolsCommon.Extension;

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

        private List<SgaSpriteAbstract> spriteList_;
        private int waitForLoading_;

        public List<SgaSpriteAbstract> SpriteList => spriteList_;

        //////////////////////////////////////////////////////////////////////////////////
        // Xaml 바인딩용
        public SgaImage()
        {
            spriteList_ = new();
            indexLoaded_ = true;
            dataLoaded_ = true;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaImage(int _spriteCount, SgaPackage _parent, SgaElementHeader _header, int _version, int _indexOffset, int _indexLength)
            : base(SgaElementType.Image, _parent, _header, _version, _indexOffset, _indexLength)
        {
            waitForLoading_ = _spriteCount;
            indexLoaded_ = false;
            dataLoaded_ = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool IsValidSpriteIndex(int _index) => _index >= 0 && _index < spriteList_.Count;

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSpriteAbstract GetSprite(int _index)
        {
            if (!IndexLoaded)
                LoadIndexOnly();

            if (!IsValidSpriteIndex(_index))
                throw new Exception($"{_index}번 인덱스 스프라이트를 {Header.Name}에서 찾지 못했습니다.");

            return spriteList_[_index];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Load(bool _indexOnly)
        {
            if (spriteList_ != null)
                Unload();

            spriteList_ = new List<SgaSpriteAbstract>(new SgaSpriteAbstract[waitForLoading_]);

            VersionLoader[Version](this, _indexOnly);
            indexLoaded_ = true;

            if (_indexOnly)
                return;

            dataLoaded_ = true;
            NotifyUpdateList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Unload()
        {
            if (spriteList_ == null)
                return;

            for (int i = 0; i < spriteList_.Count; ++i)
                spriteList_[i]?.Unload();

            dataLoaded_ = false;
            NotifyUpdateList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
            => Header.NameWithoutExt;

        //////////////////////////////////////////////////////////////////////////////////
        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(SpriteList));


        // ===============================================================================
        // 버전별 로딩 분리
        // ===============================================================================

        //////////////////////////////////////////////////////////////////////////////////
        public static void LoadVersion1(SgaImage _image, bool _indexOnly)
        {
            Stream stream = _image.parent_.ReadStream;
            stream.Seek(_image.IndexOffset, SeekOrigin.Begin);

            for (int i = 0; i < _image.waitForLoading_; ++i)
            {
                SgaColorFormat format = (SgaColorFormat)stream.ReadInt();

                if (format == SgaColorFormat.Link)
                {
                    int targetFrameIndex = stream.ReadInt();
                    _image.spriteList_[i] = new SgaLinkSprite(_image, targetFrameIndex, i);
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

                _image.spriteList_[i] = new SgaSprite(rect, format, compressMode, dataOffset, dataLength, _image, i);

                if (_indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }

                _image.spriteList_[i].Load();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void LoadVersion2(SgaImage _image, bool _indexOnly)
        {
            Stream stream = _image.parent_.ReadStream;
            stream.Seek(_image.IndexOffset, SeekOrigin.Begin);

            List<Tuple<SgaColorFormat, SgaSpriteRect, SgaCompressMode, int>> tempList = new(_image.waitForLoading_);

            for (int i = 0; i < _image.waitForLoading_; ++i)
            {
                SgaColorFormat format = (SgaColorFormat)stream.ReadInt();

                if (format == SgaColorFormat.Link)
                {
                    int targetFrameIndex = stream.ReadInt();
                    _image.spriteList_[i] = new SgaLinkSprite(_image, targetFrameIndex, i);
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

                _image.spriteList_[i] = new SgaSprite(temp.Item2, temp.Item1, temp.Item3, dataOffset, dataLength, _image, i);

                if (_indexOnly)
                {
                    stream.Seek(dataOffset + dataLength, SeekOrigin.Begin);
                    continue;
                }

                _image.spriteList_[i].Load();
            }
        }
    }
}
