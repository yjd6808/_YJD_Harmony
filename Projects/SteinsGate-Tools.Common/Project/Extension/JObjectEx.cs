/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 7:30:01 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;
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
using Newtonsoft.Json.Linq;

namespace SGToolsCommon.Extension
{
    public static class JObjectEx
    {
        public static bool TryGetValue<T>(this JObject obj, string propName, out T? value)
        {
            value = default(T);

            if (obj.TryGetValue(propName, out JToken? token))
            {
                value = token.ToObject<T>();
                return true;
            }

            return false;
        }

        public static bool TryGetValueDefault<T>(this JObject obj, string propName, out T value, T defaultValue)
        {
            value = defaultValue;

            if (obj.TryGetValue(propName, out JToken? token))
            {
                value = token.ToObject<T>();
                return true;
            }

            return false;
        }
    }
}
