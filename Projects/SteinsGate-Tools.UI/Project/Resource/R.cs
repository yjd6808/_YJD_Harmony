/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 11:27:53 AM
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

namespace SGToolsUI.Resource
{
    public class R
    {
        public const string IconPath = "pack://application:,,,/SGToolsUI;component/Resource/Icon/";
        public const string UIGroupIconKey = IconPath + "ui_black_group.ico";
        public const string UIButtonIconKey = IconPath + "ui_black_button.ico";
        public const string UICheckBoxIconKey = IconPath + "ui_black_checkbox.ico";
        public const string UIEditBoxIconKey = IconPath + "ui_black_editbox.ico";
        public const string UILabelIconKey = IconPath + "ui_black_label.ico";
        public const string UISpriteIconKey = IconPath + "ui_black_sprite.ico";

        public const string UIColoredGroupIconKey = IconPath + "ui_colored_group.ico";
        public const string UIColoredButtonIconKey = IconPath + "ui_colored_button.ico";
        public const string UIColoredCheckBoxIconKey = IconPath + "ui_colored_checkbox.ico";
        public const string UIColoredEditBoxIconKey = IconPath + "ui_colored_editbox.ico";
        public const string UIColoredLabelIconKey = IconPath + "ui_colored_label.ico";
        public const string UIColoredSpriteIconKey = IconPath + "ui_colored_sprite.ico";


        public static readonly ImageSource UIGroupIconImageSource = new BitmapImage(new Uri(UIGroupIconKey));
        public static readonly ImageSource UIButtonIconImageSource = new BitmapImage(new Uri(UIButtonIconKey));
        public static readonly ImageSource UICheckBoxIconImageSource = new BitmapImage(new Uri(UICheckBoxIconKey));
        public static readonly ImageSource UIEditBoxIconImageSource = new BitmapImage(new Uri(UIEditBoxIconKey));
        public static readonly ImageSource UILabelIconImageSource = new BitmapImage(new Uri(UILabelIconKey));
        public static readonly ImageSource UISpriteIconImageSource = new BitmapImage(new Uri(UISpriteIconKey));

        public static readonly ImageSource UIColoredGroupIconImageSource = new BitmapImage(new Uri(UIColoredGroupIconKey));
        public static readonly ImageSource UIColoredButtonIconImageSource = new BitmapImage(new Uri(UIColoredButtonIconKey));
        public static readonly ImageSource UIColoredCheckBoxIconImageSource = new BitmapImage(new Uri(UIColoredCheckBoxIconKey));
        public static readonly ImageSource UIColoredEditBoxIconImageSource = new BitmapImage(new Uri(UIColoredEditBoxIconKey));
        public static readonly ImageSource UIColoredLabelIconImageSource = new BitmapImage(new Uri(UIColoredLabelIconKey));
        public static readonly ImageSource UIColoredSpriteIconImageSource = new BitmapImage(new Uri(UIColoredSpriteIconKey));
    }
}
