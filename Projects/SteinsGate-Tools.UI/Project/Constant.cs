using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using SGToolsCommon.Converter;
using SGToolsUI.Converter;
using SGToolsUI.Model;

namespace SGToolsUI
{
    public static class Constant
    {
        public const double ResolutionWidth = 640.0f;
        public const double ResolutionHeight = 480.0f;
        public const double CanvasWheelDelta = 10.0;
        public const double CanvasWidth = ResolutionWidth + 50;
        public const double CanvasHeight = ResolutionHeight + 50;

        public const double PropertyGridHeight = 360;               // 실제프로퍼티 그리드 크기
        public const double PropertyGridLowerInspectorHide = 0;     // 하단 패널 높이 (숨기기 위한 수치)
        public const double PropertyGridUpperInspectorHide = 0;     // 상단 패널 높이 (숨기기 위한 수치)

        public const double ResolutionRatio = 480.0f / 640.0f;
        public const string SettingFileName = "setting.json";

        public const double LeftPaneWidth = 300;
        public const double DragActivateDistance = 5.0f;

        public const int GroupCodeInterval = 1000;                  // 그룹당 코드 부여단위
        public const int CodeAssignerCapacity = 1000;               // 최대 그룹 수


        public const string ElementCategoryName = "공통";
        public const int ElementCategoryOrder = 2;

        public const string GroupCategoryName = "그룹";
        public const string ButtonCategoryName = "버튼";
        public const int OtherCategoryOrder = 1;
    }
}
