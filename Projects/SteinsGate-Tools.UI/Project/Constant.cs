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
        public const double CanvasWidth = ResolutionWidth + 50.0;
        public const double CanvasHeight = ResolutionHeight + 50.0;
        public const double ResolutionRatio = 480.0f / 640.0f;
        public const string SettingFileName = "setting.json";

        public const double LeftPaneWidth = 300;
        public const int ZOrderInterval = 100;

        public static readonly RectThicknessConverter RectThicknessConverter= new ();
        public static readonly UIElementTypeBlackImageConverter UIElementTypeBlackImageConverter = new();
        public static readonly UIElementTypeColoredImageConverter UIElementTypeColoredImageConverter = new();



        public const string IconPath = "pack://application:,,,/SGToolsUI;component/Resource/Icon/";
        public const string UIGroupIconKey = IconPath + "ui_black_group.ico";
        public const string UIButtonIconKey = IconPath + "ui_black_button.ico";
        public const string UICheckBoxIconKey = IconPath + "ui_black_checkbox.ico";
        public const string UIEditBoxIconKey = IconPath + "ui_black_editbox.ico";
        public const string UILabelIconKey = IconPath + "ui_black_label.ico";

        public const string UIColoredGroupIconKey = IconPath + "ui_colored_group.ico";
        public const string UIColoredButtonIconKey = IconPath + "ui_colored_button.ico";
        public const string UIColoredCheckBoxIconKey = IconPath + "ui_colored_checkbox.ico";
        public const string UIColoredEditBoxIconKey = IconPath + "ui_colored_editbox.ico";
        public const string UIColoredLabelIconKey = IconPath + "ui_colored_label.ico";

        public static readonly ImageSource UIGroupIconImageSource = new BitmapImage(new Uri(UIGroupIconKey));
        public static readonly ImageSource UIButtonIconImageSource = new BitmapImage(new Uri(UIButtonIconKey));
        public static readonly ImageSource UICheckBoxIconImageSource = new BitmapImage(new Uri(UICheckBoxIconKey));
        public static readonly ImageSource UIEditBoxIconImageSource = new BitmapImage(new Uri(UIEditBoxIconKey));
        public static readonly ImageSource UILabelIconImageSource = new BitmapImage(new Uri(UILabelIconKey));

        public static readonly ImageSource UIColoredGroupIconImageSource = new BitmapImage(new Uri(UIColoredGroupIconKey));
        public static readonly ImageSource UIColoredButtonIconImageSource = new BitmapImage(new Uri(UIColoredButtonIconKey));
        public static readonly ImageSource UIColoredCheckBoxIconImageSource = new BitmapImage(new Uri(UIColoredCheckBoxIconKey));
        public static readonly ImageSource UIColoredEditBoxIconImageSource = new BitmapImage(new Uri(UIColoredEditBoxIconKey));
        public static readonly ImageSource UIColoredLabelIconImageSource = new BitmapImage(new Uri(UIColoredLabelIconKey));


        public const double DragActivateDistance = 5.0f;
    }
}
