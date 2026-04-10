/*
 * 작성자: 윤정도
 * 생성일: 3/19/2023 12:23:41 AM
 *
 */

namespace SGToolsCommon.Model
{
    public interface IKeyboardInputReceiver
    {
        void OnKeyDown(SGKey _key);
        void OnKeyUp(SGKey _key);
        void OnLostFocus();
    }
}
