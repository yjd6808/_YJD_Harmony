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
        public const string Title = "슈타인즈게이트 : UITool";

        public const double ResolutionWidth = 640.0f;
        public const double ResolutionHeight = 480.0f;
        public const double CanvasWheelDelta = 10.0;
        public const double CanvasWidth = ResolutionWidth + 50;
        public const double CanvasHeight = ResolutionHeight + 50;
        public const double CanvasAnchorSize = 16;

        public const double PropertyGridHeight = 400;               // 실제프로퍼티 그리드 크기
        public const double PropertyGridLowerInspectorHide = -30;     // 하단 패널 높이 (숨기기 위한 수치)
        public const double PropertyGridUpperInspectorHide = -20;     // 상단 패널 높이 (숨기기 위한 수치)

        public const double ResolutionRatio = 480.0f / 640.0f;
        public const string SettingFileName = "setting.json";
        public const string UIToolDataFileName = "data.json";
        public const string GameDataFileName = "ui.json";
        public const string HeaderFileName = "UIDefine.h";
        public const string BackupDirectoryRoot = "backup";
        public const string BackupDirectoryFmt = "yyyy-MM-dd";
        public const string BackupFileFmt = "HH.mm.ss";
        public const string ImageSaveDirName = "save";

        public const double LeftPaneWidth = 300;
        public const double DragActivateDistance = 5.0f;

        public const int GroupCodeInterval = 1000;                  // 그룹당 코드 부여단위
        public const int CodeAssignerCapacity = 1000;               // 최대 그룹 수


        // 디자이너 윈도우와 실제 운영체제가 그리는 윈도우간의 너비, 높이 차이를 보정하기위한 수치
        public const double VisualHeightDifferenceBetweenDesignerAndReal = 35;
        public const double VisualWidthDifferenceBetweenDesignerAndReal = 16;

        public const string ElementCategoryName = "공통";
        public const int ElementCategoryOrder = 2;

        public const string GroupCategoryName = "그룹";
        public const string ButtonCategoryName = "버튼";
        public const string SpriteCategoryName = "스프라이트";
        public const string LabelCategoryName = "라벨";
        public const string EditBoxCategoryName = "에딧박스";
        public const string CheckBoxCategoryName = "체크박스";
        public const string ToggleButtonCategoryName = "토글버튼";
        public const string ProgressBarCategoryName = "프로그래스바";
        public const string ScrollBarCategoryName = "스크롤바";
        public const int OtherCategoryOrder = 1;

        public const int InvalidValue = -1;
        public const int CacheAlignSize = 64;
        public const bool UseDebugData = false;

        public static readonly Size DefaultVisualSize = new (50, 50);
        public static readonly Size DefaultBoxVisualSize = new(200, 30);
    }
}
