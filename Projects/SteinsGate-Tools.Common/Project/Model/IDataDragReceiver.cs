/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 12:28:01 AM
 *
 */

using SGToolsCommon.Primitive;

namespace SGToolsCommon.Model
{
    public interface IDataDragReceiver
    {
        void DragEnd(IntPoint _p, object _data);
        bool ContainPoint(IntPoint _p);
    }
}
