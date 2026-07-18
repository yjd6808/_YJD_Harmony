using System;
using System.Collections.Generic;
using System.IO;
using Newtonsoft.Json;

namespace SGToolsUI.FileSystem
{
    public class SGUIMetaInfo
    {
        public string Name { get; set; } = "";
        public string XmlPath { get; set; } = "";
        public string LastModified { get; set; } = "";
    }

    public class SGUIMetaManager
    {
        public string BaseDirectory { get; set; } = "";

        public SGUIMetaManager(string _baseDirectory)
        {
            BaseDirectory = _baseDirectory;
        }

        public static SGUIMetaInfo LoadMeta(string _metaFilePath)
        {
            string json = File.ReadAllText(_metaFilePath);
            return JsonConvert.DeserializeObject<SGUIMetaInfo>(json)!;
        }

        public void SaveMeta(string _metaFilePath, SGUIMetaInfo _info)
        {
            string json = JsonConvert.SerializeObject(_info, Formatting.Indented);
            File.WriteAllText(_metaFilePath, json);
        }

        public List<SGUIMetaInfo> ScanAllMeta()
        {
            var results = new List<SGUIMetaInfo>();
            if (!Directory.Exists(BaseDirectory))
                return results;

            foreach (string metaFile in Directory.GetFiles(BaseDirectory, "*.uimeta", SearchOption.AllDirectories))
            {
                try
                {
                    results.Add(LoadMeta(metaFile));
                }
                catch { }
            }
            return results;
        }

        public SGUIMetaInfo CreateMeta(string _name, string _xmlRelativePath)
        {
            return new SGUIMetaInfo
            {
                Name = _name,
                XmlPath = _xmlRelativePath,
                LastModified = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")
            };
        }
    }
}
