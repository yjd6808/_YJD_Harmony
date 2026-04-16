using System;
using System.Threading.Tasks;

namespace SGToolsCommon.Base.InterruptableTask
{
    public class IntTask
    {
        public IntTask(Task _task, IntCTS _cts, IntTaskResult? _previousResult = null)
        {
            Task           = _task;
            Cts            = _cts;
            PreviousResult = _previousResult;
        }

        public Task Task                    { get; }
        public IntCTS Cts                   { get; }
        public IntTaskResult? PreviousResult { get; }

        public static IntTask Run(int _id, Action<IntCTS> _action) => IntTaskFactory.Instance.Run(_id, _action);
    }
}
