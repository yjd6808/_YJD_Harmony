/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 3:10:00 AM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.IO.Packaging;
using System.Linq;
using System.Net.Security;
using System.Reflection.PortableExecutable;
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
using SGToolsCommon.Extension;

namespace SGToolsCommon.Sga
{
    public class SgaPackage : Bindable
    {
        private Stream _readStream;
        private string _path;
        private Dictionary<int, SgaElement> _elementMap;
        private int _elementCount;
        private int _packageIndex;
        private bool _indexLoaded;
        

        // Lazy Loading 용
        private List<SgaElementHeader> _elementHeaderList;
        private Dictionary<string, int> _elementNameToIndexMap;

        public Stream ReadStream => _readStream;
        public string Path => _path;
        public int ElementCount => _elementCount;
        public bool Loaded => _elementMap.Count > 0;
        public bool IndexLoaded => _indexLoaded;
        public string FileName => System.IO.Path.GetFileName(_path);
        public string FileNameWithoutExt => System.IO.Path.GetFileNameWithoutExtension(_path);
        public List<SgaElementHeader> ElementHeaderList => _elementHeaderList;
        public int PackageIndex => _packageIndex;
        // public ListBoxItem Item { get; set; }

        // Xaml 바인딩용
        public SgaPackage()
        {
            _elementHeaderList = new();
        }

        public SgaPackage(Stream readStream, string path, int elementCount, int packageIndex)
        {
            _elementMap = new();
            _elementHeaderList = new();
            _elementNameToIndexMap = new();

            _elementCount = elementCount;
            _readStream = readStream;
            _path = path;
            _indexLoaded = false;
            _packageIndex = packageIndex;
        }



        public void LoadIndex()
        {
            _elementHeaderList.Clear();

            for (int i = 0; i < _elementCount; ++i)
            {
                int offset = _readStream.ReadInt();
                int length = _readStream.ReadInt();
                string name = SgaLoader.ReadElementPath(_readStream);

                _elementHeaderList.Add(new SgaElementHeader()
                {
                    Offset = offset,
                    NextOffset = 0,
                    IndexInPackage = i,
                    Length = length,
                    Name = name,
                    NameWithoutExt = System.IO.Path.GetFileNameWithoutExtension(name)
                });
            }

            for (int i = 0; i < _elementCount; ++i)
            {
                SgaElementHeader header = _elementHeaderList[i];

                header.NextOffset =
                    i < _elementCount - 1 ? _elementHeaderList[i + 1].Offset : (int)_readStream.Length;

                _elementNameToIndexMap.Add(header.Name, header.IndexInPackage);
            }

            _indexLoaded = true;
            OnPropertyChanged(nameof(ElementHeaderList));
        }


        public void UnloadAll()
        {
            foreach (var element in _elementMap.Values)
                element.Unload();
            OnPropertyChanged(nameof(ElementHeaderList));
        }

        public SgaElement GetElement(int index) => _elementMap[index];

        public SgaElement GetElement(string elementName)
        {
            if (!Loaded)
                Load(true);

            return _elementMap[GetElementIndex(elementName)];
        } 

        public int GetElementIndex(string elementName)
        {
            if (!_elementNameToIndexMap.ContainsKey(elementName))
                throw new Exception($"{elementName}을 패키지${FileName}에서 찾지 못했습니다.");

            return _elementNameToIndexMap[elementName];
        }
        public bool HasElementIndex(string elementName) => _elementNameToIndexMap.ContainsKey(elementName);
        public bool IsElementLoaded(int index) => _elementMap.ContainsKey(index);
        public bool HasElement(int index) => index >= 0 && index < _elementHeaderList.Count;

        public void Load(bool indexOnly)
        {
            if (!_indexLoaded)
                LoadIndex();

            for (int i = 0; i < _elementCount; ++i)
            {
                SgaElementHeader header = _elementHeaderList[i];
                SgaElement element = SgaLoader.ReadElement(this, _readStream, header, header.NextOffset, indexOnly);
                _elementMap.Add(header.IndexInPackage, element);
            }
        }

        public void LoadElementIfNotLoaded(int index, bool indexOnly)
        {
            if (!IsElementLoaded(index))
                LoadElement(index, indexOnly);
        }

        public void LoadElement(int index, bool indexOnly)
        {
            if (!_indexLoaded)
                LoadIndex();

            if (index < 0 || index >= _elementHeaderList.Count)
                throw new IndexOutOfRangeException($"{FileName} 패키지에서 {index} 엘리멘트헤더 정보를 가져오지못했습니다. 이상합니다.");

            SgaElementHeader header = _elementHeaderList[index];

            if (_elementMap.ContainsKey(index))
                throw new Exception($"{FileName} 패키지 키:{index}{header.Name} 엘리멘트가 이미 로딩되어 있습니다. 중복 로딩을 시도하셨습니다.");

            SgaElement element = SgaLoader.ReadElement(this, _readStream, header, header.NextOffset, indexOnly);
            _elementMap.Add(header.IndexInPackage, element);
        }

        public void NotifyUpdateList()
            => OnPropertyChanged(nameof(ElementHeaderList));

        public override string ToString()
            => FileNameWithoutExt;
    }
}
