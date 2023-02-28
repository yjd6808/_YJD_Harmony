/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:18:09 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
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

namespace SGToolsCommon.Sga
{
    public abstract class SgaElement
    {
        // Xaml 바인딩용
        public SgaElement()
        {
        }

        public SgaElement(SgaElementType type, SgaPackage parent, SgaElementHeader header, int version, int indexOffset, int indexLength)
        {
            _parent = parent;

            Type = type;
            Header = header;
            Version = version;
            IndexOffset = indexOffset;
            IndexLength = indexLength;
        }

        protected bool _dataLoaded;
        protected bool _indexLoaded;
        protected SgaPackage _parent;

        public SgaPackage Parent => _parent;
        public SgaElementType Type { get; }
        public SgaElementHeader Header { get; }

        public int Version { get; }
        public int IndexOffset { get; }
        public int IndexLength { get; }
        public bool DataLoaded => _dataLoaded;
        public bool IndexLoaded => _indexLoaded;
        

        public abstract void Load(bool indexOnly);
        public abstract void Unload();
        public void LoadPerfectly() => Load(false);
        public void LoadIndexOnly() => Load(true);
        
    }
}
