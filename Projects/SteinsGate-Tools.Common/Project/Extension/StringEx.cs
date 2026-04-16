/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:55:41 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;

using MoreLinq;

using Newtonsoft.Json.Linq;

using SGToolsCommon.Base;

namespace SGToolsCommon.Extension
{
    public enum TemplateSingatureParamBasicType
    {
        Unknown,
        Bool,
        Char,
        WChar,
        Short,
        Int,
        Long,
        LongLong,
        Int64,
        Float,
        Double,
        LongDouble,
        UChar,
        UShort,
        UInt,
        ULong,
        ULongLong,
        UInt64,
        Char8,
        Char16,
        Char32
    }

    public enum TemplateSignatureParamType
    {
        Unknown,
        Placeholder,
        Basic,
        UDT
    }

    public struct TemplateSignatureParam
    {
        public TemplateSignatureParamType type_ = TemplateSignatureParamType.Unknown;
        public TemplateSingatureParamBasicType basicType_ = TemplateSingatureParamBasicType.Unknown;
        public string udtName_ = string.Empty;
        public TemplateSignatureParam() {}
    }

    public struct WordWithPos
    {
        public int Offset { get; } = -1;
        public string Word { get; }
        public WordWithPos(int _offset, string _word)
        {
            Offset = _offset;
            Word = _word;
        }
    }

    public static class StringEx
    {
        public const string HEX_CHAR = "0123456789ABCDEF";
        public static readonly Dictionary<string, TemplateSingatureParamBasicType> primitiveTypes = new()
        {
            { "bool", TemplateSingatureParamBasicType.Bool },
            { "char", TemplateSingatureParamBasicType.Char },
            { "wchar_t", TemplateSingatureParamBasicType.WChar },
            { "short", TemplateSingatureParamBasicType.Short },
            { "int", TemplateSingatureParamBasicType.Int },
            { "long", TemplateSingatureParamBasicType.Long },
            { "long long", TemplateSingatureParamBasicType.Int64 },
            { "__int64", TemplateSingatureParamBasicType.Int64 },
            { "float", TemplateSingatureParamBasicType.Float },
            { "double", TemplateSingatureParamBasicType.Double },
            { "long double", TemplateSingatureParamBasicType.LongDouble },
            { "unsigned char", TemplateSingatureParamBasicType.UChar },
            { "unsigned short", TemplateSingatureParamBasicType.UShort },
            { "unsigned int", TemplateSingatureParamBasicType.UInt },
            { "unsigned long", TemplateSingatureParamBasicType.ULong },
            { "unsigned long long", TemplateSingatureParamBasicType.UInt64 },
            { "unsigned __int64", TemplateSingatureParamBasicType.UInt64 },
            { "char8_t", TemplateSingatureParamBasicType.Char8 },
            { "char16_t", TemplateSingatureParamBasicType.Char16 },
            { "char32_t", TemplateSingatureParamBasicType.Char32 }
        };

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumberN(string _target, int[] _arr)
        {
            string temp = string.Empty;
            int readCount = 0;

            for (int i = 0; i < _target.Length; ++i)
            {
                temp += _target[i];

                if (_target[i] == ' ')
                {
                    _arr[readCount++] = int.Parse(temp);
                    temp = string.Empty;
                }
            }

            _arr[readCount++] = int.Parse(temp);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string NumberNToString<T>(T[] _arr)
        {
            string str = string.Empty;
            _arr.ForEach((num, index) =>
            {
                str += num;

                if (index < _arr.Length - 1)
                    str += ' ';
            });

            return str;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseDoubleNumberN(string _target, double[] _arr)
        {
            string temp = string.Empty;
            int readCount = 0;

            for (int i = 0; i < _target.Length; ++i)
            {
                temp += _target[i];

                if (_target[i] == ' ')
                {
                    _arr[readCount++] = double.Parse(temp);
                    temp = string.Empty;
                }
            }

            _arr[readCount++] = double.Parse(temp);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber2(string _s, out int _num1, out int _num2)
        {
            int[] nums = new int[2];
            ParseIntNumberN(_s, nums);
            _num1 = nums[0];
            _num2 = nums[1];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber3(string _s, out int _num1, out int _num2, out int _num3)
        {
            int[] nums = new int[3];
            ParseIntNumberN(_s, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber4(string _s, out int _num1, out int _num2, out int _num3, out int _num4)
        {
            int[] nums = new int[4];
            ParseIntNumberN(_s, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
            _num4 = nums[3];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber2(JToken? _token, out int _num1, out int _num2)
        {
            int[] nums = new int[2];

            if (_token!.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token!, nums);
            _num1 = nums[0];
            _num2 = nums[1];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber3(JToken? _token, out int _num1, out int _num2, out int _num3)
        {
            int[] nums = new int[3];

            if (_token!.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token!, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber4(JToken? _token, out int _num1, out int _num2, out int _num3, out int _num4)
        {
            int[] nums = new int[4];

            if (_token!.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token!, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
            _num4 = nums[3];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseDoubleNumber4(JToken? _token, out double _num1, out double _num2, out double _num3, out double _num4)
        {
            double[] nums = new double[4];

            if (_token!.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)_token!, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
            _num4 = nums[3];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseDoubleNumber4(JToken? _token, out double _num1, out double _num2, out double _num3)
        {
            double[] nums = new double[3];

            if (_token!.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)_token!, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static string RandomAlphabatString(int _len, bool _containNumber = false)
        {
            StringBuilder builder = new StringBuilder(_len);
            Random random = new Random((int)DateTime.Now.Ticks);

            for (int i = 0; i < _len; i++)
            {
                int idx = 0;
                char ch = 'a';

                if (_containNumber)
                    idx = random.Next(0, 2);

                if (idx == 0)
                    ch = (char)('0' + random.Next(0, 10));
                else if (idx == 1)
                    ch = (char)('a' + random.Next(0, 27));

                builder.Append(ch);
            }

            return builder.ToString();
        }

        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/19592404/escape-characters-in-string-being-escaped
        public static string? Unescape(this string _value)
        {
            if (_value == null)
                return null;

            int length = _value.Length;
            StringBuilder result = new StringBuilder(length);

            for (int i = 0; i < length; i++)
            {
                char c = _value[i];

                if (c == '\\' && i++ < length)
                {
                    c = _value[i];

                    switch (c)
                    {
                        case 'n':
                            result.Append('\n');
                            break;
                        case 'r':
                            result.Append('\r');
                            break;
                        case 't':
                            result.Append('\t');
                            break;
                        case '\\':
                            result.Append('\\');
                            break;
                        default:
                            result.Append(c);
                            break;
                    }
                }
                else
                {
                    result.Append(c);
                }
            }

            return result.ToString();
        }

        public static string RemoveAllBlanks(string _str)
        {
            return new string(_str.Where(_c => !char.IsWhiteSpace(_c)).ToArray());
        }

        public static bool IsValidTypeName(string _str)
        {
            if (_str.Length == 0)
                return false;

            bool digitExclude = true;
            bool delimFound = false;

            for (int i = 0; i < _str.Length; i++)
            {
                if (delimFound)
                {
                    if (_str[i] != ':')
                        return false;
                    delimFound = false;
                    digitExclude = true;
                    continue;
                }

                if (digitExclude)
                {
                    if (char.IsDigit(_str[i]))
                        return false;
                    digitExclude = false;
                }
                else if (char.IsDigit(_str[i]))
                    continue;

                if (char.IsLetter(_str[i]))
                    continue;
                if (_str[i] == '*' || _str[i] == '&')
                    continue;
                if (_str[i] == ':')
                    delimFound = true;
            }

            return true;
        }

        public static int TryParseTemplateSignatures(string _name, ref List<TemplateSignatureParam>? _paramsList)
        {
            if (_paramsList == null)
                _paramsList = new ();
            else if (_paramsList.Count > 0)
            {
                Debug.Assert(false, "빈거 넣으삼");
                return 777;
            }

            int start = _name.IndexOf('<');
            int end = _name.LastIndexOf('>');

            if (start == -1 && end == -1)
                return 0;
            if (start == -1)
                return 1;
            if (end == -1)
                return 2;
            if (start >= end)
                return 3;

            string paramStr = _name.Substring(start + 1, end - start - 1);
            string[] tokens = paramStr.Split(',');

            foreach (var token in tokens)
            {
                string trimmedToken = token.Trim();
                if (string.IsNullOrEmpty(trimmedToken))
                    return 4;

                TemplateSignatureParam param = new TemplateSignatureParam();

                if (trimmedToken == "*")
                {
                    param.type_ = TemplateSignatureParamType.Placeholder;
                }
                else
                {
                    if (primitiveTypes.TryGetValue(trimmedToken, out TemplateSingatureParamBasicType bt))
                    {
                        param.type_ = TemplateSignatureParamType.Basic;
                        param.basicType_ = bt;
                    }
                    else
                    {
                        trimmedToken = RemoveAllBlanks(trimmedToken);
                        if (!IsValidTypeName(trimmedToken))
                            return 3;

                        param.type_ = TemplateSignatureParamType.UDT;
                        param.udtName_ = trimmedToken;
                    }
                }

                _paramsList.Add(param);
            }

            return 0;
        }

        public static void ReplaceLineBreak(this string _str, char _ch)
        {
            Debug.Assert(_str != null);
            if (_str == null)
                throw new ArgumentNullException(_str);

            unsafe
            {
                fixed (char* p = _str)
                {
                    if (p[_str.Length - 1] == '\n')
                    {
                        p[_str.Length - 1] = _ch;
                    }
                }
            }
        }

        public static IPEndPoint ConvertToEp(this string _str)
        {
            string[] ep = _str.Split(':');

            if (ep.Length != 2)
            {
                throw new FormatException("Invalid endpoint format");
            }

            IPAddress? ip;
            if (!IPAddress.TryParse(ep[0], out ip))
            {
                throw new FormatException("Invalid ip-adress");
            }

            int port;
            if (!int.TryParse(ep[1], NumberStyles.None, NumberFormatInfo.CurrentInfo, out port))
            {
                throw new FormatException("Invalid port");
            }

            return new IPEndPoint(ip, port);
        }

        public static byte[] ToByteArray(this string _str)
        {
            byte[] byteArray = new byte[_str.Length * sizeof(char)];

            for (int i = 0; i < _str.Length; i++)
            {
                char c = _str[i];
                byteArray[i * 2] = (byte)(c & 0xFF);                // lb
                byteArray[i * 2 + 1] = (byte)((c >> 8) & 0xFF);     // hb
            }

            return byteArray;
        }

        public static string ToUTF8(this string _str)
        {
            if (_str.Length == 0)
                return string.Empty;
            byte[] bytes = Encoding.Default.GetBytes(_str);
            string utf8String = Encoding.UTF8.GetString(bytes);
            return utf8String;
        }

        public static bool IsEqualNoCase(this string _str, string _target)
        {
            return string.Compare(_str, _target, StringComparison.OrdinalIgnoreCase) == 0;
        }

        public static string[] SplitPropertyPath(this string _str)
        {
            return _str.Split(new [] {'.'});
        }

        public static string[] SplitBreakLine(this string _str)
        {
            return _str.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None);
        }

        public static string LastSubstring(this string _str, string _param, StringComparison _comparison = StringComparison.CurrentCulture)
        {
            int idx = _str.LastIndexOf(_param, _comparison);
            if (idx == -1)
                return _str;
            int paramLen = _param.Length;
            return _str.Substring(idx + paramLen, _str.Length - idx - paramLen);
        }

        public static char LastChar(this string _str) => string.IsNullOrEmpty(_str) ? '-' : _str[_str.Length - 1];

        public static bool IsDirectoryPath(this string _path)
        {
            try
            {
                if (!Directory.Exists(_path))
                    return false;
                FileAttributes attr = File.GetAttributes(_path);
                return attr.HasFlag(FileAttributes.Directory);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static bool IsFilePath(this string _path)
        {
            try
            {
                if (!File.Exists(_path))
                    return false;
                FileAttributes attr = File.GetAttributes(_path);
                return !attr.HasFlag(FileAttributes.Directory);
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static string CreateRepeat(string _s, int _count)
        {
            if (_s.Length == 0)
                return string.Empty;

            StringBuilder builder = new StringBuilder(_s.Length * _count + 20);

            for (int i = 0; i < _count; ++i)
                builder.Append(_s[i]);
            return builder.ToString();
        }

        private static readonly string[] depthStrings_ = new[]
        {
            string.Empty,
            "\t",
            "\t\t",
            "\t\t\t",
            "\t\t\t\t",
            "\t\t\t\t\t",
            "\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t\t",
            "\t\t\t\t\t\t\t\t\t",
        };

        public static string GetDepthString(int _depth)
        {
            if (_depth < 0)
                throw new Exception("인덱스 아옷오브 렌지");
            if (_depth >= depthStrings_.Length)
                throw new Exception("해당 뎁쓰 스트링은 없습니다. 인덱스 아웃오브렌지");

            return depthStrings_[_depth];
        }

        public static void ParseIdentifierAndLine(string _fNameWithLine, ref string _identifier, ref uint _line, string _identifierDesc)
        {
            string[] splts = _fNameWithLine.Split(',');
            if (splts.Length < 2)
                throw new Exception($"{_identifierDesc} 식별자 분석실패: \"식별자,라인\"으로 입력해주세요.");

            _identifier = RemoveAllBlanks(splts[0]);
            uint.TryParse(RemoveAllBlanks(splts[1]), out _line);

            if (string.IsNullOrWhiteSpace(_identifier) || _line == 0)
                throw new Exception($"{_identifierDesc} 식별자 입력이 올바르지 않습니다.");
        }

        // 추가는 무조건하고, 추가하게 될 경우 _limitLength를 초과하는지 여부를 반환한다.
        public static bool AppendLimit(this StringBuilder _builder, string _text, int _limitLength)
        {
            _builder.Append(_text);

            if (_limitLength <= 0)
                return false;
            if (_builder.Length > _limitLength)
                return true;
            return false;
        }

        public static bool AppendLimit(this StringBuilder _builder, char _char, int _limitLength)
        {
            _builder.Append(_char);
            if (_limitLength <= 0)
                return false;
            if (_builder.Length > _limitLength)
                return true;
            return false;
        }

        public static bool IsSequential(this ReadOnlySpan<char> _span, int _start, params char[] _token)
        {
            if (_start < 0 || _start + _token.Length > _span.Length)
                return false;

            for (int i = 0; i < _token.Length; i++)
            {
                if (_span[_start + i] != _token[i])
                    return false;
            }

            return true;
        }

        public static bool SequenceEqual(this Span<char> span, string value)
        {
            if (span.Length != value.Length)
                return false;

            ReadOnlySpan<char> valueSpan = value.AsSpan();
            return span.SequenceEqual(valueSpan);
        }

        public static INumber ParseNumber(this string _str, NumberType _type, NumberStyle _style = NumberStyle.None)
        {
            int baseNum = 10;

            if (_style == NumberStyle.None)
            {
                if (_str.StartsWith("0b"))
                {
                    _str = _str.Substring(2, _str.Length - 2);
                    baseNum = 2;
                }
                else if (_str.StartsWith("0x"))
                {
                    _str = _str.Substring(2, _str.Length - 2);
                    baseNum = 16;
                }
            }
            else if (_style == NumberStyle.Hex)
            {
                baseNum = 16;
            }
            else if (_style == NumberStyle.Binary)
            {
                baseNum = 2;
            }

            try
            {
                switch (_type)
                {
                    case NumberType.Int32:
                        return new Number<int>(Convert.ToInt32(_str, baseNum));
                    case NumberType.Int64:
                        return new Number<long>(Convert.ToInt64(_str, baseNum));
                    case NumberType.Int32U:
                        return new Number<uint>(Convert.ToUInt32(_str, baseNum));
                    case NumberType.Int64U:
                        return new Number<ulong>(Convert.ToUInt64(_str, baseNum));
                    case NumberType.Single:
                        return new Number<float>(Convert.ToSingle(_str));
                    case NumberType.Double:
                        return new Number<double>(Convert.ToDouble(_str));
                    default:
                        throw new ArgumentOutOfRangeException(nameof(_type), _type, null);
                }
            }
            catch
            {
                return new Number<int>(0);
            }
        }

        // 아스키 코드 값이 256보다 큰 문자는 콘솔 너비 2를 차지한다고 가정한다.
        public static int Width(this string str)
        {
            return str.Aggregate(0, (len, ch) => ch > 256 ? len + 2 : len + 1);
        }

        public static string AddSpace(this string str, int count)
        {
            return str + "".PadRight(count);
        }

        public static string AddChar(this string str, char ch, int count)
        {
            return str + "".PadRight(count, ch);
        }

        public static readonly Regex RegexWhiteSpacePattern = new (@"\s+");
        public static string RemoveAllWhiteSpaces(this string _str) => RegexWhiteSpacePattern.Replace(_str, "");

        public static int CountOf(this string _str, char _ch, int _start = 0, int _length = int.MaxValue, List<int>? _indexes = null)
        {
            _length = Math.Min(_length, _str.Length - _start);
            _start = Math.Max(_start, 0);

            int count = 0;
            for (;;)
            {
                int i = _str.IndexOf(_ch, _start, _length);
                if (i == -1)
                    break;
                if (_indexes != null)
                    _indexes.Add(i);

                ++count;
                int prevPos = _start;
                _start = i + 1;
                _length -= _start - prevPos;
                if (_length <= 0)
                    break;
            }
            return count;
        }

        // @https://stackoverflow.com/questions/1359948/why-doesnt-stringbuilder-have-indexof-method
        /// <summary>
        /// Returns the index of the start of the contents in a StringBuilder
        /// </summary>
        /// <param name="value">The string to find</param>
        /// <param name="startIndex">The starting index.</param>
        /// <param name="ignoreCase">if set to <c>true</c> it will ignore case</param>
        /// <returns></returns>
        public static int IndexOf(this StringBuilder sb, string value, int startIndex, bool ignoreCase = false)
        {
            int index;
            int length = value.Length;
            int maxSearchLength = (sb.Length - length) + 1;

            if (ignoreCase)
            {
                for (int i = startIndex; i < maxSearchLength; ++i)
                {
                    if (Char.ToLower(sb[i]) == Char.ToLower(value[0]))
                    {
                        index = 1;
                        while ((index < length) && (Char.ToLower(sb[i + index]) == Char.ToLower(value[index])))
                            ++index;

                        if (index == length)
                            return i;
                    }
                }

                return -1;
            }

            for (int i = startIndex; i < maxSearchLength; ++i)
            {
                if (sb[i] == value[0])
                {
                    index = 1;
                    while ((index < length) && (sb[i + index] == value[index]))
                        ++index;

                    if (index == length)
                        return i;
                }
            }

            return -1;
        }

        public static long GetHashCode64(this string _str)
        {
            if (_str.Length == 0)
                return "".GetHashCode();
            if (_str.Length == 1)
                return "".GetHashCode() | _str[0].GetHashCode();
            int len = _str.Length;
            var s1 = _str.Substring(0, len / 2);
            var s2 = _str.Substring(len / 2);
            return ((long)s1.GetHashCode() << 32) | (uint)s2.GetHashCode();
        }

        public static string Reverse(this string _input)
        {
            if (string.IsNullOrEmpty(_input))
                return _input;

            char[] arr = _input.ToCharArray();
            Array.Reverse(arr);
            return new string(arr);
        }

        public static StringBuilder? Reverse(this StringBuilder sb)
        {
            if (sb == null || sb.Length <= 1)
                return sb;

            int left = 0;
            int right = sb.Length - 1;

            while (left < right)
            {
                (sb[left], sb[right]) = (sb[right], sb[left]);
                left++;
                right--;
            }
            return sb;
        }

        public static IEnumerable<WordWithPos> GetWordEnumerable(this string _str, int _tabSize = 1, bool _startFront = true)
        {
            if (_tabSize <= 0)
                _tabSize = 1;

            var wordsWithOffsets = new List<WordWithPos>();
            var word = new StringBuilder(64);
            int wordStart = -1;
            int tabCount = 0; // 지금까지 만난 탭 개수
            int len = _str.Length;
            int i = _startFront ? 0 : len - 1;

            foreach (char ch in _startFront ? _str : _str.Reverse())
            {
                // 실제 오프셋 계산: 현재 인덱스 + 탭이전까지의 탭 개수 * (_tabSize - 1)
                var realOffset = i + tabCount * (_tabSize - 1); // 탭 크기를 반영한 실제 위치
                bool isWordChar = ch.IsNumeric() || ch.IsAlphabat() || ch == '_' || ch == '@' ||
                                  (ch >= '가' && ch <= '힣');

                if (isWordChar)
                {
                    if (wordStart == -1)
                    {
                        wordStart = realOffset;
                    }

                    word.Append(ch);
                }
                else
                {
                    if (word.Length > 0)
                    {
                        yield return new WordWithPos(
                            _startFront ? wordStart : wordStart - word.Length + 1,
                            _startFront ? word.ToString() : word.Reverse()!.ToString());
                        word.Clear();
                        wordStart = -1;
                    }

                    if (ch == '\t')
                    {
                        tabCount++;
                    }
                }

                if (_startFront)
                {
                    ++i;
                }
                else
                {
                    --i;
                }
            }

            // 마지막 단어 처리
            if (word.Length > 0)
            {
                yield return new WordWithPos(
                    _startFront ? wordStart : wordStart - word.Length + 1,
                    _startFront ? word.ToString() : word.Reverse()!.ToString());
            }
        }

        public static int IndexOfIgnoreSpace(this string _str, string _text)
        {
            if (string.IsNullOrEmpty(_str) || string.IsNullOrEmpty(_text))
                return -1;

            for (int i = 0; i < _str.Length; i++)
            {
                int strIdx = i;
                int textIdx = 0;
                while (strIdx < _str.Length && textIdx < _text.Length)
                {
                    if (char.IsWhiteSpace(_str[strIdx]))
                    {
                        strIdx++;
                        continue;
                    }
                    if (_str[strIdx] != _text[textIdx])
                        break;
                    strIdx++;
                    textIdx++;
                }
                if (textIdx == _text.Length)
                    return i;
            }
            return -1;
        }

        public static string Ellipsis(this string _text, int _maxLength)
        {
            if (string.IsNullOrEmpty(_text) || _maxLength < 0)
                return string.Empty;

            if (_text.Length > _maxLength)
                return _text.Substring(0, _maxLength) + "...";
            return _text;
        }
    }
}
