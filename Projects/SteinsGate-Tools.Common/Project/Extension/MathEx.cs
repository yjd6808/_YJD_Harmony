/*
 * 작성자: 윤정도
 * 생성일: 3/17/2023 10:31:34 PM
 *
 */

using System.Linq;

namespace SGToolsCommon.Extension
{
    public static class MathEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static T Min<T>(params T[] _vals)
        {
            return _vals.Min()!;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static T Max<T>(params T[] _vals)
        {
            return _vals.Max()!;
        }
    }
}
