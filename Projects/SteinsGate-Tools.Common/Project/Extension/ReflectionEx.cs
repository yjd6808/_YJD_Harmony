/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 7:08:12 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;

namespace SGToolsCommon.Extension
{
    public static class ReflectionEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static List<PropertyInfo> GetAllProperties(this Type _type, BindingFlags _flags = BindingFlags.Default)
        {
            return _type.GetProperties(_flags).ToList();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void PrintAllProperties(this Type _type, BindingFlags _flags = BindingFlags.Default)
        {
            List<PropertyInfo> list = GetAllProperties(_type, _flags);
            list.ForEach(p => Debug.WriteLine(p.Name));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void PrintAllPropertiesBrowsable(this Type _type, BindingFlags _flags = BindingFlags.Default)
        {
            List<PropertyInfo> list = GetAllProperties(_type, _flags)
                .Where(p =>
                {
                    BrowsableAttribute attr = p.GetCustomAttributes<BrowsableAttribute>().FirstOrDefault();
                    if (attr == null) return true;
                    return attr.Browsable;
                }).ToList();
            list.ForEach(p => Debug.WriteLine(p.Name));
        }
    }
}
