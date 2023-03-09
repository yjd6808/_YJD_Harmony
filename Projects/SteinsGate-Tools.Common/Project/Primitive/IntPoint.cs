/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 2:59:35 PM
 *
 */


using System.Windows;


namespace SGToolsCommon.Primitive
{
    public struct IntPoint
    {
        public int X { get; set; }
        public int Y { get; set; }

        public IntPoint()
        {
            X = 0;
            Y = 0;
        }

        public IntPoint(int x, int y)
        {
            X = x;
            Y = y;
        }

        public static implicit operator Point(IntPoint p)
        {
            return new Point(p.X, p.Y);
        }

        public static implicit operator IntPoint(Point p)
        {
            return new IntPoint((int)p.X, (int)p.Y);
        }

        public Point ToPoint()
        {
            return new Point(X, Y);
        }

        public override string ToString()
        {
            return $"{X} {Y}";
        }
    }
}
