/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 7:30:01 PM
 *
 */

using Newtonsoft.Json.Linq;

namespace SGToolsCommon.Extension
{
    public static class JObjectEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static bool TryGetValue<T>(this JObject _obj, string _propName, out T? _value)
        {
            _value = default(T);

            if (_obj.TryGetValue(_propName, out JToken? token))
            {
                _value = token.ToObject<T>();
                return true;
            }

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool TryGetValueDefault<T>(this JObject _obj, string _propName, out T _value, T _defaultValue)
        {
            _value = _defaultValue;

            if (_obj.TryGetValue(_propName, out JToken? token))
            {
                _value = token.ToObject<T>();
                return true;
            }

            return false;
        }
    }
}
