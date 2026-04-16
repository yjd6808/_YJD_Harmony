/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 6:19:40 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Media.Imaging;
using System.Windows.Threading;

namespace SGToolsCommon.Sga
{
    public class SgaManager : Bindable
    {
        private List<SgaPackage>? packages_;

        public static SgaManager Instance = new();
        public List<SgaPackage>? Packages => packages_;
        public Dictionary<string, SgaPackage> PackageMap { get; } = new();
        public string SgaDirectory { get; set; } = string.Empty;
        public Dispatcher Dispatcher { get; set; } = null!;

        //////////////////////////////////////////////////////////////////////////////////
        private SgaManager() {}

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadSga(int _type)
        {
            if (packages_ != null)
                UnloadAll();

            packages_ = new List<SgaPackage>();

            if (_type < 0 || _type >= SgaPackageType.Max)
                throw new Exception("올바르지 않은 sga 패키지 타입입니다.");

            string dir = Path.Combine(SgaDirectory, SgaPackageType.Name[_type]);

            if (!Directory.Exists(dir))
                throw new Exception("SGA 패키지 디렉토리가 존재하지 않습니다.");

            string[] sgaFileList = Directory.GetFiles(dir, "*.sga", SearchOption.TopDirectoryOnly);

            for (int i = 0; i < sgaFileList.Length; ++i)
            {
                SgaPackage loadedPackage = SgaLoader.Load(sgaFileList[i], false, true, i);
                packages_.Add(loadedPackage);
                PackageMap.Add(loadedPackage.FileName, loadedPackage);
            }
            NotifyUpdateList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void UnloadAll()
        {
            if (packages_ == null)
                return;

            foreach (SgaPackage sgaPackage in packages_)
                sgaPackage.UnloadAll();
            packages_ = null;
            NotifyUpdateList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaPackage GetPackage(int _index)
        {
            if (!IsValidPackageIndex(_index))
                throw new Exception("올바르지 않은 패키지 인덱스입니다.");

            return Packages![_index];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaImage GetImg(string _packageName, string _imgName)
        {
            if (!IsValidPackageName(_packageName))
                throw new Exception("올바르지 않은 패키지 이름입니다.");

            SgaImage? img = PackageMap[_packageName].GetElement(_imgName) as SgaImage;

            if (img == null)
                throw new Exception("해당 SgaElement는 SgaImage타입이 아닙니다.");

            return img;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSpriteAbstract GetSprite(string _packageName, string _imgName, int _spriteIndex)
            => GetImg(_packageName, _imgName).GetSprite(_spriteIndex);

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSprite? GetSpriteLink(string _packageName, string _imgName, int _spriteIndex)
        {
            SgaImage img = GetImg(_packageName, _imgName);
            SgaSpriteAbstract sprite = img.GetSprite(_spriteIndex);

            if (sprite.IsLink)
            {
                SgaSpriteAbstract link = img.GetSprite(sprite.TargetFrameIndex);

                if (link.IsLink)
                    throw new Exception("링크로 따라간 스프라이트가 링크 스프라이트입니다.");

                return link as SgaSprite;
            }

            return sprite as SgaSprite;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 링크는 타겟 프레임 찾아서 비트맵 가져옴
        public BitmapSource GetBitmapSourceLink(string _packageName, string _imgName, int _spriteIndex)
            => GetSpriteLink(_packageName, _imgName, _spriteIndex)!.Source;

        //////////////////////////////////////////////////////////////////////////////////
        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(Packages));

        //////////////////////////////////////////////////////////////////////////////////
        private bool IsValidPackageIndex(int _index) => _index >= 0 && _index < (Packages?.Count ?? 0);

        //////////////////////////////////////////////////////////////////////////////////
        private bool IsValidPackageName(string _packageName) => PackageMap.ContainsKey(_packageName);
    }
}
