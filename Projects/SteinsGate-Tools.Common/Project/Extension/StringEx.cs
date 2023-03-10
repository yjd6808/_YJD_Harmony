/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:55:41 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection.Metadata.Ecma335;
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
using MoreLinq;
using Newtonsoft.Json.Linq;

namespace SGToolsCommon.Extension
{
    public static class StringEx
    {
       
        public static void ParseIntNumberN(string target, int[] arr)
        {
            string temp = string.Empty;
            int readCount = 0;

            for (int i = 0; i < target.Length; ++i)
            {
                temp += target[i];

                if (target[i] == ' ')
                {
                    arr[readCount++] = int.Parse(temp);
                    temp = string.Empty;
                }
            }
                
            arr[readCount++] = int.Parse(temp);
        }

        public static string NumberNToString<T>(T[] arr)
        {
            string str = string.Empty;
            arr.ForEach((num, index) =>
            {
                str += num;

                if (index < arr.Length - 1)
                    str += ' ';
            });

            return str;
        }

        public static void  ParseDoubleNumberN(string target, double[] arr)
        {
            string temp = string.Empty;
            int readCount = 0;

            for (int i = 0; i < target.Length; ++i)
            {
                temp += target[i];

                if (target[i] == ' ')
                {
                    arr[readCount++] = double.Parse(temp);
                    temp = string.Empty;
                }
            }

            arr[readCount++] = double.Parse(temp);
        }

        public static void ParseIntNumber2(string s, out int num1, out int num2)
        {
            int[] nums = new int[2];

            ParseIntNumberN(s, nums);

            num1 = nums[0];
            num2 = nums[1];
        }


        public static void ParseIntNumber3(string s, out int num1, out int num2, out int num3)
        {
            int[] nums = new int[3];

            ParseIntNumberN(s, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
        }

        public static void ParseIntNumber4(string s, out int num1, out int num2, out int num3, out int num4)
        {
            int[] nums = new int[4];

            ParseIntNumberN(s, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
            num4 = nums[3];
        }

        public static void ParseIntNumber2(JToken? token, out int num1, out int num2)
        {
            int[] nums = new int[2];

            if (token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)token, nums);

            num1 = nums[0];
            num2 = nums[1];
        }


        public static void  ParseIntNumber3(JToken? token, out int num1, out int num2, out int num3)
        {
            int[] nums = new int[3];

            if (token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)token, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
        }

        public static void  ParseIntNumber4(JToken? token, out int num1, out int num2, out int num3, out int num4)
        {
            int[] nums = new int[4];

            if (token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseIntNumberN((string)token, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
            num4 = nums[3];
        }


        public static void  ParseDoubleNumber4(JToken? token, out double num1, out double num2, out double num3, out double num4)
        {
            double[] nums = new double[4];

            if (token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)token, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
            num4 = nums[3];
        }

        public static void  ParseDoubleNumber4(JToken? token, out double num1, out double num2, out double num3)
        {
            double[] nums = new double[3];

            if (token.Type != JTokenType.String)
                throw new Exception("해당 JToken은 문자열 타입이 아닙니다.");

            ParseDoubleNumberN((string)token, nums);

            num1 = nums[0];
            num2 = nums[1];
            num3 = nums[2];
        }


        public static string RandomAlphabatString(int len, bool containNumber = false)
        {
            StringBuilder builder = new StringBuilder(len);
            Random random = new Random((int)DateTime.Now.Ticks);

            for (int i = 0; i < len; i++)
            {
                int idx = 0;
                char ch = 'a';

                if (containNumber)
                {
                    idx = random.Next(0, 2);
                }

                if (idx == 0)
                {
                    ch = (char)('0' + random.Next(0, 10));
                }
                else if (idx == 1)
                {
                    ch = (char)('a' + random.Next(0, 27));
                }

                builder.Append(ch);
            }

            return builder.ToString();
        }


       

    }
}
