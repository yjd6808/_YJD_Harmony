/*
 * 작성자: 윤정도
 * 생성일: 3/18/2023 8:50:26 PM
 *
 */

using System;

namespace SGToolsCommon.Extension
{
    public static class EnumEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static T Parse<T>(string _t)
        {
            return (T)Enum.Parse(typeof(T), _t);
        }
    }
}
