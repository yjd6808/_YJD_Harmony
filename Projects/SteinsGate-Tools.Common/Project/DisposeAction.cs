/*
 * 작성자: 윤정도
 * 생성일: 3/7/2023 1:19:17 PM
 *
 */

using System;

namespace SGToolsCommon
{
    public class DisposeAction : IDisposable
    {
        private Action action_;

        //////////////////////////////////////////////////////////////////////////////////
        public DisposeAction(Action _action) => action_ = _action;

        //////////////////////////////////////////////////////////////////////////////////
        public void Dispose()
        {
            if (action_ == null)
                return;

            action_();
        }
    }
}
