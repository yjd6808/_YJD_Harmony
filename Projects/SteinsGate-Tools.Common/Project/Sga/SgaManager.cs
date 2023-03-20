/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 6:19:40 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Threading;

namespace SGToolsCommon.Sga
{
    public class SgaManager : Bindable
    {
        private SgaManager() {}
        public static SgaManager Instance = new ();

        public List<SgaPackage> Packages => _packages;

        public Dictionary<string, SgaPackage> PackageMap { get; } = new();
        public string SgaDirectory { get; set; } = string.Empty;
        public Dispatcher Dispatcher { get; set; }
        private List<SgaPackage> _packages;

        public void LoadSga(int type)
        {
            if (_packages != null)
                UnloadAll();

            _packages = new List<SgaPackage>();

            if (type < 0 || type >= SgaPackageType.Max)
                throw new Exception("올바르지 않은 sga 패키지 타입입니다.");
            
            string dir = Path.Combine(SgaDirectory, SgaPackageType.Name[type]);

            if (!Directory.Exists(dir))
                throw new Exception("SGA 패키지 디렉토리가 존재하지 않습니다.");

            string[] sgaFileList = Directory.GetFiles(dir, "*.sga", SearchOption.TopDirectoryOnly);

            for (int i = 0; i < sgaFileList.Length; ++i)
            {
                SgaPackage loadedPackage = SgaLoader.Load(sgaFileList[i], false, true, i);
                _packages.Add(loadedPackage);
                PackageMap.Add(loadedPackage.FileName, loadedPackage);
            }
            NotifyUpdateList();
        }

        public void UnloadAll()
        {
            if (_packages == null)
                return;

            foreach (var sgaPackage in _packages)
                sgaPackage.UnloadAll();

            PackageMap.Clear();
            _packages = null;
            NotifyUpdateList();
        }

        public SgaPackage GetPackage(int index)
        {
            if (!IsValidPackageIndex(index))
                throw new Exception("올바르지 않은 패키지 인덱스입니다.");

            return Packages[index];
        }

        public SgaImage GetImg(string packageName, string imgName)
        {
            //if (!packageName.EndsWith(".sga"))
            //    packageName += ".sga";

            //if (!imgName.EndsWith(".img"))
            //    imgName += ".img";

            if (!IsValidPackageName(packageName))
                throw new Exception("올바르지 않은 패키지 이름입니다.");

            SgaImage img = PackageMap[packageName].GetElement(imgName) as SgaImage;

            if (img == null)
                throw new Exception("해당 SgaElement는 SgaImage타입이 아닙니다.");

            return img;
        }

        public SgaSpriteAbstract GetSprite(string packageName, string imgName, int spriteIndex)
            => GetImg(packageName, imgName).GetSprite(spriteIndex);

        public SgaSprite GetSpriteLink(string packageName, string imgName, int spriteIndex)
        {
            SgaImage img = GetImg(packageName, imgName);
            SgaSpriteAbstract sprite = img.GetSprite(spriteIndex);

            if (sprite.IsLink)
            {
                SgaSpriteAbstract link = img.GetSprite(sprite.TargetFrameIndex);

                if (link.IsLink)
                    throw new Exception("링크로 따라간 스프라이트가 링크 스프라이트입니다.");

                return link as SgaSprite;
            }

            return sprite as SgaSprite;
        }


        // 링크는 타겟 프레임 찾아서 비트맵 가져옴
        public BitmapSource GetBitmapSourceLink(string packageName, string imgName, int spriteIndex)
            => GetSpriteLink(packageName, imgName, spriteIndex).Source;

        bool IsValidPackageIndex(int index) => index >= 0 && index < Packages.Count;
        bool IsValidPackageName(string packageName) => PackageMap.ContainsKey(packageName);

        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(Packages));

    }
}
