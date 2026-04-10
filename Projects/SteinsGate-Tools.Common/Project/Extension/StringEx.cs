/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:55:41 PM
 *
 */

using System;
using System.Text;

using MoreLinq;

using Newtonsoft.Json.Linq;

namespace SGToolsCommon.Extension
{
    public static class StringEx
    {
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

            if (_token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token, nums);
            _num1 = nums[0];
            _num2 = nums[1];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber3(JToken? _token, out int _num1, out int _num2, out int _num3)
        {
            int[] nums = new int[3];

            if (_token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseIntNumber4(JToken? _token, out int _num1, out int _num2, out int _num3, out int _num4)
        {
            int[] nums = new int[4];

            if (_token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)_token, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
            _num4 = nums[3];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseDoubleNumber4(JToken? _token, out double _num1, out double _num2, out double _num3, out double _num4)
        {
            double[] nums = new double[4];

            if (_token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)_token, nums);
            _num1 = nums[0];
            _num2 = nums[1];
            _num3 = nums[2];
            _num4 = nums[3];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ParseDoubleNumber4(JToken? _token, out double _num1, out double _num2, out double _num3)
        {
            double[] nums = new double[3];

            if (_token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)_token, nums);
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
        public static string Unescape(this string _value)
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
    }
}
