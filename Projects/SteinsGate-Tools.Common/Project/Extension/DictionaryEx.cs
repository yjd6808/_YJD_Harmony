// jdyun 24/04/07(일)
using System;
using System.Collections.Generic;

namespace SGToolsCommon.Extension
{
    public static class DictionaryEx
    {
        public static TValue GetValue<TKey, TValue>(this IDictionary<TKey, TValue> _dictionary, TKey _key)
        {
            if (_dictionary.TryGetValue(_key, out TValue? value))
            {
                return value;
            }

            throw new Exception("딕셔너리에서 Key를 찾지 못했습니다.");
        }

        public static TValue? GetValueOrDefault<TKey, TValue>(this IDictionary<TKey, TValue> _dictionary, TKey _key)
        {
            if (_dictionary.TryGetValue(_key, out TValue? value))
            {
                return value;
            }
            return default;
        }

        public static bool TryAdd<TKey, TValue>(this IDictionary<TKey, TValue> _dictionary, TKey _key, TValue _value, bool _override = false)
        {
            if (_dictionary == null)
            {
                throw new ArgumentNullException(nameof(_dictionary));
            }

            if (!_dictionary.ContainsKey(_key))
            {
                _dictionary.Add(_key, _value);
                return true;
            }

            if (_override)
            {
                _dictionary[_key] = _value;
                return true;
            }

            return false;
        }
    }
}
