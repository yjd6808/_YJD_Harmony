// jdyun 24/04/29(월) - 오프
using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using System.Reflection;

namespace SGToolsCommon.Provider
{
    // @원본 소스코드: https://stackoverflow.com/questions/4306743/wpf-data-binding-how-to-data-bind-an-enum-to-combo-box-using-xaml
    //  내가 추가한 기능: 지정하는 enum 타입만 추가할 수 있도록 _mustInclude 파라미터 추가해줌.
    public class EnumerationProvider
    {
        public static Array GetValues(Type _enumeration, string? _mustInclude = null)
        {
            string[]? mustIncludeEnums = null;

            if (_mustInclude != null)
                mustIncludeEnums = _mustInclude.Split(new char[]{','}, StringSplitOptions.RemoveEmptyEntries);

            Array wArray = Enum.GetValues(_enumeration);
            ArrayList wFinalArray = new ArrayList();
            foreach (Enum wValue in wArray)
            {
                string wValueString = wValue.ToString();
                if (mustIncludeEnums != null && !mustIncludeEnums.Contains(wValueString))
                    continue;

                FieldInfo? fi = _enumeration.GetField(wValueString);
                if (null != fi)
                {
                    BrowsableAttribute[]? wBrowsableAttributes = fi.GetCustomAttributes(typeof(BrowsableAttribute), true) as BrowsableAttribute[];
                    if (wBrowsableAttributes?.Length > 0)
                    {
                        //  If the Browsable attribute is false
                        if (wBrowsableAttributes[0].Browsable == false)
                        {
                            // Do not add the _enumeration to the list.
                            continue;
                        }
                    }

                    bool exist = false;
                    foreach (Enum sub in wFinalArray)
                    {
                        if (wValue.GetHashCode() == sub.GetHashCode())
                        {
                            exist = true;
                            break;
                        }
                    }

                    if (exist)
                    {
                        continue;
                    }
                    wFinalArray.Add(wValue);
                }
            }

            return wFinalArray.ToArray();
        }
    }
}
