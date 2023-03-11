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
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;

namespace SGToolsCommon
{
    public class JobEvent
    {
        public JobEvent(Action job, SemaphoreSlim notifier, Action<Exception> errorHandler)
        {
            Job = job;
            ErrorHandler = errorHandler;
            Notifier = notifier;
            _failed = false;
        }

        public void Do()
        {
            try
            {
                Job();
                _failed = false;
            }
            catch (Exception e)
            {
                _failed = true;
                Exception = e;
                if (ErrorHandler != null)
                    ErrorHandler(e);
            }
            finally
            {
                Notifier.Release(1);
            }
        }

        private Action Job { get; }
        private Action<Exception> ErrorHandler { get; }
        public SemaphoreSlim Notifier { get; }
        public Exception Exception { get; private set; }
        public bool IsFailed => _failed;
        public bool IsSuccess => !_failed;
        private bool _failed;
    }




    public class JobQueue : Bindable, IDisposable
    {

        public JobQueue()
        {
            _isRunning = true;
            _notifierPool = new Queue<SemaphoreSlim>(32);
            _jobQueue = new ConcurrentQueue<JobEvent>();
            _jobSignal = new SemaphoreSlim(0, int.MaxValue);
            _jobThread = new Thread(JobProcessingRoutine);
            _jobThread.Start();

            for (int i = 0; i < 32; ++i)
                _notifierPool.Enqueue(new SemaphoreSlim(0, 1));
        }

        public Task<JobEvent> Enqueue(Action job, Action<Exception> errorHandler = null)
        {
            SemaphoreSlim notifier;

            if (_notifierPool.Count == 0)
                notifier = new SemaphoreSlim(0, 1);
            else
                notifier = _notifierPool.Dequeue();

            JobEvent jobEvent = new JobEvent(job, notifier, errorHandler);
            Task<JobEvent> task = Task.Run(() =>
            {
                jobEvent.Notifier.Wait();
                _notifierPool.Enqueue(jobEvent.Notifier);
                return jobEvent;
            });
            
            _jobQueue.Enqueue(jobEvent);
            _jobSignal.Release(1);
            
            return task;
        }



        public void Dispose()
        {
            // 내부에서 UI쓰레드 관련 작업 처리를 할 수 있으므로 다른 쓰레드로 Join 하자.
            _isRunning = false;
            Task.Run(() => _jobThread.Join());
        }

        private void JobProcessingRoutine()
        {
            while (true)
            {
                if (!_isRunning)
                    return;

                if (_jobSignal.Wait(50))
                    ProcessJob();
            }
        }

        private void ProcessJob()
        {
            JobEvent job;

            if (_jobQueue.TryDequeue(out job))
                job.Do();
        }

        public bool Running => _isRunning;
        public bool DoingJob => _jobQueue.Count > 0;

        private ConcurrentQueue<JobEvent> _jobQueue;
        private Queue<SemaphoreSlim> _notifierPool;
        private SemaphoreSlim _jobSignal;
        private Thread _jobThread;
        private volatile bool _isRunning;
    }
}
