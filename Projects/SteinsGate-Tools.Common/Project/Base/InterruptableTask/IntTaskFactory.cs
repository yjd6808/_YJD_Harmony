using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using SGToolsCommon.Base;

namespace SGToolsCommon.Base.InterruptableTask
{
    public class IntTaskFactory
    {
        public static readonly IntTaskFactory Instance = new();

        private Dictionary<int, JohnScatMan> managed_ = new();
        private object syncRoot_ = new();

        public IntTask Run(int _id, Action<IntCTS> _action)
        {
            using var _ = new DisposableLock(syncRoot_);

            Exception? interruptException = null;
            JohnScatMan? managedTask = null;

            if (managed_.TryGetValue(_id, out JohnScatMan? existing))
            {
                existing.Cts.Source.Cancel();
                try
                {
                    existing.Task.Wait();
                }
                catch (AggregateException ex)
                {
                    if (existing.Task.Status != TaskStatus.Canceled)
                        interruptException = ex.InnerExceptions[0];
                }
            }

            var cts  = new IntCTS();
            var task = Task.Run(() => _action(cts), cts.Source.Token);

            managedTask = new JohnScatMan(task, cts);
            managed_[_id] = managedTask;

            return new IntTask(managedTask.Task, managedTask.Cts, new IntTaskResult(interruptException));
        }

        public class JohnScatMan
        {
            public JohnScatMan(Task _task, IntCTS _cts)
            {
                Task = _task;
                Cts  = _cts;
            }

            public Task Task   { get; }
            public IntCTS Cts  { get; }
        }
    }
}
