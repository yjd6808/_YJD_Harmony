/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 3:10:00 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;

using SGToolsCommon.Extension;

namespace SGToolsCommon.Sga
{
    public class SgaPackage : Bindable
    {
        private Stream readStream_ = null!;
        private string path_ = null!;
        private Dictionary<int, SgaElement> elementMap_ = null!;
        private int elementCount_;
        private int packageIndex_;
        private bool indexLoaded_;

        // Lazy Loading 용
        private List<SgaElementHeader> elementHeaderList_ = null!;
        private Dictionary<string, int> elementNameToIndexMap_ = null!;

        public Stream ReadStream => readStream_;
        public string Path => path_;
        public int ElementCount => elementCount_;
        public bool Loaded => elementMap_.Count > 0;
        public bool IndexLoaded => indexLoaded_;
        public string FileName => System.IO.Path.GetFileName(path_);
        public string FileNameWithoutExt => System.IO.Path.GetFileNameWithoutExtension(path_);
        public List<SgaElementHeader> ElementHeaderList => elementHeaderList_;
        public int PackageIndex => packageIndex_;

        //////////////////////////////////////////////////////////////////////////////////
        // Xaml 바인딩용
        public SgaPackage()
        {
            elementHeaderList_ = new();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaPackage(Stream _readStream, string _path, int _elementCount, int _packageIndex)
        {
            elementMap_ = new();
            elementHeaderList_ = new();
            elementNameToIndexMap_ = new();

            elementCount_ = _elementCount;
            readStream_ = _readStream;
            path_ = _path;
            indexLoaded_ = false;
            packageIndex_ = _packageIndex;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadIndex()
        {
            elementHeaderList_.Clear();

            for (int i = 0; i < elementCount_; ++i)
            {
                int offset = readStream_.ReadInt();
                int length = readStream_.ReadInt();
                string name = SgaLoader.ReadElementPath(readStream_);

                elementHeaderList_.Add(new SgaElementHeader()
                {
                    Offset = offset,
                    NextOffset = 0,
                    IndexInPackage = i,
                    Length = length,
                    Name = name,
                    NameWithoutExt = System.IO.Path.GetFileNameWithoutExtension(name)
                });
            }

            for (int i = 0; i < elementCount_; ++i)
            {
                SgaElementHeader header = elementHeaderList_[i];

                header.NextOffset =
                    i < elementCount_ - 1 ? elementHeaderList_[i + 1].Offset : (int)readStream_.Length;

                elementNameToIndexMap_.Add(header.Name, header.IndexInPackage);
            }

            indexLoaded_ = true;
            OnPropertyChanged(nameof(ElementHeaderList));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void UnloadAll()
        {
            foreach (SgaElement element in elementMap_.Values)
                element.Unload();
            OnPropertyChanged(nameof(ElementHeaderList));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public SgaElement GetElement(int _index) => elementMap_[_index];

        //////////////////////////////////////////////////////////////////////////////////
        public SgaElement GetElement(string _elementName)
        {
            if (!Loaded)
                Load(true);

            return elementMap_[GetElementIndex(_elementName)];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public int GetElementIndex(string _elementName)
        {
            if (!elementNameToIndexMap_.ContainsKey(_elementName))
                throw new Exception($"{_elementName}을 패키지${FileName}에서 찾지 못했습니다.");

            return elementNameToIndexMap_[_elementName];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool HasElementIndex(string _elementName) => elementNameToIndexMap_.ContainsKey(_elementName);

        //////////////////////////////////////////////////////////////////////////////////
        public bool IsElementLoaded(int _index) => elementMap_.ContainsKey(_index);

        //////////////////////////////////////////////////////////////////////////////////
        public bool HasElement(int _index) => _index >= 0 && _index < elementHeaderList_.Count;

        //////////////////////////////////////////////////////////////////////////////////
        public void Load(bool _indexOnly)
        {
            if (!indexLoaded_)
                LoadIndex();

            for (int i = 0; i < elementCount_; ++i)
            {
                SgaElementHeader header = elementHeaderList_[i];
                SgaElement element = SgaLoader.ReadElement(this, readStream_, header, header.NextOffset, _indexOnly);
                elementMap_.Add(header.IndexInPackage, element);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadElementIfNotLoaded(int _index, bool _indexOnly)
        {
            if (!IsElementLoaded(_index))
                LoadElement(_index, _indexOnly);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void LoadElement(int _index, bool _indexOnly)
        {
            if (!indexLoaded_)
                LoadIndex();

            if (_index < 0 || _index >= elementHeaderList_.Count)
                throw new IndexOutOfRangeException($"{FileName} 패키지에서 {_index} 엘리멘트헤더 정보를 가져오지못했습니다. 이상합니다.");

            SgaElementHeader header = elementHeaderList_[_index];

            if (elementMap_.ContainsKey(_index))
                throw new Exception($"{FileName} 패키지 키:{_index}{header.Name} 엘리멘트가 이미 로딩되어 있습니다. 중복 로딩을 시도하셨습니다.");

            SgaElement element = SgaLoader.ReadElement(this, readStream_, header, header.NextOffset, _indexOnly);
            elementMap_.Add(header.IndexInPackage, element);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(ElementHeaderList));

        //////////////////////////////////////////////////////////////////////////////////
        public override string ToString()
            => FileNameWithoutExt;
    }
}
