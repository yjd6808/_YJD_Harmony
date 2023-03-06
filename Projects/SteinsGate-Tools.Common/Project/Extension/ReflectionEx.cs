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

namespace SGToolsCommon.Extension
{
    public static class ReflectionEx
    {
        public static List<PropertyInfo> GetAllProperties(this Type type, BindingFlags flags = BindingFlags.Default)
        {
            return type.GetProperties(flags).ToList();
        }

        public static void PrintAllProperties(this Type type, BindingFlags flags = BindingFlags.Default)
        {
            var list = GetAllProperties(type, flags);
            list.ForEach(p => Debug.WriteLine(p.Name));
        }

        public static void PrintAllPropertiesBrowsable(this Type type, BindingFlags flags = BindingFlags.Default)
        {
            var list = GetAllProperties(type, flags)
                .Where(p =>
                {
                    var attr = p.GetCustomAttributes<BrowsableAttribute>().FirstOrDefault();
                    if (attr == null) return true;
                    return attr.Browsable;
                }).ToList();
            list.ForEach(p => Debug.WriteLine(p.Name));
        }
    }
}
