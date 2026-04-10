/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:52:49 AM
 *
 */

using System.IO;

namespace SGToolsCommon.Sga
{
    public class SgaSound : SgaElement
    {
        private byte[] data_;

        public byte[] Data => data_;

        //////////////////////////////////////////////////////////////////////////////////
        public SgaSound(SgaPackage _parent, SgaElementHeader _header, int _version, int _indexOffset, int _indexLength)
            : base(SgaElementType.Sound, _parent, _header, _version, _indexOffset, _indexLength)
        {
            data_ = null;
            indexLoaded_ = true;
            dataLoaded_ = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Load(bool _indexOnly)
        {
            if (_indexOnly)
                return;

            data_ = new byte[IndexLength];
            dataLoaded_ = true;
            parent_.ReadStream.Seek(IndexOffset, SeekOrigin.Begin);
            parent_.ReadStream.Read(data_, 0, IndexLength);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public override void Unload()
        {
            data_ = null;
            dataLoaded_ = false;
        }
    }
}
