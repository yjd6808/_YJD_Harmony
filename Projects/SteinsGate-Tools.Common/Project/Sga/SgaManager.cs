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
        public string SgaDirectory { get; set; } = string.Empty;
        public Dispatcher Dispatcher { get; set; }

        // =========================================================
        // 기능 (무조건 디스패쳐 쓰레드에서만 아래 함수들 호출할 것)
        // =========================================================
        public void LoadSga(int type)
        {
            if (Packages.Count > 0)
                throw new Exception("이미 로딩되었습니다.");

            if (type < 0 || type >= SgaPackageType.Max)
                throw new Exception("올바르지 않은 sga 패키지 타입입니다.");
            
            string dir = Path.Combine(SgaDirectory, SgaPackageType.Name[type]);

            if (!Directory.Exists(dir))
                throw new Exception("SGA 패키지 디렉토리가 존재하지 않습니다.");

            foreach (string sgaFile in Directory.GetFiles(dir, "*.sga", SearchOption.TopDirectoryOnly))
                Packages.Add(SgaLoader.Load(sgaFile, false, true));
        }

        public void UnloadAll()
        {
            foreach (var sgaPackage in Packages)
                sgaPackage.UnloadAll();

            Packages.Clear();
        }

        public SgaPackage GetPackage(int index)
        {
            return Packages[index];
        }

    }
}
