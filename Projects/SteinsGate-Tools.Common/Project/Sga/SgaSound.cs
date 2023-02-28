/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:52:49 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
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
    public class SgaSound : SgaElement
    {
        private byte[] _data;

        public byte[] Data => _data;

        public SgaSound(SgaPackage parent, SgaElementHeader header, int version, int indexOffset, int indexLength) 
            : base(SgaElementType.Sound, parent, header, version, indexOffset, indexLength)
        {
            _data = null;
            _indexLoaded = true;
            _dataLoaded = false;
        }

        public override void Load(bool indexOnly)
        {
            if (indexOnly)
                return;

            _data = new byte[IndexLength];
            _dataLoaded = true;
            _parent.ReadStream.Seek(IndexOffset, SeekOrigin.Begin);
            _parent.ReadStream.Read(_data, 0, IndexLength);
        }

        public override void Unload()
        {
            _data = null;
            _dataLoaded = false;
        }
    }
}
