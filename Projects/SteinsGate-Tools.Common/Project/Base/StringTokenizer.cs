using System;
using System.Collections.Generic;
using System.Text;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// 문자/문자열 구분자 기반 토크나이저. ParseDictionary 포함.
    /// </summary>
    public struct StringTokenizer
    {
        public const char INVALID_DELIM = '긁';

        public const int DELIM_TYPE_NONE   = -1;
        public const int DELIM_TYPE_CHAR   =  0;
        public const int DELIM_TYPE_STRING =  1;

        private readonly string str_;
        private readonly List<char> delims_;
        private List<string>? strDelims_;
        private int prev_;
        private int cur_;
        private int max_;

        public int Cur  => cur_;
        public int Prev => prev_;

        public int LastOffset = -1;
        public int LastDelimType = -1;
        public char LastDelim = INVALID_DELIM;
        public string LastDelimStr = string.Empty;

        public bool Option_ReturnStart  { get; set; } = false;
        public bool Option_ReturnString { get; set; } = true;

        public StringTokenizer(string str, int _max = int.MaxValue)
        {
            str_       = str ?? string.Empty;
            delims_    = new List<char>(4);
            strDelims_ = null;
            max_       = _max;
            prev_      = 0;
            cur_       = 0;
        }

        public bool IsDelim(char delim) => delims_.Contains(delim);

        public bool IsDelim(string delim)
        {
            if (string.IsNullOrEmpty(delim)) return false;
            if (delim.Length == 1) return IsDelim(delim[0]);
            if (strDelims_ == null) return false;
            return strDelims_.Contains(delim);
        }

        public bool IsEnd() => cur_ >= str_.Length;

        public void AddDelim(char delim)
        {
            if (!IsDelim(delim))
                delims_.Add(delim);
        }

        public void AddDelim(string delim)
        {
            if (string.IsNullOrEmpty(delim))
                throw new Exception("빈 문자열 전달하지마");
            if (delim.Length == 1)
                AddDelim(delim[0]);
            else
            {
                if (!IsDelim(delim))
                {
                    strDelims_ ??= new List<string>(4);
                    strDelims_.Add(delim);
                }
            }
        }

        public void RemoveDelim(char delim)
        {
            if (IsDelim(delim))
                delims_.Remove(delim);
        }

        public void SetCur(int _cur)   => cur_  = _cur;
        public void SetPrev(int _prev) => prev_ = _prev;

        public string? Next()
        {
            LastDelim    = INVALID_DELIM;
            LastDelimStr = string.Empty;
            LastOffset   = -1;
            LastDelimType = DELIM_TYPE_NONE;

            if (cur_ >= str_.Length)
                return null;

            int start = prev_;
            for (; cur_ < str_.Length && cur_ < max_; cur_++)
            {
                char ch = str_[cur_];
                if (IsDelim(ch))
                {
                    LastDelimType = DELIM_TYPE_CHAR;
                    LastDelim = ch;
                    break;
                }

                if (strDelims_ != null)
                {
                    foreach (string strDelim in strDelims_)
                    {
                        if (cur_ + strDelim.Length <= str_.Length &&
                            cur_ + strDelim.Length <= max_ &&
                            str_.Substring(cur_, strDelim.Length) == strDelim)
                        {
                            LastDelimType = DELIM_TYPE_STRING;
                            LastDelimStr  = strDelim;
                            cur_ += strDelim.Length - 1;
                            goto OUTER_BREAK;
                        }
                    }
                }
            }
        OUTER_BREAK:
            string ret = string.Empty;
            if (Option_ReturnString)
                ret = str_.Substring(start, cur_ - start);

            prev_ = cur_ + 1;
            cur_  = prev_;

            LastOffset = Option_ReturnStart ? start : cur_;
            return ret;
        }

        public static Dictionary<string, string> ParseDictionary(string _str)
        {
            var tokenizer = new StringTokenizer(_str);
            tokenizer.AddDelim(':');
            tokenizer.AddDelim(',');
            var dict = new Dictionary<string, string>();
            while (true)
            {
                string? key = tokenizer.Next();
                if (string.IsNullOrEmpty(key)) break;
                if (tokenizer.LastDelim != ':') break;
                key = key.Trim();
                string? value = tokenizer.Next();
                if (value == null || tokenizer.LastDelim != ',') break;
                value = value.Trim();
                dict.Add(key, value);
            }
            return dict;
        }
    }
}
