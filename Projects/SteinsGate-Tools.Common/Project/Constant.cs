/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 1:33:20 PM
 *
 */

using System;
using System.Transactions;
using System.Windows;

namespace SGToolsCommon
{
    public class Constant
    {
        public const double LogListBoxItemHeaderImageSize = 16;
        public const double LogListBoxItemWidthDecrease = 35;

        public const double DragActivateDistance = 5.0;
        public const int TitleLogMaxLength = 70;
        public const int CacheAlignSize = 64;
        public static readonly Duration FadeInDuration = new(TimeSpan.FromMilliseconds(400));

    }
}
