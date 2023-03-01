﻿/*
 * 작성자: 윤정도
 * 생성일: 2/28/2023 12:22:45 AM
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
using SGToolsCommon.Command;
using SGToolsUI.Command.MainViewCommand;
using SGToolsUI.Command.MainViewCommand.Control;
using SGToolsUI.ViewModel;

namespace SGToolsUI.Command
{
    public class MainCommandCenter : CommandCenter
    {
        public MainViewModel ViewModel { get; }

        // =================================================
        // 커맨드 목록
        // =================================================
        public CommandAbstract OpenSetting { get; }
        public CommandAbstract ReloadPackage { get; }
        public CommandAbstract SelectPackage { get; }
        public CommandAbstract SelectElement { get; }
        public CommandAbstract SelectSprite { get; }
        public CommandAbstract DeleteUIElement { get; }
        public CommandAbstract AddUIElement { get; }



        // =================================================
        // 컨트롤 커맨드 목록 (개발자가 직접 호출할 일없는..)
        // =================================================
        public CommandAbstract MouseMoveOnWindow { get; }
        public CommandAbstract SpritePreview { get; }
        public CommandAbstract CanvasWheel{ get; }

        public MainCommandCenter(MainViewModel viewModel)
        {
            ViewModel = viewModel;

            Add(OpenSetting = new OpenSetting(ViewModel));
            Add(ReloadPackage = new ReloadPackage(ViewModel));
            Add(SelectPackage = new SelectPackage(ViewModel) { UseParameter = true });
            Add(SelectElement = new SelectElement(ViewModel) { UseParameter = true });
            Add(SelectSprite = new SelectSprite(ViewModel) { UseParameter = true });
            Add(DeleteUIElement = new DeleteUIElement(ViewModel));
            Add(AddUIElement = new AddUIElement(ViewModel));


            Add(MouseMoveOnWindow = new MouseMoveOnWindow(ViewModel) { UseParameter = true });
            Add(SpritePreview = new SpritePreview(ViewModel) { UseParameter = true });
            Add(CanvasWheel = new CanvasWheel(ViewModel) { UseParameter = true });
        }
    }
}
