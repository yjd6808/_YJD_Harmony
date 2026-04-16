/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 7:30:01 PM
 *
 */

using Newtonsoft.Json.Linq;
using SGToolsCommon.Base;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace SGToolsCommon.Extension
{
    public interface IJsonBuilder
    {
        // _limit 초과할 시 true 반환
        public bool BuildJson(StringBuilder _builder, int _limit);
    }

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
        public static bool TryGetValueDefault<T>(this JObject _obj, string _propName, out T? _value, T _defaultValue)
        {
            _value = _defaultValue;

            if (_obj.TryGetValue(_propName, out JToken? token))
            {
                _value = token.ToObject<T>();
                return true;
            }

            return false;
        }

        public static T GetValueOrDefault<T>(this JToken _jtoken, string _propertyName, T _defaultValue = default(T)!)
        {
            if (_jtoken is not JObject obj)
                throw new ArgumentException("JObject 타입만 가능함");

            if (obj.TryGetValue(_propertyName, out JToken? value))
            {
                T? r = value.ToObject<T>();
                if (r == null)
                    return _defaultValue;
                return r;
            }
            return _defaultValue;
        }

        public static List<T> LoadFromJArray<T>(this JToken _root, string _name, Optional<bool>? _success = null)
        {
            if (_root is not JObject root)
                throw new ArgumentException("JObject 타입만 가능함");

            bool hasProperty = root.TryGetValue(_name, out JToken? token);
            if (_success != null)
            {
                _success.Value = hasProperty;
            }

            if (!hasProperty)
            {
                return new List<T>();
            }

            if (token != null && token.Type == JTokenType.Array)
            {
                JArray array = (JArray)token;

                if (typeof(T) == typeof(string) || typeof(T).IsPrimitive)
                {
                    return array.ToObject<List<T>>() ?? new List<T>();
                }

                if (typeof(T).IsEnum)
                {
                    List<T> enums = new();
                    foreach (var item in array)
                    {
                        string s = item.ToString();
                        object o = Enum.Parse(typeof(T), s);
                        enums.Add((T)o);
                    }
                    return enums;
                }

                // T가 ISerializableToJObject를 상속받았는가
                if (typeof(ISerializableToJToken).IsAssignableFrom(typeof(T)))
                {
                    List<T> list = new();

                    foreach (var jToken in array)
                    {
                        var obj = jToken as JObject;
                        if (obj == null)
                            throw new Exception("JObject가 아닌 녀석임");

                        T instance = Activator.CreateInstance<T>();
                        if (instance is not ISerializableToJToken serializable)
                            throw new Exception("ISerializableToJObject이 아님");
                        serializable.Deserialize(obj);
                        list.Add(instance);
                    }
                    return list;
                }
            }

            throw new Exception($"JArray를 {typeof(T).Name}리스트로 변환하는데 실패함");
        }

        public static T GetEnumValueOrDefault<T>(this JToken _obj, string _key, T _defaultValue = default(T)) where T : struct
        {
            if (!typeof(T).IsEnum)
                throw new ArgumentException("이넘 전달점..");

            if (_obj is not JObject obj)
                throw new ArgumentException("JObject 타입만 가능함");

            if (obj.TryGetValue(_key, out JToken? value))
            {
                if (Enum.TryParse(value.ToString(), out T result))
                {
                    return result;
                }
            }
            return _defaultValue;
        }

        public static T? GetSerializableObject<T>(this JToken _token, string _name, T? _default = default) where T : ISerializableToJToken
        {
            if (_token is not JObject root)
                throw new ArgumentException("JObject 타입만 가능함");

            bool hasProperty = root.TryGetValue(_name, out JToken? token);
            if (!hasProperty)
                return _default;
            if (token is not JObject obj)
            {
                Debug.Assert(false);
                return _default;
            }

            T instance = Activator.CreateInstance<T>();
            instance.Deserialize(obj);
            return instance;
        }
    }
}

