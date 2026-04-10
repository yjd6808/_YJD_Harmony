/*
 * 작성자: 윤정도
 * 생성일: 3/2/2023 4:54:12 AM
 *
 */

using SGToolsCommon.Primitive;

namespace SGToolsUI.Model.Main
{
    public class CanvasLine : CanvasShape
    {
        private IntPoint start_;
        private IntPoint end_;

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint Start
        {
            get => start_;
            set
            {
                start_ = value;
                OnPropertyChanged();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public IntPoint End
        {
            get => end_;
            set
            {
                end_ = value;
                OnPropertyChanged();
            }
        }

        public override bool IsLine => true;
        public override ShapeElementType ShapeElementType => ShapeElementType.Line;
    }
}
