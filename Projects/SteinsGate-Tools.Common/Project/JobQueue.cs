/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 10:44:24 AM
 * 소비자1개인 비동기 순차 작업처리용 잡뀨
 * 구현한 이유: 파일저장기능을 비동기로 수행할건데 순차적으로 저장되는게 보장되어야하기 때문에
 *
 * 사용법
 * ViewModel.JobQueue.Enqueue(() =>
 * {
 *     Thread.Sleep(1000);
 *     Debug.WriteLine("안녕하세요1");
 * }, exception => MessageBox.Show("오류가 발생하였습니다.1")).ContinueWith(task => Debug.WriteLine("작업 완료1"));
 * ViewModel.JobQueue.Enqueue(() =>
 * {
 *     Thread.Sleep(1000);
 *     Debug.WriteLine("안녕하세요2");
 * }, exception => MessageBox.Show("오류가 발생하였습니다.2")).ContinueWith(task => Debug.WriteLine("작업 완료2"));
 *
 */

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace SGToolsCommon
{
    public class JobEvent
    {
        private Action Job { get; }
        private Action<Exception>? ErrorHandler { get; }
        private bool failed_;

        public SemaphoreSlim Notifier { get; }
        public Exception? Exception { get; private set; }
        public bool IsFailed => failed_;
        public bool IsSuccess => !failed_;

        //////////////////////////////////////////////////////////////////////////////////
        public JobEvent(Action _job, SemaphoreSlim _notifier, Action<Exception>? _errorHandler)
        {
            Job = _job;
            ErrorHandler = _errorHandler;
            Notifier = _notifier;
            failed_ = false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Do()
        {
            try
            {
                Job();
                failed_ = false;
            }
            catch (Exception e)
            {
                failed_ = true;
                Exception = e;
                if (ErrorHandler != null)
                    ErrorHandler(e);
            }
            finally
            {
                Notifier.Release(1);
            }
        }
    }

    public class JobQueue : Bindable, IDisposable
    {
        private ConcurrentQueue<JobEvent> jobQueue_;
        private Queue<SemaphoreSlim> notifierPool_;
        private SemaphoreSlim jobSignal_;
        private Thread jobThread_;
        private volatile bool isRunning_;

        public bool Running => isRunning_;
        public bool DoingJob => jobQueue_.Count > 0;

        //////////////////////////////////////////////////////////////////////////////////
        public JobQueue()
        {
            isRunning_ = true;
            notifierPool_ = new Queue<SemaphoreSlim>(32);
            jobQueue_ = new ConcurrentQueue<JobEvent>();
            jobSignal_ = new SemaphoreSlim(0, int.MaxValue);
            jobThread_ = new Thread(JobProcessingRoutine);
            jobThread_.Start();

            for (int i = 0; i < 32; ++i)
                notifierPool_.Enqueue(new SemaphoreSlim(0, 1));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public Task<JobEvent> Enqueue(Action _job, Action<Exception>? _errorHandler = null)
        {
            SemaphoreSlim notifier;

            if (notifierPool_.Count == 0)
                notifier = new SemaphoreSlim(0, 1);
            else
                notifier = notifierPool_.Dequeue();

            JobEvent jobEvent = new JobEvent(_job, notifier, _errorHandler);
            Task<JobEvent> task = Task.Run(() =>
            {
                jobEvent.Notifier.Wait();
                notifierPool_.Enqueue(jobEvent.Notifier);
                return jobEvent;
            });

            jobQueue_.Enqueue(jobEvent);
            jobSignal_.Release(1);

            return task;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Dispose()
        {
            // 내부에서 UI쓰레드 관련 작업 처리를 할 수 있으므로 다른 쓰레드로 Join 하자.
            isRunning_ = false;
            Task.Run(() => jobThread_.Join());
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void JobProcessingRoutine()
        {
            while (true)
            {
                if (!isRunning_)
                    return;

                if (jobSignal_.Wait(50))
                    ProcessJob();
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void ProcessJob()
        {
            if (jobQueue_.TryDequeue(out JobEvent? job))
                job.Do();
        }
    }
}
