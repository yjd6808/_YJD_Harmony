/*
 * 작성자: 윤정도
 *
 */

using System.Windows;
using System.Windows.Controls;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;

namespace SGToolsUI.View
{
    /// <summary>
    /// 모달 윈도우임
    /// </summary>
    public partial class CreateElementView : Window
    {
        public SGUIGroup SelectedGroup { get; }
        public SgaSprite DroppedSprite { get; }
        public SGUISpriteInfo DroppedSpriteInfo { get; }
        public IntPoint DroppedPositionOnCanvas { get; }

        //////////////////////////////////////////////////////////////////////////////////
        public CreateElementView(SGUIGroup _group, SgaSprite _sprite, IntPoint _droppedPosOnCanvas)
        {
            SelectedGroup = _group;
            DroppedSprite = _sprite;
            DroppedSpriteInfo = new SGUISpriteInfo(_sprite);
            DroppedPositionOnCanvas = _droppedPosOnCanvas;

            InitializeComponent();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void CreateElementView_OnLoaded(object _sender, RoutedEventArgs _e)
        {
            this.MoveToClosestDisplayCenter();
            this.SizeToContent = SizeToContent.Manual;
        }

        //////////////////////////////////////////////////////////////////////////////////
        private async void CreateElement_OnClick(object _sender, RoutedEventArgs _e)
        {
            Button btn = _sender as Button;
            if (btn is null) return;

            SGUIElementType type = (SGUIElementType)btn.Tag;
            SGUIElement newElement = SGUIElement.Create(type);
            newElement.CreateInit();
            newElement.ViewModel = SelectedGroup.ViewModel;
            newElement.VisualPosition = DroppedPositionOnCanvas;

            switch (newElement)
            {
                case SGUIButton button:
                    ApplyButton(button);
                    break;
                case SGUISprite sprite:
                    sprite.Sprite = DroppedSpriteInfo;
                    break;
                case SGUIToggleButton toggleButton:
                    toggleButton.Normal = DroppedSpriteInfo;
                    break;
                case SGUIProgressBar progressBar:
                    progressBar.Sprite = DroppedSpriteInfo;
                    break;
                case SGUIScrollBar scrollBar:
                    scrollBar.UpNormal = DroppedSpriteInfo;
                    break;
            }

            SelectedGroup.AddChild(newElement);
            DialogResult = true;
            Close();
            await SelectedGroup.ViewModel.Saver.BackupAsync($"{type} 드랍 생성");
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ApplyButton(SGUIButton _button)
        {
            SgaImage img = DroppedSprite.Parent;

            _button.Normal = DroppedSpriteInfo;

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 1))
                _button.Over = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 1) as SgaSprite);

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 2))
                _button.Pressed = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 2) as SgaSprite);

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 3))
                _button.Disabled = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 3) as SgaSprite);
        }
    }
}
