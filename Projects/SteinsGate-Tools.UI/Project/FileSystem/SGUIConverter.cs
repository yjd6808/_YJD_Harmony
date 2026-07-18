using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Xml.Linq;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsUI.Model.Main;

namespace SGToolsUI.FileSystem
{
    public static class SGUIConverter
    {
        public static async Task ConvertAsync(string _jsonPath, string _outputDir)
        {
            await Task.Run(() =>
            {
                if (!Directory.Exists(_outputDir))
                    Directory.CreateDirectory(_outputDir);

                string json = File.ReadAllText(_jsonPath);
                JObject root = JObject.Parse(json);

                var elementDict = new Dictionary<int, (JToken token, int typeInt)>();
                var parentMap = new Dictionary<int, int>();
                var positionMap = new Dictionary<int, IntPoint>();

                JArray elements = (JArray)root["elements"]!;
                foreach (JObject elem in elements)
                {
                    int code = (int)elem["code"]!;
                    elementDict[code] = (elem, (int)elem["type"]!);
                }

                JArray groups = (JArray)root["groups"]!;
                foreach (JObject grp in groups)
                {
                    int code = (int)grp["code"]!;
                    elementDict[code] = (grp, 0);
                }

                foreach (var (code, (token, typeInt)) in elementDict)
                {
                    if (typeInt != 0) continue;
                    JArray? children = token["children"] as JArray;
                    if (children == null) continue;

                    foreach (JToken childToken in children)
                    {
                        string childInfo = (string)childToken;
                        int[] parts = new int[3];
                        StringEx.ParseIntNumberN(childInfo, parts);
                        parentMap[parts[0]] = code;
                        positionMap[parts[0]] = new IntPoint(parts[1], parts[2]);
                    }
                }

                JObject groupMaster = (JObject)root["group_master"]!;
                JArray masterChildren = (JArray)groupMaster["children"]!;

                int rootIndex = 0;
                foreach (JToken childToken in masterChildren)
                {
                    string childInfo = (string)childToken;
                    int[] parts = new int[3];
                    StringEx.ParseIntNumberN(childInfo, parts);
                    int rootCode = parts[0];

                    XElement rootXml = BuildXElement(rootCode, elementDict, positionMap);
                    if (rootXml == null) continue;

                    string name = (string)rootXml.Attribute("name") ?? "";
                    if (string.IsNullOrWhiteSpace(name))
                        name = $"RootGroup{rootIndex}";
                    string fileName = SanitizeFileName(name);

                    string xmlPath = Path.Combine(_outputDir, $"{fileName}.xml");
                    rootXml.Save(xmlPath);

                    var meta = new SGUIMetaInfo
                    {
                        Name = name,
                        XmlPath = $"{fileName}.xml",
                        LastModified = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss")
                    };
                    string metaJson = JsonConvert.SerializeObject(meta, Formatting.Indented);
                    File.WriteAllText(Path.Combine(_outputDir, $"{fileName}.uimeta"), metaJson);

                    rootIndex++;
                }
            });
        }

        private static XElement? BuildXElement(int _code, Dictionary<int, (JToken token, int typeInt)> _dict,
            Dictionary<int, IntPoint> _positionMap)
        {
            if (!_dict.TryGetValue(_code, out var entry))
                return null;

            var (token, typeInt) = entry;
            SGUIElementType type = (SGUIElementType)typeInt;
            string tagName = TypeToTagName(type);
            XElement xml = new XElement(tagName);

            string dname = (string)token["dname"] ?? string.Empty;
            if (!string.IsNullOrEmpty(dname))
                xml.SetAttributeValue("name", dname);
            else
                xml.SetAttributeValue("name", (string)token["name"] ?? string.Empty);

            if (_positionMap.TryGetValue(_code, out var pos))
            {
                xml.SetAttributeValue("x", pos.X);
                xml.SetAttributeValue("y", pos.Y);
            }

            if (token["valign"] != null)
                xml.SetAttributeValue("valign", (int)token["valign"]);
            if (token["halign"] != null)
                xml.SetAttributeValue("halign", (int)token["halign"]);

            switch (type)
            {
                case SGUIElementType.Group:
                    ApplyGroupProperties(xml, token, _dict, _positionMap);
                    break;
                case SGUIElementType.Button:
                    ApplyButtonProperties(xml, token);
                    break;
                case SGUIElementType.Label:
                    ApplyLabelProperties(xml, token);
                    break;
                case SGUIElementType.Sprite:
                    ApplySpriteProperties(xml, token);
                    break;
                case SGUIElementType.EditBox:
                    ApplyEditBoxProperties(xml, token);
                    break;
                case SGUIElementType.CheckBox:
                    ApplyCheckBoxProperties(xml, token);
                    break;
                case SGUIElementType.ToggleButton:
                    ApplyToggleButtonProperties(xml, token);
                    break;
                case SGUIElementType.ScrollBar:
                    ApplyScrollBarProperties(xml, token);
                    break;
                case SGUIElementType.ProgressBar:
                    ApplyProgressBarProperties(xml, token);
                    break;
                case SGUIElementType.Static:
                    ApplyStaticProperties(xml, token);
                    break;
            }

            return xml;
        }

        private static void ApplyGroupProperties(XElement _xml, JToken _token,
            Dictionary<int, (JToken token, int typeInt)> _dict,
            Dictionary<int, IntPoint> _positionMap)
        {
            string? sizeStr = _token["size"]?.ToString();
            if (!string.IsNullOrEmpty(sizeStr))
            {
                string[] sizeParts = SplitNumbers(sizeStr);
                if (sizeParts.Length >= 2)
                {
                    _xml.SetAttributeValue("width", int.Parse(sizeParts[0]));
                    _xml.SetAttributeValue("height", int.Parse(sizeParts[1]));
                }
            }

            JArray? children = _token["children"] as JArray;
            if (children == null) return;

            foreach (JToken childToken in children)
            {
                string childInfo = (string)childToken;
                int[] parts = new int[3];
                StringEx.ParseIntNumberN(childInfo, parts);
                XElement? childXml = BuildXElement(parts[0], _dict, _positionMap);
                if (childXml != null)
                    _xml.Add(childXml);
            }
        }

        private static void ApplySpriteProperties(XElement _xml, JToken _token)
        {
            ApplySgaAttributes(_xml, _token);
            ApplySizeAttributes(_xml, _token);
            if (_token["linear_dodge"] != null && (bool)_token["linear_dodge"])
                _xml.SetAttributeValue("linear_dodge", true);
            if (_token["scale9"] != null && (bool)_token["scale9"])
                _xml.SetAttributeValue("scale9", true);
        }

        private static void ApplyButtonProperties(XElement _xml, JToken _token)
        {
            ApplySgaAttributes(_xml, _token);
            if (_token["linear_dodge"] != null && (bool)_token["linear_dodge"])
                _xml.SetAttributeValue("linear_dodge", true);
        }

        private static void ApplyLabelProperties(XElement _xml, JToken _token)
        {
            ApplySizeAttributes(_xml, _token);
            CopyAttribute(_xml, _token, "font");
            CopyAttribute(_xml, _token, "font_size");
            CopyAttribute(_xml, _token, "font_color");
            CopyAttribute(_xml, _token, "text");
            CopyAttribute(_xml, _token, "text_wrap");
            CopyAttribute(_xml, _token, "text_valign");
            CopyAttribute(_xml, _token, "text_halign");
        }

        private static void ApplyEditBoxProperties(XElement _xml, JToken _token)
        {
            ApplySizeAttributes(_xml, _token);
            CopyAttribute(_xml, _token, "text_halign");
            CopyAttribute(_xml, _token, "font_size");
            CopyAttribute(_xml, _token, "font_color");
            CopyAttribute(_xml, _token, "p_text");
            CopyAttribute(_xml, _token, "p_font_color");
            CopyAttribute(_xml, _token, "p_font_size");
            CopyAttribute(_xml, _token, "max_length");
            CopyAttribute(_xml, _token, "input_mode");
        }

        private static void ApplyCheckBoxProperties(XElement _xml, JToken _token)
        {
            if (_token["check"] != null && (bool)_token["check"])
                _xml.SetAttributeValue("check", true);
            CopyAttribute(_xml, _token, "bg_sga");
            CopyAttribute(_xml, _token, "bg_img");
            CopyAttribute(_xml, _token, "cross_sga");
            CopyAttribute(_xml, _token, "cross_img");
            CopyAttribute(_xml, _token, "sprite");
        }

        private static void ApplyToggleButtonProperties(XElement _xml, JToken _token)
        {
            ApplySgaAttributes(_xml, _token);
            CopyAttribute(_xml, _token, "sprite2");
            if (_token["linear_dodge"] != null && (bool)_token["linear_dodge"])
                _xml.SetAttributeValue("linear_dodge", true);
        }

        private static void ApplyProgressBarProperties(XElement _xml, JToken _token)
        {
            ApplySgaAttributes(_xml, _token);
            ApplySizeAttributes(_xml, _token);
            CopyAttribute(_xml, _token, "direction");
        }

        private static void ApplyScrollBarProperties(XElement _xml, JToken _token)
        {
            ApplySgaAttributes(_xml, _token);
            CopyAttribute(_xml, _token, "track_size");
        }

        private static void ApplyStaticProperties(XElement _xml, JToken _token)
        {
            ApplySizeAttributes(_xml, _token);
        }

        private static void ApplySgaAttributes(XElement _xml, JToken _token)
        {
            CopyAttribute(_xml, _token, "sga");
            CopyAttribute(_xml, _token, "img");
            CopyAttribute(_xml, _token, "sprite");
        }

        private static void ApplySizeAttributes(XElement _xml, JToken _token)
        {
            string? sizeStr = _token["size"]?.ToString();
            if (!string.IsNullOrEmpty(sizeStr))
            {
                string[] parts = SplitNumbers(sizeStr);
                if (parts.Length >= 2)
                {
                    _xml.SetAttributeValue("width", int.Parse(parts[0]));
                    _xml.SetAttributeValue("height", int.Parse(parts[1]));
                }
            }
        }

        private static void CopyAttribute(XElement _xml, JToken _token, string _key)
        {
            JToken? value = _token[_key];
            if (value != null)
                _xml.SetAttributeValue(_key, value.ToString());
        }

        private static string[] SplitNumbers(string _input)
        {
            return _input.Split(new[] { ' ', ',' }, StringSplitOptions.RemoveEmptyEntries);
        }

        private static string SanitizeFileName(string _name)
        {
            foreach (char c in Path.GetInvalidFileNameChars())
                _name = _name.Replace(c, '_');
            return _name;
        }

        private static string TypeToTagName(SGUIElementType _type)
        {
            return _type switch
            {
                SGUIElementType.Group => "Group",
                SGUIElementType.Button => "Button",
                SGUIElementType.Label => "Label",
                SGUIElementType.Sprite => "Sprite",
                SGUIElementType.EditBox => "EditBox",
                SGUIElementType.CheckBox => "CheckBox",
                SGUIElementType.ToggleButton => "ToggleButton",
                SGUIElementType.ScrollBar => "ScrollBar",
                SGUIElementType.ProgressBar => "ProgressBar",
                SGUIElementType.Static => "Static",
                _ => "Group"
            };
        }
    }
}
