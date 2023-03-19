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
using System.Windows.Shapes;
using SGToolsCommon.Extension;
using SGToolsCommon.Primitive;
using SGToolsCommon.Sga;
using SGToolsUI.Model.Main;
using Vanara.PInvoke;

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

        public CreateElementView(SGUIGroup group, SgaSprite sprite, IntPoint droppedPosOnCanvas)
        {
            SelectedGroup = group;
            DroppedSprite = sprite;
            DroppedSpriteInfo = new SGUISpriteInfo(sprite);
            DroppedPositionOnCanvas = droppedPosOnCanvas;

            InitializeComponent();
        }

        private void CreateElementView_OnLoaded(object sender, RoutedEventArgs e)
        {
            this.MoveToClosestDisplayCenter();
            this.SizeToContent = SizeToContent.Manual;
        }

        private async void CreateElement_OnClick(object sender, RoutedEventArgs e)
        {
            Button btn = sender as Button;
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

        private void ApplyButton(SGUIButton button)
        {
            SgaImage img = DroppedSprite.Parent;
            
            button.Normal = DroppedSpriteInfo;

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 1))
                button.Over = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 1) as SgaSprite);

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 2))
                button.Pressed = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 2) as SgaSprite);

            if (img.IsValidSpriteIndex(DroppedSprite.FrameIndex + 3))
                button.Disabled = new SGUISpriteInfo(img.GetSprite(DroppedSprite.FrameIndex + 3) as SgaSprite);
        }
    }
}
