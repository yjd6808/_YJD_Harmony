/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:18:09 AM
 *
 */

using SGToolsCommon.Sga;

namespace SGToolsCommon.Sga
{
    public abstract class SgaElement : Bindable
    {
        protected bool dataLoaded_;
        protected bool indexLoaded_;
        protected SgaPackage parent_ = null!;

        public SgaPackage Parent => parent_;
        public SgaElementType Type { get; }
        public SgaElementHeader Header { get; }
        public int Version { get; }
        public int IndexOffset { get; }
        public int IndexLength { get; }
        public bool DataLoaded => dataLoaded_;
        public bool IndexLoaded => indexLoaded_;

        //////////////////////////////////////////////////////////////////////////////////
        // Xaml 바인딩용
        public SgaElement()
        {
            Header = new SgaElementHeader();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaElement(SgaElementType _type, SgaPackage _parent, SgaElementHeader _header, int _version, int _indexOffset, int _indexLength)
        {
            parent_ = _parent;
            Type = _type;
            Header = _header;
            Version = _version;
            IndexOffset = _indexOffset;
            IndexLength = _indexLength;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public abstract void Load(bool _indexOnly);

        //////////////////////////////////////////////////////////////////////////////////
        public abstract void Unload();

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadPerfectly() => Load(false);

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadIndexOnly() => Load(true);
    }
}
