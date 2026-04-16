/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:21:01 AM
 *
 */

using SGToolsUI.Command;
using SGToolsUI.Model.Setting;
using SGToolsUI.View;

namespace SGToolsUI.ViewModel
{
    public class SettingViewModel
    {
        //////////////////////////////////////////////////////////////////////////////////
        public SettingViewModel()
        {
            Model = new Setting();
            Model.Load();
            Commander = new SettingCommandCenter(this);
        }

        public SettingView View { get; set; } = null!;       // Xaml에서 주입
        public Setting Model { get; }
        public SettingCommandCenter Commander { get; }
    }
}
