/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 3:10:00 AM
 *
 */

using System;
using System.Collections.Generic;
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
using System.Windows.Shapes;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Sga
{
    public class SgaPackage
    {
        private Stream _readStream;
        private string _path;
        private Dictionary<int, SgaElement> _elementMap;
        private int _elementCount;
        private bool _indexLoaded;

        // Lazy Loading 용
        private List<SgaElementHeader> _elementHeaderList;
        private Dictionary<string, int> _elementNameToIndexMap;

        public Stream ReadStream => _readStream;
        public string Path => _path;
        public int ElementCount => _elementCount;
        public bool Loaded => _elementMap.Count > 0;
        public bool IndexLoaded => _indexLoaded;

        public SgaPackage(Stream readStream, string path, int elementCount)
        {
            _elementMap = new();
            _elementHeaderList = new(elementCount);
            _elementNameToIndexMap = new();

            _readStream = readStream;
            _path = path;
            _indexLoaded = false;
        }



        public void LoadIndex()
        {
            _elementHeaderList.Clear();

            for (int i = 0; i < _elementCount; ++i)
            {
                int offset = _readStream.ReadInt();
                int length = _readStream.ReadInt();
                string path = SgaLoader.ReadElementPath(_readStream);
                string name = _readStream.ReadString();

                _elementHeaderList.Add(new SgaElementHeader()
                {
                    Offset = offset,
                    NextOffset = 0,
                    IndexInPackage = i,
                    Length = length,
                    Name = name
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
        }


        public void UnloadAll()
        {
            foreach (var element in _elementMap.Values)
                element.Unload();
        }

        public int GetElementIndex(string elementName) => _elementNameToIndexMap[elementName];
        public bool HasElementIndex(string elementName) => _elementNameToIndexMap.ContainsKey(elementName);
        public bool IsElementLoaded(int index) => _elementMap.ContainsKey(index);

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
    }
}
