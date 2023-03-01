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
    public class SgaManager
    {
        private SgaManager() {}
        public static SgaManager Instance = new ();

        public List<SgaPackage> Packages { get; } = new();
        public Dictionary<string, SgaPackage> PackageMap { get; } = new();
        public string SgaDirectory { get; set; } = string.Empty;
        public Dispatcher Dispatcher { get; set; }

        public void LoadSga(int type)
        {
            if (Packages.Count > 0)
                throw new Exception("이미 로딩되었습니다.");

            if (type < 0 || type >= SgaPackageType.Max)
                throw new Exception("올바르지 않은 sga 패키지 타입입니다.");
            
            string dir = Path.Combine(SgaDirectory, SgaPackageType.Name[type]);

            if (!Directory.Exists(dir))
                throw new Exception("SGA 패키지 디렉토리가 존재하지 않습니다.");

            string[] sgaFileList = Directory.GetFiles(dir, "*.sga", SearchOption.TopDirectoryOnly);

            for (int i = 0; i < sgaFileList.Length; ++i)
            {
                SgaPackage loadedPackage = SgaLoader.Load(sgaFileList[i], false, true, i);
                Packages.Add(loadedPackage);
                PackageMap.Add(loadedPackage.FileNameWithoutExt, loadedPackage);
            }
        }

        public void UnloadAll()
        {
            foreach (var sgaPackage in Packages)
                sgaPackage.UnloadAll();

            Packages.Clear();
        }

        public SgaPackage GetPackage(int index)
        {
            if (!IsValidPackageIndex(index))
                throw new Exception("올바르지 않은 패키지 인덱스입니다.");

            return Packages[index];
        }

        public SgaImage GetImg(string packageNameWithoutExt, string imgNameWithoutExt)
        {
            if (!IsValidPackageName(packageNameWithoutExt))
                throw new Exception("올바르지 않은 패키지 이름입니다.");

            SgaImage img = PackageMap[packageNameWithoutExt].GetElement(imgNameWithoutExt) as SgaImage;

            if (img == null)
                throw new Exception("해당 SgaElement는 SgaImage타입이 아닙니다.");

            return img;
        }

        public SgaSpriteAbstract GetSprite(string packageNameWithoutExt, string imgNameWithoutExt, int spriteIndex)
            => GetImg(packageNameWithoutExt, imgNameWithoutExt).GetSprite(spriteIndex);

        public SgaSprite GetSpriteLink(string packageNameWithoutExt, string imgNameWithoutExt, int spriteIndex)
        {
            SgaImage img = GetImg(packageNameWithoutExt, imgNameWithoutExt);
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
        public BitmapSource GetBitmapSourceLink(string packageNameWithoutExt, string imgNameWithoutExt, int spriteIndex)
            => GetSpriteLink(packageNameWithoutExt, imgNameWithoutExt, spriteIndex).Source;

        bool IsValidPackageIndex(int index) => index >= 0 && index < Packages.Count;
        bool IsValidPackageName(string packageNameWithoutExt) => PackageMap.ContainsKey(packageNameWithoutExt);
    }
}
