/*
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
        public CommandAbstract ReloadSgaPackage { get; }
        public CommandAbstract SelectSgaPackage { get; }
        public CommandAbstract SelectSgaElement { get; }
        public CommandAbstract SelectSgaSprite { get; }
        public CommandAbstract SearchSgaResource { get; }

        public CommandAbstract DeleteUIElement { get; }
        public CommandAbstract CreateUIElement { get; }
        public CommandAbstract SelectUIElement { get; }
        public CommandAbstract AddUIElement { get; }
        public CommandAbstract PickUIElement { get; }

        public CommandAbstract ToggleCanvasViewport { get; }
        public CommandAbstract ToggleCanvasGrid { get; }
        public CommandAbstract CanvasZoomUp { get; }
        public CommandAbstract CanvasZoomDown { get; }
        public CommandAbstract CanvasZoomReset { get; }
        public CommandAbstract CocosPositionToZero { get; }


        // =================================================
        // 컨트롤 커맨드 목록 (개발자가 직접 호출할 일없는..)
        // =================================================
        public CommandAbstract MouseMoveOnWindow { get; }
        public CommandAbstract SpritePreview { get; }
        public CommandAbstract CanvasZoomWheel{ get; }
        

        public MainCommandCenter(MainViewModel viewModel)
        {
            ViewModel = viewModel;

            Add(OpenSetting = new OpenSetting(ViewModel));
            Add(ReloadSgaPackage = new ReloadSgaPackage(ViewModel));
            Add(SelectSgaPackage = new SelectSgaPackage(ViewModel) { UseParameter = true });
            Add(SelectSgaElement = new SelectSgaElement(ViewModel) { UseParameter = true });
            Add(SelectSgaSprite = new SelectSgaSprite(ViewModel) { UseParameter = true });
            Add(SearchSgaResource = new SearchSgaResource(ViewModel) { UseParameter = true });
            Add(DeleteUIElement = new DeleteUIElement(ViewModel));
            Add(CreateUIElement = new CreateUIElement(ViewModel));
            Add(SelectUIElement = new SelectUIElement(ViewModel) { UseParameter = true });
            Add(AddUIElement = new AddUIElement(ViewModel) { UseParameter = true });
            Add(PickUIElement = new PickUIElement(ViewModel));
            Add(CanvasZoomUp = new CanvasZoomUp(ViewModel));
            Add(CanvasZoomDown = new CanvasZoomDown(ViewModel));
            Add(CanvasZoomReset = new CanvasZoomReset(ViewModel));
            
            Add(ToggleCanvasViewport = new ToggleCanvasViewport(ViewModel));
            Add(ToggleCanvasGrid = new ToggleCanvasGrid(ViewModel));

            Add(CocosPositionToZero = new CocosPositionToZero(ViewModel));

            Add(MouseMoveOnWindow = new MouseMoveOnWindow(ViewModel) { UseParameter = true });
            Add(SpritePreview = new SpritePreview(ViewModel) { UseParameter = true });
            Add(CanvasZoomWheel = new CanvasZoomWheel(ViewModel) { UseParameter = true });
        }
    }
}
